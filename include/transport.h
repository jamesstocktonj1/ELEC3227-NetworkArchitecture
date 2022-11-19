#ifndef TRANSPORT_H
#define TRANSPORT_H

#include "config.h"


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

// Timeout Constants
#define TRANS_TIMEOUT   500


// Timeout Variables
extern uint16_t transport_timer;


// State Machine
enum ConnectionState { IDLE, CONN_OPEN, CONN_ACCEPT, CONN_SEND, CONN_DATA, CONN_CLOSE, CONN_FAIL };
enum ConnectionType  { NONE, HOST, CLIENT };


// Timeout Functions
void transport_timer_reset(void);
void transport_timer_update(void);
uint8_t transport_timeout(void);


#endif