#include "../include/eeprom.h"

#include <avr/eeprom.h>

uint8_t eeprom_get_node_id() {
    uint8_t node_id = eeprom_read_byte(EP_NODE_ID_ADDR);
    return node_id == 0 ? 0x1 : node_id; // Return 0x1 if value is 0
}

void eeprom_set_node_id(uint8_t node_id) {
    if (eeprom_read_byte(EP_NODE_ID_ADDR) != node_id) {
        eeprom_write_byte(EP_NODE_ID_ADDR, node_id);
    }
}