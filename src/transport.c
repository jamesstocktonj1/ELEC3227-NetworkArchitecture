#include "../include/transport.h"


// Transport Buffer
uint8_t transport_tx_flag;
AppData transport_tx_buffer;
uint8_t transport_rx_flag;
AppData transport_rx_buffer;

// Timeout Variables
uint16_t transport_timer = 0;



void transport_timer_reset() {

    transport_timer = TRANS_TIMEOUT;
}

void transport_timer_update() {

    if(transport_timer) {
        transport_timer--;
    }
}

uint8_t transport_timeout() {

    return transport_timer == 0;
}


void transport_handle_rx(Segment seg, ConnectionState *connState, ConnectionType *connType) {

    uint8_t segmentState = seg.control & PROT_MASK;

    switch(*connState) {
        case IDLE:
            // client requests to connect
            if((*connType == NONE) && (segmentState == CONNECT)) {
                *connState = CONN_OPEN;
                *connType = HOST;

                transport_timer_reset();
            }
            // ignore if not connect request
            break;
        case CONN_OPEN:
            // host accepts connection
            if((*connType == CLIENT) && (segmentState == ACCEPT)) {
                *connState = CONN_DATA;
                
                transport_timer_reset();
            }
            else if((*connType == CLIENT) && (segmentState == NACK)) {
                *connState = CONN_FAIL;
                *connType = NONE;
            }
            // client sends data
            else if((*connType == HOST) && (segmentState == SEND)) {
                *connState = CONN_DATA;
                
                transport_timer_reset();
            }
            break;
        case CONN_DATA:
            if((*connType == CLIENT) && (segmentState == ACK)) {
                *connState = IDLE;
                *connType = NONE;
            }
            else if((*connType == CLIENT) && (segmentState == NACK)) {
                *connState = IDLE;
                *connType = NONE;
            }
            else if((*connType == HOST) && (segmentState == CLOSE)) {
                *connState = IDLE;
                *connType = NONE;
            }
            break;
        case CONN_FAIL:
            // connection failure should be handled elsewhere
            break;
    }
}