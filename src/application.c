#include "../include/application.h"

#include <string.h>

// Encryption Keys
uint16_t applicationEncryption[16] PROGMEM = {
    0xce9d, 0x311c,
    0xfc5d, 0x8059,
    0x1231, 0xc579,
    0x6013, 0x2f04,
    0x766f, 0xfca3,
    0x5eae, 0x88fd,
    0x8241, 0x6e9f,
    0x8865, 0x72fa
};

// Application Buffer
uint8_t applicationTxFlag;
uint8_t *applicationTxData;
uint8_t applicationTxLength;
uint8_t applicationTxPort;
uint8_t applicationTxAddress;
uint8_t applicationTxEncryption;
uint8_t applicationRxFlag;
uint8_t *applicationRxData;
uint8_t applicationRxLength;
uint8_t applicationRxPort;
uint8_t applicationRxEncryption;


void application_init() {
    applicationTxFlag = 0;
    applicationTxData = (uint8_t *)malloc(BUFF_SIZE);
    applicationTxLength = 0;
    applicationTxPort = 0;
    applicationTxAddress = 0;
    applicationTxEncryption = 0;

    applicationRxFlag = 0;
    applicationRxData = (uint8_t *)malloc(BUFF_SIZE);
    applicationRxLength = 0;
    applicationRxPort = 0;
}


uint8_t application_handle_rx(uint8_t *data, uint8_t *port) {
    if(applicationRxFlag) {
        decrypt_data(applicationRxData, applicationRxLength, applicationEncryption[applicationRxEncryption]);
        memcpy(data, applicationRxData, applicationRxLength);
        
        applicationRxFlag = 0;
        return applicationRxLength;
    }
    return 0;
}

uint8_t application_handle_tx(uint8_t *data, uint8_t length, uint8_t port, uint8_t address) {
    if(applicationTxFlag == 0) {
        memcpy(applicationTxData, data, length);
        applicationTxLength = length;
        applicationTxPort = port;
        applicationTxAddress = address;
        applicationTxFlag = 1;

        applicationTxEncryption = (applicationTxEncryption + 1) & 0xf;
        encrypt_data(applicationTxData, applicationTxLength, applicationEncryption[applicationTxEncryption]);
    }
    return applicationTxFlag;
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
