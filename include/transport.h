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
extern uint8_t transportRxAddress;
extern uint8_t transportErrorFlag;

// Timeout Variables
extern uint16_t transport_timer;


// State Machine
typedef enum ConnectionState_t { IDLE, CONN_OPEN, CONN_DATA, CONN_FAIL } ConnectionState;
typedef enum ConnectionType_t  { NONE, HOST, CLIENT } ConnectionType;

// Transport State Machine
extern ConnectionState transportConnectionState;
extern ConnectionType transportConnectionType;


/**
 * This function initialises all the variables for the transport layer
 * 
 */
void transport_init(void);

/**
 * This function resets the timer for the transport layer
 */
void transport_timer_reset(void);

/**
 * This function decrements the timer for the transport layer
 * It is called in the 1ms hardware timer set in include/timer.h
 * 
 */
void transport_timer_update(void);

/**
 * This function checks whether the transport timer has timed out
 * 
 * @return returns 1 - timed out, 0 - otherwise
 */
uint8_t transport_timeout(void);


/**
 * This functions takes data from the application layer and processes it to the network layer
 * 
 */
void transport_handle_tx(void);

/**
 * This functions takes data from the network layer and processes it to the application layer
 * 
 */
void transport_handle_rx(void);

/**
 * This function checks the transport timer and handles the resend of segments
 * 
 */
void transport_handle_timeout(void);

/**
 * This function checks whether data can be processed from the application layer to the network layer
 * 
 * @return returns 1 - data can be processed, 0 - no data available / cannot process
 */
uint8_t transport_poll_tx(void);

/**
 * This function checks whether data can be processed from the network layer to the application layer
 * 
 * @return returns 1 - data can be processed, 0 - no data available / cannot process 
 */
uint8_t transport_poll_rx(void);

#endif