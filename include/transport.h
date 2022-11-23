#ifndef TRANSPORT_H
#define TRANSPORT_H

#include "config.h"

#define SRC_ADDR        0x02
#define TEST_DEST_ADDR  0x01
#define TEST_NET_ID     0x12

// Control Byte Constants
#define PROT            0
#define PROT_MASK       0x0f
#define ENCRYPT         4
#define ENCRYPT_MASK    0x0f

// "TCP" Constants
#define CONNECT 0x00
#define ACCEPT  0x01
#define SEND    0x02
#define ACK     0x03
#define NACK    0x04
#define CLOSE   0x05

// Transport Buffer Constants
#define BUFF_SIZE 114

// Timeout Constants
#define TRANS_TIMEOUT   500


// Transport Buffer
extern uint8_t transport_tx_flag;
extern AppData transport_tx_buffer;
extern uint8_t transport_rx_flag;
extern AppData transport_rx_buffer;

// Timeout Variables
extern uint16_t transport_timer;


// State Machine
typedef enum ConnectionState_t { IDLE, CONN_OPEN, CONN_DATA, CONN_FAIL } ConnectionState;
typedef enum ConnectionType_t  { NONE, HOST, CLIENT } ConnectionType;


// Timeout Functions
void transport_timer_reset(void);
void transport_timer_update(void);
uint8_t transport_timeout(void);

// Transport Functions
void transport_handle_rx(Segment seg, ConnectionState *connState, ConnectionType *connType);
void transport_handle_tx(uint8_t *data, uint8_t length, uint8_t address, uint8_t port);
Segment transport_encapsulate_data(void);
uint8_t transport_send_data(void);

#endif