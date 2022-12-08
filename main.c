#include <avr/io.h>
#include <util/delay.h>

#include "include/rfm12.h"

#include "include/io.h"
#include "include/serial.h"
#include "include/timer.h"

#include "include/util.h"
#include "include/application.h"
#include "include/datalink.h"
#include "include/network.h"


#define NODE1

#undef APP_ADDR
#ifdef NODE1
#define DEST_ADDR 0x01
#define APP_ADDR 0x02
#else
#define DEST_ADDR 0x02
#define APP_ADDR 0x01
#endif


void poll_network_stack(void);


int main() {
    init_pins();
    init_serial();
    init_timer();

    rfm12_init();

    application_init();
    transport_init();
    // network_init();
    dll_init();

    sei();

    uint8_t t = 500;

    uint8_t i;
    uint8_t rxLength;
    uint8_t rxBuffer[BUFF_SIZE];


    while(1) {


        if(t) {
            t--;
        }
        else {
            uint8_t data[5] = {0x00, 0x01, 0x02, 0x03, 0x04};
            uint8_t sent = application_handle_tx(data, 5, LIGHT_PORT, DEST_ADDR);

            printf("Data TX: ");

            if(sent) {
                for(i=0; i<rxLength; i++) {
                    printf("0x%04x, ");
                }
                printf("\n");
            }
            else {
                printf("Failed\n");
            }


            t = 500;
        }
        
        rxLength = application_handle_rx(rxBuffer, NULL);
        if(rxLength) {
            printf("Data RX: ");
            
            uint8_t i;
            for(i=0; i<rxLength; i++) {
                printf("0x%04x, ");
            }
            printf("\n");
        }

        poll_network_stack();
    }
}


void poll_network_stack() {

    // handle transport layer
    if(transport_poll_tx()) {
        transport_handle_tx();
    }

    if(transport_poll_rx()) {
        transport_handle_rx();
    }

    // handle network layer


    // handle data link layer
}