#ifndef CONFIG_H
#define CONFIG_H

#ifndef TEST
#include <avr/io.h>
#include <stdio.h>
#else
#include <stdio.h>
#include <stdint.h>
#endif

// checksum constants
#define CRC_CONSTANT 0x1021

typedef struct {
    uint8_t port;
    uint8_t address;
    uint8_t* data;
    uint8_t length;
    uint8_t keyIndex;
} AppData;

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

// CRC-16 CCITT
uint16_t crc16_update(uint16_t crc, uint8_t a);
uint16_t crc16_compute(uint8_t *data, uint8_t length);

#endif