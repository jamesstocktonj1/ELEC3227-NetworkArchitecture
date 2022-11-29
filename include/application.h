#ifndef APPLICATION_H
#define APPLICATION_H

#include "util.h"

// Application Port Constants
#define BROADCAST_PORT  0x00
#define LIGHT_PORT      0x01
#define TRANSMIT_PORT   0x02

// Application Constants
#define BUFF_SIZE       114


// Encryption Keys
extern uint16_t *applicationEncryption;

// Application Buffer
extern uint8_t applicationTxFlag;
extern uint8_t *applicationTxData;
extern uint8_t applicationTxLength;
extern uint8_t applicationRxFlag;
extern uint8_t *applicationRxData;
extern uint8_t applicationRxLength;


// Application Functions
void application_init(void);
void application_handle_rx(uint8_t *data, uint8_t length, uint8_t port);
void application_handle_tx(uint8_t *data, uint8_t length, uint8_t port, uint8_t address);

// Support Functions
void encrypt_data(uint8_t *data, uint8_t len, uint16_t key);
void decrypt_data(uint8_t *data, uint8_t len, uint16_t key);

#endif