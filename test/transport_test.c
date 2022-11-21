#include "transport_test.h"


void main_transport_test() {

    fprintf(stderr, "main_transport_test\n");

    transport_state_machine_test();
    transport_timeout_test();
}


void transport_state_machine_test() {

    fprintf(stderr, "  RUN:  transport_state_machine_test\n");

    transport_state_connect_test();
    transport_state_accept_test();
    transport_state_send_test();
    transport_state_ack_test();
    transport_state_nack_test();
    transport_state_close_test();

    fprintf(stderr, "  PASS: transport_state_machine_test\n");
}

void transport_state_connect_test() {

    Segment testSegment;
    testSegment.control = (TEST_NET_ID << 8) | CONNECT;
    testSegment.source = SRC_ADDR;
    testSegment.destination = TEST_DEST_ADDR;

    ConnectionState testState = IDLE;
    ConnectionType testConnType = NONE;

    transport_handle_segment(testSegment, &testState, &testConnType);

    if(testState != CONN_OPEN) {
        fprintf(stderr, "  FAIL: transport_state_connect_test\n");
        fprintf(stderr, "  Connection State failed to transition to CONN_OPEN after connection CONNECT\n");
        assert(0);
    }

    if(testConnType != HOST) {
        fprintf(stderr, "  FAIL: transport_state_connect_test\n");
        fprintf(stderr, "  Connection Type failed to transition to HOST after connection CONNECT\n");
        assert(0);
    }

    fprintf(stderr, "  PASS: transport_state_connect_test\n");
}

void transport_state_accept_test() {

    Segment testSegment;
    testSegment.control = (TEST_NET_ID << 8) | ACCEPT;
    testSegment.source = SRC_ADDR;
    testSegment.destination = TEST_DEST_ADDR;

    ConnectionState testState = CONN_OPEN;
    ConnectionType testConnType = CLIENT;

    transport_handle_segment(testSegment, &testState, &testConnType);


    if(testState != CONN_DATA) {
        fprintf(stderr, "  FAIL: transport_state_accept_test\n");
        fprintf(stderr, "  Connection State failed to transition to CONN_DATA after connection ACCEPT\n");
        assert(0);
    }

    if(testConnType != CLIENT) {
        fprintf(stderr, "  FAIL: transport_state_accept_test\n");
        fprintf(stderr, "  Connection Type failed to transition to CLIENT after connection ACCEPT\n");
        assert(0);
    }

    fprintf(stderr, "  PASS: transport_state_accept_test\n");
}

void transport_state_send_test() {

    Segment testSegment;
    testSegment.control = (TEST_NET_ID << 8) | SEND;
    testSegment.source = SRC_ADDR;
    testSegment.destination = TEST_DEST_ADDR;

    ConnectionState testState = CONN_OPEN;
    ConnectionType testConnType = HOST;

    transport_handle_segment(testSegment, &testState, &testConnType);

    if(testState != CONN_DATA) {
        fprintf(stderr, "  FAIL: transport_state_send_test\n");
        fprintf(stderr, "  Connection State failed to transition to CONN_DATA after connection SEND\n");
        assert(0);
    }

    if(testConnType != HOST) {
        fprintf(stderr, "  FAIL: transport_state_send_test\n");
        fprintf(stderr, "  Connection Type expected to be HOST\n");
        assert(0);
    }

    fprintf(stderr, "  PASS: transport_state_send_test\n");
}

void transport_state_ack_test() {

    Segment testSegment;
    testSegment.control = (TEST_NET_ID << 8) | ACK;
    testSegment.source = SRC_ADDR;
    testSegment.destination = TEST_DEST_ADDR;

    ConnectionState testState = CONN_DATA;
    ConnectionType testConnType = CLIENT;

    transport_handle_segment(testSegment, &testState, &testConnType);

    if(testState != IDLE) {
        fprintf(stderr, "  FAIL: transport_state_ack_test\n");
        fprintf(stderr, "  Connection State failed to transition to CONN_CLOSE after connection ACK\n");
        assert(0);
    }

    if(testConnType != NONE) {
        fprintf(stderr, "  FAIL: transport_state_ack_test\n");
        fprintf(stderr, "  Connection Type expected to be NONE\n");
        assert(0);
    }

    fprintf(stderr, "  PASS: transport_state_ack_test\n");
}

void transport_state_nack_test() {

    Segment testSegment;
    testSegment.control = (TEST_NET_ID << 8) | NACK;
    testSegment.source = SRC_ADDR;
    testSegment.destination = TEST_DEST_ADDR;

    ConnectionState testState = CONN_DATA;
    ConnectionType testConnType = CLIENT;

    transport_handle_segment(testSegment, &testState, &testConnType);

    if(testState != IDLE) {
        fprintf(stderr, "  FAIL: transport_state_nack_test\n");
        fprintf(stderr, "  Connection State failed to transition to CONN_NACK after connection NACK\n");
        assert(0);
    }

    if(testConnType != NONE) {
        fprintf(stderr, "  FAIL: transport_state_nack_test\n");
        fprintf(stderr, "  Connection Type expected to be NONE\n");
        assert(0);
    }

    fprintf(stderr, "  PASS: transport_state_nack_test\n");
}

void transport_state_close_test() {

    Segment testSegment;
    testSegment.control = (TEST_NET_ID << 8) | CLOSE;
    testSegment.source = SRC_ADDR;
    testSegment.destination = TEST_DEST_ADDR;

    ConnectionState testState = CONN_DATA;
    ConnectionType testConnType = HOST;

    transport_handle_segment(testSegment, &testState, &testConnType);

    if(testState != IDLE) {
        fprintf(stderr, "  FAIL: transport_state_close_test\n");
        fprintf(stderr, "  Connection State failed to return to IDLE after connection CLOSE\n");
        assert(0);
    }

    if(testConnType != NONE) {
        fprintf(stderr, "  FAIL: transport_state_close_test\n");
        fprintf(stderr, "  Connection Type failed to return to NONE after connection CLOSE\n");
        assert(0);
    }

    fprintf(stderr, "  PASS: transport_state_close_test\n");
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