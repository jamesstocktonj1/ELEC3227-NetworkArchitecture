#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

#include "include/io.h"
#include "include/serial.h"
#include "include/timer.h"
#include "include/eeprom.h"

#include "include/util.h"
#include "include/application.h"
#include "include/datalink.h"
#include "include/network.h"
#include "include/datalink_phy_interface.h"

#undef APP_ADDR
#define APP_ADDR 0x0

void poll_network_stack(void);

uint8_t rx_packet[DLL_MAX_PACKET_SIZE] = {0};
uint8_t tx_frame[DLL_MAX_FRAME_SIZE] = {0};
uint8_t rx_frame[DLL_MAX_FRAME_SIZE] = {0};

uint8_t dllRxFlag;
uint8_t dllRxLength;

uint8_t app_addr = APP_ADDR;
uint8_t dest_addr = 0x1;

uint8_t data_1[5] = {0x00, 0x01, 0x02, 0x03, 0x04};
uint8_t data_2[5] = {0x10, 0x11, 0x12, 0x13, 0x14};
uint8_t data_3[5] = {0x20, 0x21, 0x22, 0x23, 0x24};
uint8_t data_4[5] = {0x30, 0x31, 0x32, 0x33, 0x34};


static qrecord buffer;

int main() {
    init_pins();
    init_serial();
    init_timer();

    // TODO: Seed PRNG

    if (app_addr == 0) {
        app_addr = eeprom_get_node_id();
    } else {
        eeprom_set_node_id(app_addr);
    }

    switch(app_addr) {
        case 0x1:
            dest_addr = 0x2;
            break;
        case 0x2:
            dest_addr = 0x1;
            break;
        case 0x3:
            dest_addr = 0x4;
            break;
        case 0x4:
            dest_addr = 0x3;
            break;
    }

    printf("\n\n\n");
    printf("Starting Node 0x%x\n", app_addr);
    _delay_ms(1000);

    printf("Initialising...\n");
    application_init();
    transport_init(app_addr);
    net_init(app_addr);
    dll_init();
    dll_rf_init();
    sei(); // enable interrupts
    printf("Done\n");

    uint16_t t = 5000;

    uint8_t i;
    uint8_t rxLength;
    uint8_t rxBuffer[BUFF_SIZE];



    


    while(1) {


        if(t) {
            t--;
        }
        else {
            uint8_t *data_arr[4] = {data_1, data_2, data_3, data_4};
            uint8_t *data = data_arr[app_addr-1 < 4 ? app_addr-1 : 0];
            uint8_t data_length = 5;
            uint8_t sent = application_handle_tx(data, data_length, LIGHT_PORT, dest_addr);

            printf("\n\n\n\n\n\n\n\n\n");
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


            t = 5000;
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

        _delay_ms(1);
    }
}


void poll_network_stack() {

    // handle transport layer
    if(transport_poll_tx()) {
        printf("Transport TX Data to 0x%x\n", transportTxAddress);
        transport_handle_tx();

        // transportTxAddress = 0x00;
    }

    if(transport_poll_rx()) {
        printf("Transport RX Data\n");
        transport_handle_rx();
    }

    transport_handle_timeout();

    // handle network layer
    if(dllRxFlag)
    {
        dllRxFlag = 0;
        printf("Network RX Data\n");
        net_handle_rx_packet(rx_packet, dllRxLength);
    }


    net_transport_poll();
    net_handle_timeout_rt();


    // handle data link layer
    int8_t error = 0;

    while (dll_has_rx_packet()) {
        // printf("DLL RX Packet\n");
        uint8_t length = dll_get_rx_packet(rx_packet);
        dllRxFlag = 1;
        dllRxLength = length;
        // SEND TO NET LAYER

        printf("DLL RX Packet: ");
        uint8_t i;
        for (i = 0; i < length; i++) printf("%02x ", rx_packet[i]);
        printf("\n");
    }

     if (net_tx_poll()) {
        
        buffer = net_handle_tx();
        if(buffer.packet_size)
        {   
            printf("Network TX\n");
            if (!dll_has_tx_frame()) {
                // printf("DLL TX Queue Packet\n");
                printf("DLL TX Packet To %d: ", buffer.next_hop);
                uint8_t i;
                for (i = 0; i < buffer.packet_size; i++) printf("%02x ", buffer.packet[i]);
                printf("\n");
                uint8_t queued = dll_queue_tx_net_packet(buffer.packet, buffer.packet_size, buffer.next_hop);
                if (!queued) printf("Failed to queue TX packet\n");
            }
        }

     }

    if (dll_rf_can_tx() && dll_has_tx_frame()) {
        // printf("DLL TX Frame\n");
        uint8_t length = dll_get_tx_frame(tx_frame, app_addr);
        error = dll_rf_tx(tx_frame, length);
        if (error) printf("RF TX Error: %i\n", error);
    }

    uint8_t length = dll_rf_rx(rx_frame);
    if (length) {
        // printf("DLL RX Frame\n");
        error = dll_receive_dll_frame(rx_frame, length, app_addr);
        if (error) printf("DLL RX Error: %i\n", error);
    }

    dll_rf_tick();
}