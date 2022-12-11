#include "transport_test.h"


void main_transport_test() {

    fprintf(stderr, "main_transport_test\n");

    transport_state_machine_test();
    transport_timeout_test();
}


void transport_state_machine_test() {

    fprintf(stderr, "  RUN:  transport_state_machine_test\n");

    transport_init();

    transportRxAddress = APP_ADDR;

    transport_state_connect_test();
    transport_state_accept_test();
    transport_state_send_test();
    transport_state_ack_test();
    transport_state_nack_test();
    transport_state_close_test();

    transport_state_connect_nack_test();

    transport_timeout_connect_test();
    transport_timeout_accept_test();
    transport_timeout_send_test();
    transport_timeout_ack_test();

    transport_communication_test();

    transport_crc_test();

    fprintf(stderr, "  PASS: transport_state_machine_test\n");
}

void transport_state_connect_test() {

    transportRxSegment.control = (TEST_NET_ID << 8) | CONNECT;
    transportRxSegment.source = TEST_SRC_ADDR;
    transportRxSegment.destination = TEST_DEST_ADDR;
    transportRxSegment.checksum = transport_crc(transportRxSegment);
    applicationTxPort = LIGHT_PORT;

    transportConnectionState = IDLE;
    transportConnectionType = NONE;

    transportTxFlag = 0;

    transport_handle_rx();

    if(transportConnectionState != CONN_OPEN) {
        fprintf(stderr, "  FAIL: transport_state_connect_test\n");
        fprintf(stderr, "  Connection State failed to transition to CONN_OPEN after connection CONNECT\n");
        assert(0);
    }

    if(transportConnectionType != HOST) {
        fprintf(stderr, "  FAIL: transport_state_connect_test\n");
        fprintf(stderr, "  Connection Type failed to transition to HOST after connection CONNECT\n");
        assert(0);
    }

    if(transportTxSegment.control != ACCEPT) {
        fprintf(stderr, "  FAIL: transport_state_connect_test\n");
        fprintf(stderr, "  Connection Reply failed to reply with ACCEPT\n");
        assert(0);
    }

    fprintf(stderr, "  PASS: transport_state_connect_test\n");
}

void transport_state_accept_test() {

    transportRxSegment.control = (TEST_NET_ID << 8) | ACCEPT;
    transportRxSegment.source = TEST_SRC_ADDR;
    transportRxSegment.destination = TEST_DEST_ADDR;
    transportRxSegment.checksum = transport_crc(transportRxSegment);
    applicationTxPort = LIGHT_PORT;

    transportConnectionState = CONN_OPEN;
    transportConnectionType = CLIENT;

    transportTxFlag = 0;

    transport_handle_rx();

    if(transportConnectionState != CONN_DATA) {
        fprintf(stderr, "  FAIL: transport_state_accept_test\n");
        fprintf(stderr, "  Connection State failed to transition to CONN_DATA after connection ACCEPT\n");
        assert(0);
    }

    if(transportConnectionType != CLIENT) {
        fprintf(stderr, "  FAIL: transport_state_accept_test\n");
        fprintf(stderr, "  Connection Type failed to transition to CLIENT after connection ACCEPT\n");
        assert(0);
    }

    if(transportTxSegment.control != SEND) {
        fprintf(stderr, "  FAIL: transport_state_connect_test\n");
        fprintf(stderr, "  Connection Reply failed to reply with SEND\n");
        assert(0);
    }

    fprintf(stderr, "  PASS: transport_state_accept_test\n");
}

void transport_state_send_test() {

    transportRxSegment.control = (TEST_NET_ID << 8) | SEND;
    transportRxSegment.source = TEST_SRC_ADDR;
    transportRxSegment.destination = TEST_DEST_ADDR;
    transportRxSegment.checksum = transport_crc(transportRxSegment);
    applicationTxPort = LIGHT_PORT;

    transportConnectionState = CONN_OPEN;
    transportConnectionType = HOST;

    transportTxFlag = 0;

    transport_handle_rx();

    if(transportConnectionState != CONN_DATA) {
        fprintf(stderr, "  FAIL: transport_state_send_test\n");
        fprintf(stderr, "  Connection State failed to transition to CONN_DATA after connection SEND\n");
        assert(0);
    }

    if(transportConnectionType != HOST) {
        fprintf(stderr, "  FAIL: transport_state_send_test\n");
        fprintf(stderr, "  Connection Type expected to be HOST\n");
        assert(0);
    }

    if(transportTxSegment.control != ACK) {
        fprintf(stderr, "  FAIL: transport_state_connect_test\n");
        fprintf(stderr, "  Connection Reply failed to reply with ACK\n");
        assert(0);
    }

    fprintf(stderr, "  PASS: transport_state_send_test\n");
}

void transport_state_ack_test() {

    transportRxSegment.control = (TEST_NET_ID << 8) | ACK;
    transportRxSegment.source = TEST_SRC_ADDR;
    transportRxSegment.destination = TEST_DEST_ADDR;
    transportRxSegment.checksum = transport_crc(transportRxSegment);
    applicationTxPort = LIGHT_PORT;

    transportConnectionState = CONN_DATA;
    transportConnectionType = CLIENT;

    transportTxFlag = 0;

    transport_handle_rx();

    if(transportConnectionState != IDLE) {
        fprintf(stderr, "  FAIL: transport_state_ack_test\n");
        fprintf(stderr, "  Connection State failed to transition to CONN_CLOSE after connection ACK\n");
        assert(0);
    }

    if(transportConnectionType != NONE) {
        fprintf(stderr, "  FAIL: transport_state_ack_test\n");
        fprintf(stderr, "  Connection Type expected to be NONE\n");
        assert(0);
    }

    if(transportTxSegment.control != CLOSE) {
        fprintf(stderr, "  FAIL: transport_state_connect_test\n");
        fprintf(stderr, "  Connection Reply failed to reply with CLOSE\n");
        assert(0);
    }

    fprintf(stderr, "  PASS: transport_state_ack_test\n");
}

void transport_state_nack_test() {

    transportRxSegment.control = (TEST_NET_ID << 8) | NACK;
    transportRxSegment.source = TEST_SRC_ADDR;
    transportRxSegment.destination = TEST_DEST_ADDR;
    transportRxSegment.checksum = transport_crc(transportRxSegment);
    applicationTxPort = LIGHT_PORT;

    transportConnectionState = CONN_DATA;
    transportConnectionType = CLIENT;

    transportTxFlag = 0;

    transport_handle_rx();

    if(transportConnectionState != IDLE) {
        fprintf(stderr, "  FAIL: transport_state_nack_test\n");
        fprintf(stderr, "  Connection State failed to transition to CONN_NACK after connection NACK\n");
        assert(0);
    }

    if(transportConnectionType != NONE) {
        fprintf(stderr, "  FAIL: transport_state_nack_test\n");
        fprintf(stderr, "  Connection Type expected to be NONE\n");
        assert(0);
    }

    if(transportTxSegment.control != CLOSE) {
        fprintf(stderr, "  FAIL: transport_state_connect_test\n");
        fprintf(stderr, "  Connection Reply failed to reply with CLOSE\n");
        assert(0);
    }

    fprintf(stderr, "  PASS: transport_state_nack_test\n");
}

void transport_state_close_test() {

    transportRxSegment.control = (TEST_NET_ID << 8) | CLOSE;
    transportRxSegment.source = TEST_SRC_ADDR;
    transportRxSegment.destination = TEST_DEST_ADDR;
    transportRxSegment.checksum = transport_crc(transportRxSegment);
    applicationTxPort = LIGHT_PORT;

    transportConnectionState = CONN_DATA;
    transportConnectionType = HOST;

    transportTxFlag = 0;

    transport_handle_rx();

    if(transportConnectionState != IDLE) {
        fprintf(stderr, "  FAIL: transport_state_close_test\n");
        fprintf(stderr, "  Connection State failed to return to IDLE after connection CLOSE\n");
        assert(0);
    }

    if(transportConnectionType != NONE) {
        fprintf(stderr, "  FAIL: transport_state_close_test\n");
        fprintf(stderr, "  Connection Type failed to return to NONE after connection CLOSE\n");
        assert(0);
    }

    fprintf(stderr, "  PASS: transport_state_close_test\n");
}

void transport_state_connect_nack_test() {

    transportRxSegment.control = (TEST_NET_ID << 8) | NACK;
    transportRxSegment.source = TEST_SRC_ADDR;
    transportRxSegment.destination = TEST_DEST_ADDR;
    transportRxSegment.checksum = transport_crc(transportRxSegment);
    applicationTxPort = LIGHT_PORT;

    transportConnectionState = CONN_OPEN;
    transportConnectionType = CLIENT;

    transportTxFlag = 0;

    transport_handle_rx();

    if(transportConnectionState != CONN_FAIL) {
        fprintf(stderr, "  FAIL: transport_state_connect_nack_test\n");
        fprintf(stderr, "  Connection State failed to transition to CONN_NACK after connection NACK\n");
        assert(0);
    }

    if(transportConnectionType != NONE) {
        fprintf(stderr, "  FAIL: transport_state_connect_nack_test\n");
        fprintf(stderr, "  Connection Type failed to return to NONE after connection CLOSE\n");
        assert(0);
    }

    fprintf(stderr, "  PASS: transport_state_connect_nack_test\n");
}


void transport_timeout_test() {

    // test timer reset
    transport_timer_reset();
    if(transport_timer != TRANS_TIMEOUT) {
        fprintf(stderr, "  FAIL: transport_timeout_test\n");
        fprintf(stderr, "  Expected timer to be set to timeout value\n");
        assert(0);
    }


    // test pre-timeout
    for(int i=0; i<TRANS_TIMEOUT - 1; i++) {
        transport_timer_update();
    }
    
    if(transport_timeout()) {
        fprintf(stderr, "  FAIL: transport_timeout_test\n");
        fprintf(stderr, "  Timeout flagged too early\n");
        assert(0);
    }


    // test timeout
    transport_timer_update();
    if(transport_timeout() == 0) {
        fprintf(stderr, "  FAIL: transport_timeout_test\n");
        fprintf(stderr, "  Timer failed to timeout\n");
        assert(0);
    }

    fprintf(stderr, "  PASS: transport_timeout_test\n");
}


void transport_communication_test() {

    transport_init();
    application_init();

    // setup test data
    uint8_t testData[6] = {0, 1, 2, 3, 4, 5};
    uint8_t testDataLength = 6;
    memcpy(applicationTxData, testData, testDataLength);
    applicationTxLength = testDataLength;
    applicationTxFlag = 1;
    applicationRxFlag = 0;
    applicationTxPort = LIGHT_PORT;

    transportTxFlag = 0;

    ConnectionState clientState = IDLE;
    ConnectionType clientType = NONE;

    ConnectionState hostState = IDLE;
    ConnectionType hostType = NONE;

    // Client Request Connection
    fprintf(stderr, "    TEST: Client Connect\n");
    transportConnectionState = clientState;
    transportConnectionType = clientType;

    transport_handle_tx();

    if(transportTxSegment.control != CONNECT) {
        fprintf(stderr, "  FAIL: Client Connect\n");
        fprintf(stderr, "  Client did not send CONNECT\n");
        assert(0);
    }
    
    clientState = transportConnectionState;
    clientType = transportConnectionType;


    // Host Accepts Connection
    fprintf(stderr, "    TEST: Host Accept\n");
    transportTxFlag = 0;
    transportRxSegment = transportTxSegment;

    transportConnectionState = hostState;
    transportConnectionType = hostType;

    transport_handle_rx();

    if(transportTxSegment.control != ACCEPT) {
        fprintf(stderr, "  FAIL: Host Accept\n");
        fprintf(stderr, "  Host did not send ACCEPT\n");
        assert(0);
    }

    hostState = transportConnectionState;
    hostType = transportConnectionType;


    // Client Sends Data
    fprintf(stderr, "    TEST: Client Send\n");
    transportTxFlag = 0;
    transportRxSegment = transportTxSegment;

    transportConnectionState = clientState;
    transportConnectionType = clientType;

    transport_handle_rx();

    if(transportTxSegment.control != SEND) {
        fprintf(stderr, "  FAIL: Client Send\n");
        fprintf(stderr, "  Client did not send SEND\n");
        assert(0);
    }
    
    clientState = transportConnectionState;
    clientType = transportConnectionType;


    // Host Acknowledges Data
    fprintf(stderr, "    TEST: Host Acknowledge\n");
    transportTxFlag = 0;
    transportRxSegment = transportTxSegment;

    transportConnectionState = hostState;
    transportConnectionType = hostType;

    transport_handle_rx();

    if(transportTxSegment.control != ACK) {
        fprintf(stderr, "  FAIL: Host Acknowledge Send\n");
        fprintf(stderr, "  Host did not send ACK\n");
        assert(0);
    }

    hostState = transportConnectionState;
    hostType = transportConnectionType;

    // Client Closes Connection
    fprintf(stderr, "    TEST: Client Close\n");
    transportTxFlag = 0;
    transportRxSegment = transportTxSegment;

    transportConnectionState = clientState;
    transportConnectionType = clientType;

    transport_handle_rx();

    if(transportTxSegment.control != CLOSE) {
        fprintf(stderr, "  FAIL: Client Close\n");
        fprintf(stderr, "  Client did not send CLOSE\n");
        assert(0);
    }
    
    clientState = transportConnectionState;
    clientType = transportConnectionType;


    // Test Final Data
    if(applicationRxFlag == 0) {
        fprintf(stderr, "  FAIL: transport_communication_test\n");
        fprintf(stderr, "  Application RX Flag was not set\n");
        //assert(0);
    }

    if(applicationRxLength != testDataLength) {
        fprintf(stderr, "  FAIL: transport_communication_test\n");
        fprintf(stderr, "  Application RX Length does not match testDataLength\n");
        //assert(0);
    }

    int i;
    fprintf(stderr, "  Test Data: ");
    for(i=0; i<testDataLength; i++) {
        fprintf(stderr, "0x%04x ", testData[i]);
    }
    fprintf(stderr, "\n");

    fprintf(stderr, "  RX Data:   ");
    for(i=0; i<applicationRxLength; i++) {
        fprintf(stderr, "0x%04x ", applicationRxData[i]);
    }
    fprintf(stderr, "\n");

    fprintf(stderr, "  PASS: transport_communication_test\n");
}

void transport_timeout_connect_test() {
    

}

void transport_timeout_accept_test() {
    transportRxSegment.control = (TEST_NET_ID << 8) | CONNECT;
    transportRxSegment.source = TEST_SRC_ADDR;
    transportRxSegment.destination = TEST_DEST_ADDR;
    transportRxSegment.checksum = transport_crc(transportRxSegment);

    transportConnectionState = IDLE;
    transportConnectionType = NONE;

    transportTxFlag = 0;

    transport_handle_rx();

    int i, j;
    for(i=0; i<TRANS_RESEND+1; i++) {

        if(transportTxFlag == 0) {
            fprintf(stderr, "  FAIL: transport_timeout_accept_test\n");
            fprintf(stderr, "  Transport Flag was not set to resend segment after attempt %d\n", i);
            assert(0);
        }
        
        // timeout segment
        transportTxFlag = 0;
        for(j=0; j<TRANS_TIMEOUT; j++) {
            transport_timer_update();
        }

        transport_handle_timeout();
    }

    if(transportTxFlag != 0) {
        fprintf(stderr, "  FAIL: transport_timeout_accept_test\n");
        fprintf(stderr, "  Transport Flag was found set after %d failed resends\n", TRANS_RESEND);
        assert(0);
    }

    if(transportConnectionState != IDLE) {
        fprintf(stderr, "  FAIL: transport_timeout_accept_test\n");
        fprintf(stderr, "  Connection State was not set to IDLE after failed resends\n");
        assert(0);
    }

    if(transportConnectionType != NONE) {
        fprintf(stderr, "  FAIL: transport_timeout_accept_test\n");
        fprintf(stderr, "  Connection Type was not set to NONE after failed resends\n");
        assert(0);
    }

    fprintf(stderr, "  PASS: transport_timeout_accept_test\n");
}

void transport_timeout_send_test() {
    transportRxSegment.control = (TEST_NET_ID << 8) | ACCEPT;
    transportRxSegment.source = TEST_SRC_ADDR;
    transportRxSegment.destination = TEST_DEST_ADDR;
    transportRxSegment.checksum = transport_crc(transportRxSegment);

    transportConnectionState = CONN_OPEN;
    transportConnectionType = CLIENT;

    transportTxFlag = 0;

    transport_handle_rx();

    int i, j;
    for(i=0; i<TRANS_RESEND+1; i++) {

        if(transportTxFlag == 0) {
            fprintf(stderr, "  FAIL: transport_timeout_send_test\n");
            fprintf(stderr, "  Transport Flag was not set to resend segment after attempt %d\n", i);
            assert(0);
        }
        
        // timeout segment
        transportTxFlag = 0;
        for(j=0; j<TRANS_TIMEOUT; j++) {
            transport_timer_update();
        }

        transport_handle_timeout();
    }

    if(transportTxFlag != 0) {
        fprintf(stderr, "  FAIL: transport_timeout_send_test\n");
        fprintf(stderr, "  Transport Flag was found set after %d failed resends\n", TRANS_RESEND);
        assert(0);
    }

    if(transportConnectionState != IDLE) {
        fprintf(stderr, "  FAIL: transport_timeout_send_test\n");
        fprintf(stderr, "  Connection State was not set to IDLE after failed resends\n");
        assert(0);
    }

    if(transportConnectionType != NONE) {
        fprintf(stderr, "  FAIL: transport_timeout_send_test\n");
        fprintf(stderr, "  Connection Type was not set to NONE after failed resends\n");
        assert(0);
    }

    fprintf(stderr, "  PASS: transport_timeout_send_test\n");
}

void transport_timeout_ack_test() {
    transportRxSegment.control = (TEST_NET_ID << 8) | SEND;
    transportRxSegment.source = TEST_SRC_ADDR;
    transportRxSegment.destination = TEST_DEST_ADDR;
    transportRxSegment.checksum = transport_crc(transportRxSegment);

    transportConnectionState = CONN_OPEN;
    transportConnectionType = HOST;

    transportTxFlag = 0;

    transport_handle_rx();

    int i, j;
    for(i=0; i<TRANS_RESEND+1; i++) {

        if(transportTxFlag == 0) {
            fprintf(stderr, "  FAIL: transport_timeout_ack_test\n");
            fprintf(stderr, "  Transport Flag was not set to resend segment after attempt %d\n", i);
            assert(0);
        }
        
        // timeout segment
        transportTxFlag = 0;
        for(j=0; j<TRANS_TIMEOUT; j++) {
            transport_timer_update();
        }

        transport_handle_timeout();
    }

    if(transportTxFlag != 0) {
        fprintf(stderr, "  FAIL: transport_timeout_ack_test\n");
        fprintf(stderr, "  Transport Flag was found set after %d failed resends\n", TRANS_RESEND);
        assert(0);
    }

    if(transportConnectionState != IDLE) {
        fprintf(stderr, "  FAIL: transport_timeout_ack_test\n");
        fprintf(stderr, "  Connection State was not set to IDLE after failed resends\n");
        assert(0);
    }

    if(transportConnectionType != NONE) {
        fprintf(stderr, "  FAIL: transport_timeout_ack_test\n");
        fprintf(stderr, "  Connection Type was not set to NONE after failed resends\n");
        assert(0);
    }

    fprintf(stderr, "  PASS: transport_timeout_ack_test\n");
}

void transport_crc_test() {
    Segment testSegment;
    testSegment.control = (TEST_NET_ID << 8) | CONNECT;
    testSegment.source = 0x02;
    testSegment.destination = 0x01;
    testSegment.length = 0x01;
    testSegment.data = malloc(1);
    testSegment.data[0] = 0x00;

    uint16_t expChecksum = 33106;
    uint16_t testChecksum = transport_crc(testSegment);

    if(expChecksum != testChecksum) {
        fprintf(stderr, "  FAIL: transport_crc_test\n");
        fprintf(stderr, "  Expected Checksum 0x%04x but got 0x%04x\n", expChecksum, testChecksum);
        assert(0);
    }

    fprintf(stderr, "  PASS: transport_crc_test\n");
}