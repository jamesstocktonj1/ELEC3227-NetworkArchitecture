#include "../include/transport.h"


// Transport Buffer
uint8_t transportTxFlag;
Segment transportTxSegment;
uint8_t transportTxAddress;
uint8_t transportRxFlag;
Segment transportRxSegment;

// Timeout Variables
uint16_t transport_timer = 0;

// Transport State Machine
ConnectionState transportConnectionState;
ConnectionType transportConnectionType;



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

void transport_init() {
    transportTxFlag = 0;
    transportTxSegment.data = (uint8_t *)malloc(BUFF_SIZE);
    transportTxAddress = 0;

    transportRxFlag = 0;
    transportRxSegment.data = (uint8_t *)malloc(BUFF_SIZE);
    
    transport_timer_reset();
}

void transport_handle_tx() {

}

void transport_handle_rx() {
    uint8_t segmentState = transportRxSegment.control & PROT_MASK;

    switch(transportConnectionState) {
        case IDLE:
            // client requests to connect
            if((transportConnectionType == NONE) && (segmentState == CONNECT)) {
                transportConnectionState = CONN_OPEN;
                transportConnectionType = HOST;

                // reply with accept connection
                if(transportTxFlag == 0) {
                    transportTxFlag = 1;
                    transportTxSegment.control = ACCEPT;
                    transportTxSegment.source = TRANSMIT_PORT;
                    transportTxSegment.destination = LIGHT_PORT;
                    transportTxSegment.length = 0x01;
                    transportTxSegment.data[0] = 0x00;
                    transportTxSegment.checksum = 0x00; // TODO: Checksum
                }

                transport_timer_reset();
            }
            // ignore if not connect request
            break;
        case CONN_OPEN:
            // host accepts connection
            if((transportConnectionType == CLIENT) && (segmentState == ACCEPT)) {
                transportConnectionState = CONN_DATA;
                
                transport_timer_reset();
            }
            else if((transportConnectionType == CLIENT) && (segmentState == NACK)) {
                transportConnectionState = CONN_FAIL;
                transportConnectionType = NONE;
            }
            // client sends data
            else if((transportConnectionType == HOST) && (segmentState == SEND)) {
                transportConnectionState = CONN_DATA;
                
                transport_timer_reset();
            }
            break;
        case CONN_DATA:
            if((transportConnectionType == CLIENT) && (segmentState == ACK)) {
                transportConnectionState = IDLE;
                transportConnectionType = NONE;
            }
            else if((transportConnectionType == CLIENT) && (segmentState == NACK)) {
                transportConnectionState = IDLE;
                transportConnectionType = NONE;
            }
            else if((transportConnectionType == HOST) && (segmentState == CLOSE)) {
                transportConnectionState = IDLE;
                transportConnectionType = NONE;
            }
            break;
        case CONN_FAIL:
            // connection failure should be handled elsewhere
            break;
    }
}