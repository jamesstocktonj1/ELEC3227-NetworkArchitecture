#ifndef APPLICATION_H
#define APPLICATION_H

#include "util.h"

#include <stdlib.h>

// Application Port Constants
#define BROADCAST_PORT  0x00
#define LIGHT_PORT      0x01
#define TRANSMIT_PORT   0x02

// Application RX Address

// Application Constants
#define BUFF_SIZE       114


// Encryption Keys
extern uint16_t *applicationEncryption;

// Application Buffer
extern uint8_t applicationTxFlag;
extern uint8_t *applicationTxData;
extern uint8_t applicationTxLength;
extern uint8_t applicationTxPort;
extern uint8_t applicationTxAddress;
extern uint8_t applicationRxFlag;
extern uint8_t *applicationRxData;
extern uint8_t applicationRxLength;
extern uint8_t applicationRxPort;


/**
 * This function initialises all the variables for the application layer
 * 
 */
void application_init(void);

/**
 * This function handles the recieving data from the transport layer
 * 
 * @param data      - pointer to data
 * @param port      - pointer to destiantion address
 * @return          - returns length of data
 */
uint8_t application_handle_rx(uint8_t *data, uint8_t *port);

/**
 * This function handles the sending of data to the transport layer
 * 
 * @param data      - pointer to data
 * @param length    - length of data
 * @param port      - destination port
 * @param address   - destination address
 */
uint8_t application_handle_tx(uint8_t *data, uint8_t length, uint8_t port, uint8_t address);


/**
 * This function handles the encryption of data
 * 
 * @param data  - pointer to data
 * @param len   - length of data
 * @param key   - encryption key
 */
void encrypt_data(uint8_t *data, uint8_t len, uint16_t key);

/**
 * This function handles the decryption of data
 * 
 * @param data  - pointer to data
 * @param len   - length of data
 * @param key   - encryption key
 */
void decrypt_data(uint8_t *data, uint8_t len, uint16_t key);

#endif