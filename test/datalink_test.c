#include "datalink_test.h"

#include <string.h>

#include "../include/datalink.h"
#include "../include/util.h"

#define FAIL { printf("FAIL\n"); assert(0); return; }
#define PASS printf("PASS\n");

uint8_t get_next_prng_value();
void serialisation_test();

void main_datalink_test() {

    printf("main_datalink_test\n");
    dll_init();
    printf("PLEASE SEE demo/dll_demo.c\n");
    
}

void serialisation_test() {

}



uint8_t get_next_prng_value() {
    uint8_t seed = prng();
    prng_seed(seed);
    uint8_t x = prng();
    prng_seed(seed);
    return x;
}

void gen_frames() {
    char msg[] = "Goodbye World!!!";
    uint8_t len = sizeof(msg);
    uint8_t dest = 0x05;
    uint8_t src = 0x42;

    if (dll_queue_tx_net_packet((uint8_t*)msg, len, dest)) PASS
    else FAIL    

    uint8_t frame[DLL_MAX_FRAME_SIZE] = {0};
    uint8_t length = 0;

    while (dll_has_tx_frame()) {
        length = dll_get_tx_frame(frame, src);
        printf("Frame %d :", length);
        for (uint8_t i = 0; i < length; i++) printf(", 0x%02x", frame[i]);
        printf("\n");
    }
}