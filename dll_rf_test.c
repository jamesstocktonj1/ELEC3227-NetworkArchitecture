#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "include/io.h"
#include "include/serial.h"

#include "include/util.h"
#include "include/timer.h"
#include "include/datalink.h"
#include "include/datalink_phy_interface.h"

char msg[] = "This message should be longer than the MTU!";

#define MAC_ADDR 0x1

// Add extra byte so we can append null byte before printing
uint8_t rx_packet[DLL_MAX_PACKET_SIZE+1] = {0};
uint8_t tx_frame[DLL_MAX_FRAME_SIZE] = {0};
uint8_t rx_frame[DLL_MAX_FRAME_SIZE] = {0};

int main() {
    init_pins();
    init_serial();

    // TODO: Seed PRNG

    printf("Initialising...");
    init_timer();
    dll_init();
    dll_rf_init();
    sei(); // enable interrupts
    printf("Done\n");

    uint16_t t = 0xffff;

    int8_t error = 0;

    while(1) {
        while (dll_has_rx_packet()) {
            uint8_t length = dll_get_rx_packet(rx_packet);
            rx_packet[length] = '\x00';
            printf("%s\n", rx_packet);
        }

        if (t >= 500) {
            t = 0;
            if (!dll_has_tx_frame()) {
                uint8_t queued = dll_queue_tx_net_packet((uint8_t*)msg, sizeof(msg), DLL_BROADCAST_ADDR);
                if (!queued) printf("Failed to queue TX packet\n");
            }
        }

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
        _delay_ms(1);
        t+=1;
    }
}