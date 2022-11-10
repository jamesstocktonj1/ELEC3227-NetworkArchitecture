#include "../include/config.h"


void print_segment(Segment p) {

    printf("Segment: 0x%02x -> 0x%02x  Data Length: 0x%02x\n", p.source, p.destination, p.length);
}

void print_packet(Packet p) {

    printf("Packet:  0x%02x -> 0x%02x  Data Length: 0x%02x\n", p.source, p.destination, p.length);
}

void print_frame(Frame p) {

    printf("Frame:  0x%04x             Data Length: 0x%02x\n", p.addressing, p.length);
}
