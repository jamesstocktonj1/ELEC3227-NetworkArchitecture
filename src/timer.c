#include "../include/timer.h"

ISR(TIMER0_OVF_vect) {
    TCNT0 = msTIMER_VALUE;

    //TODO: Add timer functions in here
}

void init_timer() {

    /**
     * 12MHz (FCPU) / 64 (CS0 = 3) -> 125,000 incr/sec
     * for overflow every 1ms, reload (255-187) = 68
     */

    // normal port operation
    TCCR0A = 0;

    // set clock divide to 64
    TCCR0B |= _BV(CS01) | _BV(CS00);

    // set timer to ~1ms
    TCNT0 = msTIMER_VALUE;

    // enable interrupt register
    TIMSK0 |= _BV(TOIE0);
}