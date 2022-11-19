#ifndef APPLICATION_H
#define APPLICATION_H

#include "config.h"


// application functions
void set_light(uint8_t lightID, uint8_t lightIntensity);


// support functions
void encrypt_data(uint8_t *data, uint8_t len, uint16_t key);
void decrypt_data(uint8_t *data, uint8_t len, uint16_t key);


#endif