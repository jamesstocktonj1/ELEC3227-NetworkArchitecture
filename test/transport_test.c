#include "transport_test.h"


void main_transport_test() {

    fprintf(stderr, "main_transport_test\n");

    transport_state_machine_test();
    transport_timeout_test();
}


void transport_state_machine_test() {

    fprintf(stderr, "  RUN:  transport_state_machine_test\n");

    transport_init();

    transport_state_connect_test();
    transport_state_accept_test();
    transport_state_send_test();
    transport_state_ack_test();
    transport_state_nack_test();
    transport_state_close_test();

    transport_state_connect_nack_test();

    fprintf(stderr, "  PASS: transport_state_machine_test\n");
}

void transport_state_connect_test() {

    transportRxSegment.control = (TEST_NET_ID << 8) | CONNECT;
    transportRxSegment.source = TEST_SRC_ADDR;
    transportRxSegment.destination = TEST_DEST_ADDR;

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