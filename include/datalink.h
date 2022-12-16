#ifndef DATALINK_H
#define DATALINK_H

#include "util.h"

#define DLL_MAX_PACKET_SIZE 128
#define DLL_MAX_FRAME_SIZE 32-2
#define DLL_MAX_MTU DLL_MAX_FRAME_SIZE-9
#define DLL_MTU 10
#define DLL_RX_TABLE_SIZE 4

#if DLL_MTU > DLL_MAX_MTU
#error DLL_MTU is more than DLL_MAX_MTU
#endif

#define DLL_HEADER 0x02
#define DLL_FOOTER 0x03

#define DLL_BROADCAST_ADDR 0x00

typedef struct {
    uint8_t current_index;
    uint8_t length;
    uint8_t destination;
    uint8_t data[DLL_MAX_PACKET_SIZE];
} dll_packet_buffer_t;

typedef struct {
    uint8_t free;
    uint8_t order; // Higher order is older
    uint8_t src;
    uint8_t seq_num;
    uint8_t length;
    uint8_t data[DLL_MAX_PACKET_SIZE];
} dll_rx_table_entry_t;

/**
 * This function initialised all the static variables for the data link layer
*/
void dll_init();

/**
 * This function adds a net packet to the transmit packet buffer
 * @param data A pointer to the net packet data
 * @param length The length of the net packet
 * @param dest The destination address for the net packet
 * @returns 0 - Not Queued, 1 - Queued
*/
uint8_t dll_queue_tx_net_packet(uint8_t *data, uint8_t length, uint8_t dest);

/**
 * This function tells you if a frame is availiable to transmit
 * @returns 0 - No frame to send, 1 - frames available to send
*/
uint8_t dll_has_tx_frame();

/**
 * This function returns the next frame from the transmit packet buffer
 * @param frame A pointer to the buffer to write the frame to
 * @param node_address The address of this node
 * @returns 0 - No frame to send, n - The length of the frame
*/
uint8_t dll_get_tx_frame(uint8_t *frame, uint8_t src_address);

/**
 * This function should be called when a frame is recieved
 * @param data A pointer to the received frame
 * @param length The length of that frame
 * @param node_address The address of this node
 * @returns 0 - Frame valid, otherwise invalid frame/error
*/
int8_t dll_receive_dll_frame(uint8_t *data, uint8_t length, uint8_t node_address);

/**
 * This function tells you if a packet is available for the network layer
 * @returns 0 - No packet, 1 - Packet available
*/
uint8_t dll_has_rx_packet();

/**
 * This function returns the packet in the packet buffer
 * @param packet A pointer to the buffer to write the packet to
 * @returns 0 - No packet available, n - The length of the packet
*/
uint8_t dll_get_rx_packet(uint8_t *packet);

// The below functions are for internal use only

/**
 * This function inserts a fragment into the receive table
 * @param data A pointer to the start of the data field
 * @param length The length of the fragment
 * @param start_offset The start offset of the fragment
 * @param src The source address
 * @param seq_num The sequence number
 * @param end_flag The end flag
 * @returns 0 - Successfully Inserted, otherwise error
*/
int8_t dll_rx_table_insert_fragment(uint8_t *data, uint8_t length, uint8_t start_offset, uint8_t src, uint8_t seq_num, uint8_t end_flag);

/**
 * This function reorders the rx table from 1
 * This should be called after setting an entry's order to 0, to fix the table ordering
*/
void dll_reorder_rx_table();

#endif