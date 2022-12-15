#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

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

uint8_t dllRxFlag;
uint8_t dllRxLength;

int main() {
    init_pins();
    init_serial();
    init_timer();

    // TODO: Seed PRNG

    printf("Initialising... ");
    application_init();
    transport_init();
    net_init();
    dll_init();
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
            #ifdef NODE1
            uint8_t data[5] = {0x00, 0x01, 0x02, 0x03, 0x04};
            #else
            uint8_t data[5] = {0x10, 0x10, 0x20, 0x30, 0x40};
            #endif
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
        printf("Transport TX Data\n");
        transport_handle_tx();
    }

    if(transport_poll_rx()) {
        printf("Transport RX Data\n");
        transport_handle_rx();
    }

    transport_handle_timeout();

    // handle network layer
    if(dllRxFlag)
    {
        printf("Network RX Data\n");
        net_handle_rx_packet(rx_packet, dllRxLength);
    }

    if(transportTxFlag)
    {
        printf("Network TX Data\n");
        uint8_t packet[7+transportTxSegment.length];

            packet[CONTROL_1_BYTE] = transportTxSegment.control>>8;
            packet[CONTROL_2_BYTE] = transportTxSegment.control;
            packet[SRC_ADDRESS_BYTE] = transportTxSegment.source;
            packet[DEST_ADDRESS_BYTE] = transportTxSegment.destination;
            packet[LENGTH_BYTE] = transportTxSegment.length;
            memcpy(&packet[TRAN_SEGMENT_BYTE], &(transportRxSegment.data), transportTxSegment.length );
            packet[TRAN_SEGMENT_BYTE + transportTxSegment.length] = transportTxSegment.checksum>>8;
            packet[TRAN_SEGMENT_BYTE + transportTxSegment.length + 1] = transportTxSegment.checksum;
            send_data(transportTxAddress, transportTxSegment.data, transportTxSegment.length );

            uint8_t dllQueued = dll_queue_tx_net_packet(packet, 7+transportTxSegment.length, transportTxAddress);
            if(!dllQueued) {
                printf("Network Failed to add to DLL queue\n");
            }

            transportTxFlag = 0;
            
    }





    // handle data link layer
    int8_t error = 0;

    while (dll_has_rx_packet()) {
        printf("DLL RX Data\n");
        uint8_t length = dll_get_rx_packet(rx_packet);
        dllRxFlag = 1;
        length++;
        dllRxLength = length;
        // SEND TO NET LAYER
    }

    // if (net_has_tx_packet) {
    //     if (!dll_has_tx_frame()) {
    //         uint8_t queued = dll_queue_tx_net_packet((uint8_t*)msg, sizeof(msg), DLL_BROADCAST_ADDR);
    //         if (!queued) printf("Failed to queue TX packet\n");
    //     }
    // }

    if (dll_rf_can_tx() && dll_has_tx_frame()) {
        printf("DLL TX Data Here\n");
        uint8_t length = dll_get_tx_frame(tx_frame, MAC_ADDR);
        error = dll_rf_tx(tx_frame, length);
        if (error) printf("RF TX Error: %i\n", error);
    }

    uint8_t length = dll_rf_rx(rx_frame);
    if (length) {
        printf("DLL TX Data\n");
        error = dll_receive_dll_frame(rx_frame, length, MAC_ADDR);
        if (error) printf("DLL RX Error: %i\n", error);
    }

    dll_rf_tick();
}