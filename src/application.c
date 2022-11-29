#include "../include/application.h"

// Encryption Keys
uint16_t *applicationEncryption;

// Application Buffer
uint8_t applicationTxFlag;
uint8_t *applicationTxData;
uint8_t applicationTxLength;
uint8_t applicationRxFlag;
uint8_t *applicationRxData;
uint8_t applicationRxLength;

void application_init() {
    applicationTxFlag = 0;
    applicationTxData = (uint8_t *)malloc(BUFF_SIZE);
    applicationTxLength = 0;

    applicationRxFlag = 0;
    applicationRxData = (uint8_t *)malloc(BUFF_SIZE);
    applicationRxLength = 0;
}


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
