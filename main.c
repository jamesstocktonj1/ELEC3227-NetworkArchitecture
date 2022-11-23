#include <avr/io.h>
#include <util/delay.h>

#include "include/rfm12.h"

#include "include/io.h"
#include "include/serial.h"

#include "include/util.h"
#include "include/application.h"
#include "include/datalink.h"
#include "include/network.h"


int main() {
    init_pins();
    init_serial();

    rfm12_init();


    while(1) {
        printf("Hello World!\n");
        
        set_led();
        _delay_ms(400);

        clear_led();
        _delay_ms(400);
    }
}