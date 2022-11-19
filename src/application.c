#include "../include/application.h"


void encrypt_data(uint8_t *data, uint8_t len, uint16_t key) {
    uint8_t i;
    for(i=0; i<len; i++) {
        if((i % 2) == 0) {
            data[i] = data[i] ^ (0xff & (key >> 8));
        }
        else {
            data[i] = data[i] ^ (0xff & key);
        }
    }
}

void decrypt_data(uint8_t *data, uint8_t len, uint16_t key) {
    encrypt_data(data, len, key);
}
