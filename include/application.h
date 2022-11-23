#ifndef APPLICATION_H
#define APPLICATION_H

#include "util.h"


// application functions
void set_light(uint8_t nodeAddress, uint8_t lightID, uint8_t lightIntensity);

uint8_t application_handle_rx(void);
void application_handle_tx(uint8_t *data, uint8_t length, uint8_t port, uint8_t address);


// support functions
void encrypt_data(uint8_t *data, uint8_t len, uint16_t key);
void decrypt_data(uint8_t *data, uint8_t len, uint16_t key);


#endif