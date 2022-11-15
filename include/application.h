#ifndef APPLICATION_H
#define APPLICATION_H

#include "config.h"


#define CRC_CONSTANT 0xA001

// CRC-16 Modbus
uint16_t crc16_update(uint16_t crc, uint8_t a);
uint16_t crc16_compute(uint8_t *data, uint8_t length);

#endif