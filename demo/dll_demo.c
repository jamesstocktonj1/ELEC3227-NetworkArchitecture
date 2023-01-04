#include <string.h>
#include <assert.h>

#include "../include/datalink.h"
#include "../include/util.h"

#define FAIL { printf("FAIL\n"); assert(0); return; }
#define PASS printf("PASS\n");

void basic_checks();
void serialisation_check();
void deserialisation_check();

int main() {

    printf("----- DLL DEMO -----\n\n");
    
    basic_checks();
    printf("\n");
    serialisation_check();
    printf("\n");
    deserialisation_check();

    printf("\n------- DONE -------\n");
    return 0;
}

void basic_checks() {
    printf("Performing Basic Checks...\n");
    dll_init();

    printf("Checking if DLL starts with 0 frames to transmit...");
    if (dll_has_tx_frame()) FAIL
    else PASS

    printf("Checking if DLL doesn't have a frame to transmit anyway...");
    if (dll_get_tx_frame(NULL, 0x00)) FAIL
    else PASS

    printf("Checking if DLL starts with 0 packets to receive...");
    if (dll_has_rx_packet()) FAIL
    else PASS

    printf("Checking if DLL doesn't have a packet to receive anyway...");
    if (dll_get_rx_packet(NULL)) FAIL
    else PASS

    printf("Basic Checks Complete!\n");
}

#if DLL_MTU != 10
#error DLL_MTU is not 10
#endif

void serialisation_check() {
    printf("Performing Serialisation Check...\n");
    dll_init();
    char msg[] = "Serialisation Check";
    uint8_t len = sizeof(msg);
    uint8_t dest = 0x5;
    uint8_t src = 0x13;

    printf("Checking DLL can queue a packet...");

    if (dll_queue_tx_net_packet((uint8_t*)msg, len, dest)) PASS
    else FAIL    

    uint8_t frame[DLL_MAX_FRAME_SIZE] = {0};
    uint8_t length = 0;

    // Calculated by hand (Initial SEQ NUM = 0)
    uint8_t frame_1_expected[] = {0x02, 0x01, 0x00, 0x13, 0x05, 0x0a, 0x53, 0x65, 0x72, 0x69, 0x61, 0x6c, 0x69, 0x73, 0x61, 0x74, 0x79, 0xae, 0x03};
    uint8_t frame_2_expected[] = {0x02, 0x01, 0x8a, 0x13, 0x05, 0x0a, 0x69, 0x6f, 0x6e, 0x20, 0x43, 0x68, 0x65, 0x63, 0x6b, 0x00, 0x81, 0x51, 0x03};

    printf("Checking frame #1 is correct...");
    memset(frame, 0, DLL_MAX_FRAME_SIZE); // Clear frame array
    if (!dll_has_tx_frame()) FAIL
    length = dll_get_tx_frame(frame, src);
    if (length != sizeof(frame_1_expected)) FAIL
    if (memcmp(frame, frame_1_expected, length)) FAIL
    else PASS
 
    printf("Checking frame #2 is correct...");
    memset(frame, 0, DLL_MAX_FRAME_SIZE); // Clear frame array
    if (!dll_has_tx_frame()) FAIL
    length = dll_get_tx_frame(frame, src);
    if (length != sizeof(frame_2_expected)) FAIL
    if (memcmp(frame, frame_2_expected, length)) FAIL
    else PASS
    
    // printf("Frame %d :", length);
    // for (uint8_t i = 0; i < length; i++) printf(", 0x%02x", frame[i]);
    // printf("\n");

    printf("Checking if DLL has 0 frames to transmit...");
    if (dll_has_tx_frame()) FAIL
    else PASS

    printf("Checking if DLL doesn't have a frame to transmit anyway...");
    if (dll_get_tx_frame(NULL, 0x00)) FAIL
    else PASS
 

    printf("Serialisation Check Complete!\n");
}

void deserialisation_check() {
    printf("Performing Deserialisation Check...\n");
    dll_init();
    uint8_t mac_addr = 0x5;

    // From 0x13 to 0x5
    char hello_expected[] = "Hello World!!!";
    uint8_t hello_world_1[] = {0x02, 0x01, 0x00, 0x13, 0x05, 0x0a, 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x57, 0x6f, 0x72, 0x6c, 0x7d, 0xda, 0x03};
    uint8_t hello_world_2[] = {0x02, 0x01, 0x8a, 0x13, 0x05, 0x05, 0x64, 0x21, 0x21, 0x21, 0x00, 0x1d, 0x64, 0x03};

    // From 0x42 to 0x5
    char goodbye_expected[] = "Goodbye World!!!";
    uint8_t goodbye_world_1[] = {0x02, 0x01, 0x00, 0x42, 0x05, 0x0a, 0x47, 0x6f, 0x6f, 0x64, 0x62, 0x79, 0x65, 0x20, 0x57, 0x6f, 0x07, 0xe7, 0x03};
    uint8_t goodbye_world_2[] = {0x02, 0x01, 0x8a, 0x42, 0x05, 0x07, 0x72, 0x6c, 0x64, 0x21, 0x21, 0x21, 0x00, 0x1d, 0xc4, 0x03};

    printf("Checking frame deserialises correctly...");
    if (dll_receive_dll_frame(hello_world_1, sizeof(hello_world_1), mac_addr) != 0) FAIL
    else PASS

    printf("Checking frame too short error occurs...");
    uint8_t too_short_frame[] = {0x0, 0x1, 0x2};
    if (dll_receive_dll_frame(too_short_frame, sizeof(too_short_frame), mac_addr) != -1) FAIL
    else PASS

    printf("Checking frame wrong header error occurs...");
    uint8_t wrong_header[] = {0xFF, 0x02, 0x8a, 0x42, 0x05, 0x07, 0x72, 0x6c, 0x64, 0x21, 0x21, 0x21, 0x00, 0x19, 0xc0, 0x03};
    if (dll_receive_dll_frame(wrong_header, sizeof(wrong_header), mac_addr) != -2) FAIL
    else PASS

    printf("Checking frame wrong destination error occurs...");
    uint8_t wrong_dest[] = {0x02, 0x02, 0x8a, 0x42, 0xFF, 0x07, 0x72, 0x6c, 0x64, 0x21, 0x21, 0x21, 0x00, 0x19, 0xc0, 0x03};
    if (dll_receive_dll_frame(wrong_dest, sizeof(wrong_dest), mac_addr) != -3) FAIL
    else PASS

    printf("Checking broadcast messages work...");
    uint8_t broadcast_msg[] = {0x02, 0x02, 0x80, 0x42, 0x00, 0x05, 0x54, 0x65, 0x73, 0x74, 0x00, 0x37, 0xb1, 0x03};
    if (dll_receive_dll_frame(broadcast_msg, sizeof(broadcast_msg), mac_addr) != 0) FAIL
    else PASS

    printf("Checking frame wrong footer error occurs...");
    uint8_t wrong_footer[] = {0x02, 0x02, 0x8a, 0x42, 0x05, 0x07, 0x72, 0x6c, 0x64, 0x21, 0x21, 0x21, 0x00, 0x19, 0xc0, 0xFF};
    if (dll_receive_dll_frame(wrong_footer, sizeof(wrong_footer), mac_addr) != -6) FAIL
    else PASS

    printf("Checking frame wrong checksum error occurs...");
    uint8_t wrong_checksum[] = {0x02, 0x02, 0x8a, 0x42, 0x05, 0x07, 0x72, 0x6c, 0x64, 0x21, 0x21, 0x21, 0x00, 0xFF, 0xFF, 0x03};
    if (dll_receive_dll_frame(wrong_checksum, sizeof(wrong_checksum), mac_addr) != -7) FAIL
    else PASS

    printf("Checking RX table insert error occurs when only 2nd frame arrives...");
    dll_init();
    if (dll_receive_dll_frame(hello_world_2, sizeof(hello_world_2), mac_addr) != -20) FAIL
    else PASS

    
    printf("Checking packet reassembles correctly...");
    dll_init();
    uint8_t packet[128] = {0};
    if (dll_receive_dll_frame(hello_world_1, sizeof(hello_world_1), mac_addr) != 0) FAIL
    if (dll_receive_dll_frame(hello_world_2, sizeof(hello_world_2), mac_addr) != 0) FAIL
    if (!dll_has_rx_packet()) FAIL
    uint8_t length = dll_get_rx_packet(packet);
    if (length != sizeof(hello_expected)) FAIL
    if (memcmp(packet, hello_expected, length) != 0) FAIL
    else PASS

    printf("Checking 2nd packet reassembles correctly...");
    dll_init();
    if (dll_receive_dll_frame(goodbye_world_1, sizeof(goodbye_world_1), mac_addr) != 0) FAIL
    if (dll_receive_dll_frame(goodbye_world_2, sizeof(goodbye_world_2), mac_addr) != 0) FAIL
    if (!dll_has_rx_packet()) FAIL
    length = dll_get_rx_packet(packet);
    if (length != sizeof(goodbye_expected)) FAIL
    if (memcmp(packet, goodbye_expected, length) != 0) FAIL
    else PASS

    // Interleave receiving packets
    printf("Checking 2 packets from different senders can be reassembled at the same time...");
    dll_init();
    
#if DLL_RX_TABLE_SIZE < 2
#error DLL_RX_TABLE_SIZE must be at least 2
#endif

    if (dll_receive_dll_frame(hello_world_1, sizeof(hello_world_1), mac_addr) != 0) FAIL
    if (dll_receive_dll_frame(goodbye_world_1, sizeof(goodbye_world_1), mac_addr) != 0) FAIL
    if (dll_receive_dll_frame(hello_world_2, sizeof(hello_world_2), mac_addr) != 0) FAIL
    if (!dll_has_rx_packet()) FAIL
    length = dll_get_rx_packet(packet);
    if (length != sizeof(hello_expected)) FAIL
    if (memcmp(packet, hello_expected, length) != 0) FAIL
    if (dll_receive_dll_frame(goodbye_world_2, sizeof(goodbye_world_2), mac_addr) != 0) FAIL
    if (!dll_has_rx_packet()) FAIL
    length = dll_get_rx_packet(packet);
    if (length != sizeof(goodbye_expected)) FAIL
    if (memcmp(packet, goodbye_expected, length) != 0) FAIL
    else PASS

    printf("Deserialisation Check Complete!\n");
}
