#include <avr/io.h>
#include <util/delay.h>

#include "include/io.h"
#include "include/serial.h"


int main() {
    init_pins();
    init_serial();


    while(1) {
        printf("Hello World!\n");
        
        set_led();
        _delay_ms(400);

        clear_led();
        _delay_ms(400);
    }
}