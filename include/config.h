#ifndef CONFIG_H
#define CONFIG_H

#include <avr/io.h>


typedef struct t_Segment {
    uint8_t control;
    uint8_t source;
    uint8_t destination;
    uint8_t length;
    uint8_t* data;
    uint8_t checksum;
} Segment;

typedef struct t_Packet {
    uint8_t control;
    uint8_t source;
    uint8_t destination;
    uint8_t length;
    uint8_t* segment;
    uint8_t checksum;
} Packet;

typedef struct t_Frame {
    uint8_t header;
    uint8_t control;
    uint8_t addressing;
    uint8_t length;
    uint8_t* packet;
    uint8_t checksum;
    uint8_t footer;
} Frame;


#endif