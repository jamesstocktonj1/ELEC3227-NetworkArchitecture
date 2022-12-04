#include "../include/util.h"


void print_segment(Segment p) {

    printf("Segment: 0x%02x -> 0x%02x  Data Length: 0x%02x\n", p.source, p.destination, p.length);
}

void print_packet(Packet p) {

    printf("Packet:  0x%02x -> 0x%02x  Data Length: 0x%02x\n", p.source, p.destination, p.length);
}

void print_frame(Frame p) {

    printf("Frame:  0x%04x             Data Length: 0x%02x\n", p.addressing, p.length);
}


uint16_t crc16_update(uint16_t crc, uint8_t a) {
    uint8_t i;

    crc ^= a;
    for (i = 0; i < 8; ++i) {

        if (crc & 1) {
            crc = (crc >> 1) ^ CRC_CONSTANT;
        }
        else {
            crc = (crc >> 1);
        }
    }
    return crc;
}

uint16_t crc16_compute(uint8_t *data, uint8_t length) {
    uint8_t i;
    uint16_t crc = 0xffff;

    for(i=0; i<length; i++) {
        crc = crc16_update(crc, data[i]); 
    }

    return (uint16_t)(crc & 0xffff);
}

static long int prng_current = 0;

void prng_seed(long int seed) {
    prng_current = seed;
}

long int prng() {  //random number generator; call with 1 <= x <=M-1
    prng_current = (prng_current >> 16) + ((prng_current << 15) & M)  - (prng_current >> 21) - ((prng_current << 10) & M);
    if (prng_current < 0) prng_current += M;
    return prng_current;
}