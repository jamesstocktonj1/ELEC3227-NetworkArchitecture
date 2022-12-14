#include "../include/transport.h"

#include <string.h>

// Transport Buffer
uint8_t transportTxFlag;
Segment transportTxSegment;
uint8_t transportTxAddress;
uint8_t transportTxRetry;
uint8_t transportRxFlag;
Segment transportRxSegment;
uint8_t transportRxAddress;
uint8_t transportErrorFlag;

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

    // request open connection
    if((transportTxFlag == 0) && (applicationTxFlag) && (transportConnectionState == IDLE) && (transportConnectionType == NONE)) {

        transportConnectionState = CONN_OPEN;
        transportConnectionType = CLIENT;

        transportTxFlag = 1;
        transportTxRetry = TRANS_RESEND;
        transportTxSegment.control = (NET_ID << 8) | CONNECT;
        transportTxSegment.source = TRANSMIT_PORT;
        transportTxSegment.destination = applicationTxPort;
        transportTxAddress = applicationTxAddress;
        transportTxSegment.length = 0x01;
        transportTxSegment.data[0] = 0x00;
        transportTxSegment.checksum = transport_crc(transportTxSegment);
    }

    transport_timer_reset();
}

void transport_handle_timeout() {

    // check for message timeout
    if(transport_timeout() && (transportConnectionState != IDLE)) {

        // on connection timeout / resend handler (and not error flag)
        if(transportTxRetry && (transportErrorFlag == 0)) {
            // resend segment
            transportTxFlag = 1;
            transportTxRetry--;

            transport_timer_reset();
        }
        else {
            transportConnectionState = IDLE;
            transportConnectionType = NONE;
            transportTxFlag = 0;
        }
    }
}

void transport_handle_rx() {
    uint8_t segmentState = transportRxSegment.control & PROT_MASK;

    // filter RX Address
    if(transportRxAddress != APP_ADDR) {
        return;
    }

    if((transportRxSegment.control >> 8) != NET_ID){
        printf("Transport Error: Mismatching Network ID\n");
        return;
    }

    // filter RX Port
    if((transportConnectionState != IDLE) && (transportRxSegment.destination != applicationTxPort)) {
        printf("Transport Error: Mismatch of Port\n");
        return;
    }

    if(transport_crc(transportRxSegment) != transportRxSegment.checksum) {
        printf("Transport Error: Checksum Mismatch\n");
        return;
    }

    switch(transportConnectionState) {
        case IDLE:
            // client requests to connect
            if((transportConnectionType == NONE) && (segmentState == CONNECT)) {

                // host accepts connection
                if(transportTxFlag == 0) {
                    transportConnectionState = CONN_OPEN;
                    transportConnectionType = HOST;

                    transportTxFlag = 1;
                    transportTxRetry = TRANS_RESEND;
                    transportTxSegment.control = (NET_ID << 8) | (applicationTxEncryption << 4) | ACCEPT;
                    transportTxSegment.source = TRANSMIT_PORT;
                    transportTxSegment.destination = applicationTxPort;
                    transportTxAddress = applicationTxAddress;
                    transportTxSegment.length = 0x01;
                    transportTxSegment.data[0] = 0x00;
                    transportTxSegment.checksum = transport_crc(transportTxSegment);
                }

                transport_timer_reset();
            }
            // ignore if not connect request
            break;
        case CONN_OPEN:
            // host accepts connection
            if((transportConnectionType == CLIENT) && (segmentState == ACCEPT)) {

                // client replies with data
                if(transportTxFlag == 0) {
                    transportConnectionState = CONN_DATA;

                    applicationRxEncryption = (transportRxSegment.control >> 4) & 0xf;

                    transportTxFlag = 1;
                    transportTxRetry = TRANS_RESEND;
                    transportTxSegment.control = (NET_ID << 8) | SEND;
                    transportTxSegment.source = TRANSMIT_PORT;
                    transportTxSegment.destination = applicationTxPort;
                    transportTxAddress = applicationTxAddress;
                    transportTxSegment.length = applicationTxLength;
                    memcpy(transportTxSegment.data, applicationTxData, applicationTxLength);
                    transportTxSegment.checksum = transport_crc(transportTxSegment);
                }
                
                transport_timer_reset();
            }
            else if((transportConnectionType == CLIENT) && (segmentState == NACK)) {
                transportConnectionState = CONN_FAIL;
                transportConnectionType = NONE;
            }
            // client sends data
            else if((transportConnectionType == HOST) && (segmentState == SEND)) {
                
                // host (n)acknowledges data
                if(transportTxFlag == 0) {
                    transportConnectionState = CONN_DATA;

                    memcpy(applicationRxData, transportRxSegment.data, transportRxSegment.length);
                    applicationRxLength = transportRxSegment.length;
                    applicationRxFlag = 1;
                    
                    transportTxFlag = 1;
                    transportTxRetry = TRANS_RESEND;
                    transportTxSegment.control = (NET_ID << 8) | ACK;
                    transportTxSegment.source = TRANSMIT_PORT;
                    transportTxSegment.destination = applicationTxPort;
                    transportTxAddress = applicationTxAddress;
                    transportTxSegment.length = 0x01;
                    transportTxSegment.data[0] = 0x00;
                    transportTxSegment.checksum = transport_crc(transportTxSegment);
                }
                
                transport_timer_reset();
            }
            break;
        case CONN_DATA:
            if((transportConnectionType == CLIENT) && (segmentState == ACK)) {
                
                // client closes connection
                if(transportTxFlag == 0) {
                    transportConnectionState = IDLE;
                    transportConnectionType = NONE;

                    applicationTxFlag = 0;
                    
                    transportTxFlag = 1;
                    transportTxRetry = TRANS_RESEND;
                    transportTxSegment.control = (NET_ID << 8) | CLOSE;
                    transportTxSegment.source = TRANSMIT_PORT;
                    transportTxSegment.destination = applicationTxPort;
                    transportTxAddress = applicationTxAddress;
                    transportTxSegment.length = 0x01;
                    transportTxSegment.data[0] = 0x00;
                    transportTxSegment.checksum = transport_crc(transportTxSegment);
                }
            }
            else if((transportConnectionType == CLIENT) && (segmentState == NACK)) {

                // client closes connection
                if(transportTxFlag == 0) {
                    transportConnectionState = IDLE;
                    transportConnectionType = NONE;
                    
                    transportTxFlag = 1;
                    transportTxSegment.control = (NET_ID << 8) | CLOSE;
                    transportTxSegment.source = TRANSMIT_PORT;
                    transportTxSegment.destination = applicationTxPort;
                    transportTxAddress = applicationTxAddress;
                    transportTxSegment.length = 0x01;
                    transportTxSegment.data[0] = 0x00;
                    transportTxSegment.checksum = transport_crc(transportTxSegment);
                }
            }
            else if((transportConnectionType == HOST) && (segmentState == CLOSE)) {
                transportConnectionState = IDLE;
                transportConnectionType = NONE;
            }
            break;
        case CONN_FAIL:
            break;
    }
}

uint8_t transport_poll_tx() {
    return (transportTxFlag == 0) && (applicationTxFlag);
}

uint8_t transport_poll_rx() {
    transport_handle_timeout();
    return (transportRxFlag) && (applicationRxFlag == 0);
}

uint16_t transport_crc(Segment data) {
    uint8_t *temp = (uint8_t *)malloc(data.length + 5);

    temp[0] = (data.control >> 8);
    temp[1] = (uint8_t)data.control;
    temp[2] = data.source;
    temp[3] = data.destination;
    temp[4] = data.length;

    memcpy(temp+5, data.data, data.length);

    return crc16_compute(temp, data.length + 5);
}