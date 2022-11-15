#include "../include/application.h"


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