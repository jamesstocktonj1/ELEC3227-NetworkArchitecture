#include "../include/io.h"

#include <avr/io.h>


void init_pins() {

    LED_BUILTIN_DDR |= _BV(LED_BUILTIN);
}

void set_pins(uint8_t lightID, uint8_t lightIntensity) {
    
}