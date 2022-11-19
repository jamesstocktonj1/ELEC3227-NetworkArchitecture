#include "transport_test.h"


void main_transport_test() {

    fprintf(stderr, "main_transport_test\n");

    transport_timeout_test();
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