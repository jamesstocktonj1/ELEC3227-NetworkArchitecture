#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "../include/application.h"
#include "../include/transport.h"

void application_demo(void);
void transport_demo(void);

void format_segment(Segment seg, ConnectionType state);


int main() {

    application_init();
    transport_init();

    application_demo();
}



void application_demo() {

    uint8_t testData[4] = {0x00, 0x01, 0x02, 0x03};
    
    printf("\nApplication Layer\n");
    
    printf("Sending Data: ");
    for(int i=0; i<4; i++) {
        printf("0x%02x ", testData[i]);
    }

    application_handle_tx(testData, 4, 0x01, 0x01);

    printf("\nTX Encrypted Data: ");
    for(int i=0; i<4; i++) {
        printf("0x%02x ", applicationTxData[i]);
    }

    // transport section
    transport_demo();


    printf("\nApplication Layer\n");
    printf("\nRX Encrypted Data: ");
    for(int i=0; i<4; i++) {
        printf("0x%02x ", applicationRxData[i]);
    }

    uint8_t testRxData[4] = {0, };
    uint8_t testPort = 0;

    uint8_t rxLength = application_handle_rx(testRxData, &testPort);

    printf("\nRecieved Data: ");
    for(int i=0; i<rxLength; i++) {
        printf("0x%02x ", testData[i]);
    }
    printf("\n");
}

void transport_demo() {
    ConnectionState clientState = IDLE;
    ConnectionType clientType = NONE;

    ConnectionState hostState = IDLE;
    ConnectionType hostType = NONE;

    transportRxAddress = APP_ADDR;

    printf("\n\nTransport Layer\n");
    printf("------------------------------------------------------------------------------\n");
    printf("| Origin | State   | Control | SRC  | DEST | Data        | Length | Checksum |\n");
    printf("------------------------------------------------------------------------------\n");
    
    // Client Request Connection
    transportTxFlag = 0;
    transportConnectionState = clientState;
    transportConnectionType = clientType;

    transport_handle_tx();
    format_segment(transportTxSegment, transportConnectionType);

    clientState = transportConnectionState;
    clientType = transportConnectionType;


    // Host Accept Connection
    transportTxFlag = 0;
    transportRxSegment = transportTxSegment;

    transportConnectionState = hostState;
    transportConnectionType = hostType;

    transport_handle_rx();
    format_segment(transportTxSegment, transportConnectionType);

    hostState = transportConnectionState;
    hostType = transportConnectionType;


    // Client Sends Data
    transportTxFlag = 0;
    transportRxSegment = transportTxSegment;

    transportConnectionState = clientState;
    transportConnectionType = clientType;

    transport_handle_rx();
    format_segment(transportTxSegment, transportConnectionType);
    
    clientState = transportConnectionState;
    clientType = transportConnectionType;


    // Host Acknowledges Data
    transportTxFlag = 0;
    transportRxSegment = transportTxSegment;

    transportConnectionState = hostState;
    transportConnectionType = hostType;

    transport_handle_rx();
    format_segment(transportTxSegment, transportConnectionType);

    hostState = transportConnectionState;
    hostType = transportConnectionType;

    
    // Client Closes Connection
    transportTxFlag = 0;
    transportRxSegment = transportTxSegment;

    transportConnectionState = clientState;
    transportConnectionType = clientType;

    transport_handle_rx();
    format_segment(transportTxSegment, transportConnectionType);

    clientState = transportConnectionState;
    clientType = transportConnectionType;

    printf("------------------------------------------------------------------------------\n");
}

void format_segment(Segment seg, ConnectionType state) {
    if(state == HOST) {
        printf("| Host   |");
    }
    else {
        printf("| Client |");
    }

    switch(seg.control & 0xf) {
        case CONNECT:
            printf(" Connect |");
            break;
        case ACCEPT:
            printf(" Accept  |");
            break;
        case SEND:
            printf(" Send    |");
            break;
        case ACK:
            printf(" Ack     |");
            break;
        case NACK:
            printf(" Nack    |");
            break;
        case CLOSE:
            printf(" Close   |");
            break;
    }


    printf(" 0x%04x  | 0x%02x | 0x%02x | ", seg.control, seg.source, seg.destination);
    
    for(int i=0; i<4; i++) {
        if(i < seg.length) {
            printf("%02x ", seg.data[i]);
        }
        else {
            printf("   ");
        }
    }
    printf("| 0x%02x   | 0x%04x   |\n", seg.length, seg.checksum);
}