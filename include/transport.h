#ifndef TRANSPORT_H
#define TRANSPORT_H

#include "util.h"
#include "application.h"

#include <stdlib.h>


#define TEST_SRC_ADDR   0x02
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

// Resend Constants
#define TRANS_RESEND 5


// Transport Buffer
extern uint8_t transportTxFlag;
extern Segment transportTxSegment;
extern uint8_t transportTxAddress;
extern uint8_t transportTxRetry;
extern uint8_t transportRxFlag;
extern Segment transportRxSegment;

// Timeout Variables
extern uint16_t transport_timer;


// State Machine
typedef enum ConnectionState_t { IDLE, CONN_OPEN, CONN_DATA, CONN_FAIL } ConnectionState;
typedef enum ConnectionType_t  { NONE, HOST, CLIENT } ConnectionType;

// Transport State Machine
extern ConnectionState transportConnectionState;
extern ConnectionType transportConnectionType;


// Timeout Functions
void transport_init(void);
void transport_timer_reset(void);
void transport_timer_update(void);
uint8_t transport_timeout(void);

// Transport Functions
void transport_handle_tx(void);
void transport_handle_rx(void);

#endif