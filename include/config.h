#ifndef CONFIG_H
#define CONFIG_H

#ifndef TEST
#include <avr/io.h>
#else
#include <stdio.h>
#include <stdint.h>
#endif


typedef struct {
    uint16_t control;
    uint8_t source;
    uint8_t destination;
    uint8_t length;
    uint8_t* data;
    uint16_t checksum;
} Segment;

typedef struct {
    uint16_t control;
    uint8_t source;
    uint8_t destination;
    uint8_t length;
    uint8_t* segment;
    uint16_t checksum;
} Packet;

typedef struct {
    uint8_t header;
    uint16_t control;
    uint16_t addressing;
    uint8_t length;
    uint8_t* packet;
    uint16_t checksum;
    uint8_t footer;
} Frame;


void print_segment(Segment p);
void print_packet(Packet p);
void print_frame(Frame p);

#endif