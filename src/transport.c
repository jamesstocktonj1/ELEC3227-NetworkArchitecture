#include "../include/transport.h"


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
