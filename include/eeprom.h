#ifndef EEPROM_H
#define EEPROM_H

#include <stdint.h>

// EEPROM Addresses
#define EP_NODE_ID_ADDR 0x00

uint8_t eeprom_get_node_id();
void eeprom_set_node_id(uint8_t node_id);

#endif // EEPROM_H
