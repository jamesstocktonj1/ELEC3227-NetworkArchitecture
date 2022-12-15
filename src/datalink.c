#include "../include/datalink.h"

#include <string.h>

static dll_packet_buffer_t dll_frame_buffer = {};

static dll_rx_table_entry_t dll_rx_table[DLL_RX_TABLE_SIZE] = {};

static uint8_t dll_sequence_number = 0;

static uint8_t dll_rx_packet[DLL_MAX_PACKET_SIZE] = {};
static uint8_t dll_rx_packet_length = 0;

void dll_init() {
    uint8_t i;
    for (i = 0; i < DLL_RX_TABLE_SIZE; i++) {
        dll_rx_table[i].free = 1;
    }
    dll_rx_packet_length = 0;
}

uint8_t dll_queue_tx_net_packet(uint8_t *data, uint8_t length, uint8_t dest) {
    if (dll_frame_buffer.length != 0) return 0;
    if (length == 0 || length > DLL_MAX_PACKET_SIZE) return 1;

    dll_frame_buffer.current_index = 0;
    dll_frame_buffer.length = length;
    dll_frame_buffer.destination = dest;
    memcpy(&dll_frame_buffer.data, data, length);
    dll_sequence_number++;

    return 1;
}

uint8_t dll_has_tx_frame() {
    return dll_frame_buffer.length != 0;
}

uint8_t dll_get_tx_frame(uint8_t *frame, uint8_t node_address) {
    if (dll_frame_buffer.length == 0) return 0;

    uint8_t end_flag = 0x80;
    uint8_t frag_length = dll_frame_buffer.length - dll_frame_buffer.current_index;
    
    if (frag_length > DLL_MTU) {
        frag_length = DLL_MTU;
        end_flag = 0;
    };

    frame[0] = DLL_HEADER;
    frame[1] = dll_sequence_number;
    frame[2] = end_flag | dll_frame_buffer.current_index;
    frame[3] = node_address;
    frame[4] = dll_frame_buffer.destination;
    frame[5] = frag_length;
    memcpy(&frame[6],
        &dll_frame_buffer.data[dll_frame_buffer.current_index],
        frag_length);
    uint16_t checksum = crc16_compute(frame, 6+frag_length);
    frame[6+frag_length] = ((checksum >> 8) & 0xff);
    frame[6+frag_length+1] = (checksum & 0xff);
    frame[6+frag_length+2] = DLL_FOOTER;
    
    if (end_flag) {
        // Mark frame buffer as empty if last fragment
        dll_frame_buffer.length = 0;
    } else {
        // Update current index of fragment
        dll_frame_buffer.current_index += frag_length;
    }
    
    return 6+frag_length+3;
}

int8_t dll_receive_dll_frame(uint8_t *frame, uint8_t length, uint8_t node_address) {
    // for (uint8_t i = 0; i < length; i++) printf("%02x ", frame[i]);
    // printf("\n");
    if (length < 6+3) return -1; // Below minimum length
    if (frame[0] != DLL_HEADER) return -2; // Invalid Header
    if (frame[4] != DLL_BROADCAST_ADDR && frame[4] != node_address) return -3; // Incorrect destination address
    uint8_t frag_length = frame[5];
    if (frag_length < 1 || frag_length > DLL_MAX_MTU) return -4; // Invalid data length
    if (length < 6+frag_length+3) return -5; // Below minimum length including data
    if (frame[6+frag_length+2] != DLL_FOOTER) return -6; // Invalid Footer
    uint16_t checksum = crc16_compute(frame, 6+frag_length);
    if ((frame[6+frag_length] != ((checksum >> 8) & 0xff)) &&
        (frame[6+frag_length+1] != (checksum & 0xff))) return -7; // Invalid Checksum

    uint8_t seq_number = frame[1];
    uint8_t start_offset = frame[2] & 0x7f;
    uint8_t end_flag = frame[2] & 0x80;
    uint8_t src_addr = frame[3];
    if (start_offset + frag_length > DLL_MAX_PACKET_SIZE) return -8; // Packet length overflow

    if (end_flag && start_offset == 0) {
        if (dll_rx_packet_length != 0) return -9; // RX Packet buffer full
        dll_rx_packet_length = frag_length;
        memcpy(dll_rx_packet, &frame[6], frag_length);
        return 0;
    }
    
    return dll_rx_table_insert_fragment(&frame[6], frag_length, start_offset, src_addr, seq_number, end_flag);
}

uint8_t dll_has_rx_packet() {
    return dll_rx_packet_length != 0;
}

uint8_t dll_get_rx_packet(uint8_t *packet) {
    if (dll_rx_packet_length == 0) return 0;
    uint8_t length = dll_rx_packet_length;

    memcpy(packet, dll_rx_packet, length);
    dll_rx_packet_length = 0;

    return length;
}

int8_t dll_rx_table_insert_fragment(uint8_t *data, uint8_t length, uint8_t start_offset, uint8_t src, uint8_t seq_num, uint8_t end_flag) {
    if (start_offset != 0) {
        uint8_t index;
        for (index = 0; index < DLL_RX_TABLE_SIZE; index++) {
            dll_rx_table_entry_t *entry = &dll_rx_table[index];
            if (entry->free) continue;
            if (entry->src != src) continue;
            if (entry->seq_num != seq_num) continue;
            // printf("start_offset %i %i %i\n", entry->length, start_offset, end_flag);
            if (entry->length != start_offset) continue;
            break;
        }
        if (index >= DLL_RX_TABLE_SIZE) return -20; // Not matching RX table entry
        memcpy(&dll_rx_table[index].data[start_offset], data, length);
        uint8_t new_length = start_offset + length;
        
        if (end_flag) {
            if (dll_rx_packet_length != 0) return -21; // RX Packet buffer full
            dll_rx_packet_length = new_length;
            memcpy(dll_rx_packet, dll_rx_table[index].data, new_length);
            dll_rx_table[index].free = 1;
            return 0;
        }

        dll_rx_table[index].length = new_length;
        dll_rx_table[index].order = 0;
        dll_reorder_rx_table();
        return 0;
    }

    uint8_t index;
    for (index = 0; index < DLL_RX_TABLE_SIZE; index++) {
        dll_rx_table_entry_t *entry = &dll_rx_table[index];
        if (entry->src == src) break;
    }
    if (index >= DLL_RX_TABLE_SIZE) {
        for (index = 0; index < DLL_RX_TABLE_SIZE; index++) {
            dll_rx_table_entry_t *entry = &dll_rx_table[index];
            if (entry->free) break;
        }
    }
    if (index >= DLL_RX_TABLE_SIZE) {
        uint8_t oldest = 0;
        uint8_t i;
        for (i = 0; i < DLL_RX_TABLE_SIZE; i++) {
            dll_rx_table_entry_t *entry = &dll_rx_table[i];
            if (entry->order >= oldest) {
                index = i;
                oldest = entry->order;
            }
        }
    }
    if (index >= DLL_RX_TABLE_SIZE) return -22; // Unable to select rx table entry to overwrite

    // Create new table entry
    dll_rx_table[index].free = 0;
    dll_rx_table[index].order = 0;
    dll_rx_table[index].src = src;
    dll_rx_table[index].seq_num = seq_num;
    dll_rx_table[index].length = length;
    memcpy(dll_rx_table[index].data, data, length);

    dll_reorder_rx_table();
    return 0;
}

void dll_reorder_rx_table() {
    uint8_t increment_next = 0;
    int8_t n;
    for (n = DLL_RX_TABLE_SIZE; n >= 0; n--) {
        uint8_t i;
        for (i = 0; i < DLL_RX_TABLE_SIZE; i++) {
            if (dll_rx_table[i].free) continue;
            if (dll_rx_table[i].order == (uint8_t)n) {
                // Increment if needed and break since we have found the number
                if (increment_next) dll_rx_table[i].order++;
                break;
            };
        }
        if (i == DLL_RX_TABLE_SIZE) increment_next = 1;
    }
}