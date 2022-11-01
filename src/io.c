#include "../include/io.h"

#include <avr/io.h>


void init_pins() {

    LED_BUILTIN_DDR |= _BV(LED_BUILTIN);
}

void set_led() {

    LED_BUILTIN_PORT |= _BV(LED_BUILTIN);
}

void clear_led() {

    LED_BUILTIN_PORT &= ~_BV(LED_BUILTIN);
}