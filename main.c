#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "include/io.h"
#include "include/serial.h"
#include "include/timer.h"

#include "include/util.h"
#include "include/application.h"
#include "include/datalink.h"
#include "include/network.h"
#include "include/datalink_phy_interface.h"


#define NODE1

#undef APP_ADDR
#ifdef NODE1
#define DEST_ADDR 0x01
#define APP_ADDR 0x02
#else
#define DEST_ADDR 0x02
#define APP_ADDR 0x01
#endif

#define MAC_ADDR APP_ADDR


void poll_network_stack(void);

uint8_t rx_packet[DLL_MAX_PACKET_SIZE] = {0};
uint8_t tx_frame[DLL_MAX_FRAME_SIZE] = {0};
uint8_t rx_frame[DLL_MAX_FRAME_SIZE] = {0};

int main() {
    init_pins();
    init_serial();
    init_timer();

    // TODO: Seed PRNG

    printf("Initialising...");
    application_init();
    transport_init();
    // network_init();
    dll_init();
    sei(); // enable interrupts
    printf("Done\n");

    uint16_t t = 500;

    uint8_t i;
    uint8_t rxLength;
    uint8_t rxBuffer[BUFF_SIZE];


    while(1) {


        if(t) {
            t--;
        }
        else {
            uint8_t data[5] = {0x00, 0x01, 0x02, 0x03, 0x04};
            uint8_t data_length = 5;
            uint8_t sent = application_handle_tx(data, data_length, LIGHT_PORT, DEST_ADDR);

            printf("Data TX:");

            if(sent) {
                for(i=0; i<data_length; i++) {
                    printf(" %02x", data[i]);
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
            printf("Data RX:");
            
            uint8_t i;
            for(i=0; i<rxLength; i++) {
                printf(" %02x", rxBuffer[i]);
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
    int8_t error = 0;

    while (dll_has_rx_packet()) {
        uint8_t length = dll_get_rx_packet(rx_packet);
        length++;
        // SEND TO NET LAYER
    }

    // if (net_has_tx_packet) {
    //     if (!dll_has_tx_frame()) {
    //         uint8_t queued = dll_queue_tx_net_packet((uint8_t*)msg, sizeof(msg), DLL_BROADCAST_ADDR);
    //         if (!queued) printf("Failed to queue TX packet\n");
    //     }
    // }

    if (dll_rf_can_tx() && dll_has_tx_frame()) {
        uint8_t length = dll_get_tx_frame(tx_frame, MAC_ADDR);
        error = dll_rf_tx(tx_frame, length);
        if (error) printf("RF TX Error: %i\n", error);
    }

    uint8_t length = dll_rf_rx(rx_frame);
    if (length) {
        error = dll_receive_dll_frame(rx_frame, length, MAC_ADDR);
        if (error) printf("DLL RX Error: %i\n", error);
    }

    dll_rf_tick();
}