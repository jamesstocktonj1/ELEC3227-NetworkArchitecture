#include "../include/datalink_phy_interface.h"

#include <util/delay.h>
#include <string.h>

// Include the library for the RFM12 module and the UART
#include "../include/rfm12_config.h"
#include "../lib/librfm12/src/rfm12.h"
#include "../lib/librfm12/src/include/rfm12_hw.h" // For RFM12_STATUS_RSSI
#include "../lib/librfm12/src/include/rfm12_spi.c" // For rfm12_read

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

uint16_t rfm12b_last_interrupt = 0;

uint16_t is_carrier_present();

uint8_t dll_rf_can_tx() {
    return ctrl.txstate == STATUS_FREE;
}

int8_t dll_rf_tx(uint8_t *data, uint8_t length) {
    if (ctrl.txstate != STATUS_FREE) return -1; // TX buffer not free

    if (rfm12_tx(length, 0, data) != RFM12_TX_ENQUEUED) {
        return -2; // Failed to enqueue data
    }

    return 0;
}

uint8_t dll_rf_rx(uint8_t *data) {
    //don't disturb RFM12 if transmitting or receiving
	if (ctrl.rfm12_state != STATE_RX_IDLE) return 0;
    if (rfm12_rx_status() != STATUS_COMPLETE) return 0;
    uint8_t length = rfm12_rx_len();
	memcpy(data, rfm12_rx_buffer(), length);
	rfm12_rx_clear();
    return length;
}

void dll_rf_init() {
    rfm12_init();
}

void dll_rf_tick() {
    // 0.05-p CSMA

    // Check module is still responding
    if (rfm12b_last_interrupt > RFM12B_INTERRUPT_TIMEOUT_MS) {
        printf("Resetting RFM12B...\n");
        ctrl.rfm12_state = STATE_RX_IDLE;
        rfm12_reset();
        rfm12b_last_interrupt = 0;
    }

    // Do random calculation here instead of later, so the PRNG gets a bit more random
    uint8_t actually_transmit = (prng() % 20) == 0;
    
    //don't disturb RFM12 if transmitting or receiving
	if (ctrl.rfm12_state != STATE_RX_IDLE) return;

    //do we have something to transmit?
    if (ctrl.txstate != STATUS_OCCUPIED) return;

    // Check 1-8 times for a carrier (adds a variable bit of delay)
    uint8_t n = 1 + (prng() % 8);
    for (; n > 0; n--) {
        if (is_carrier_present()) return;
    }

    if (actually_transmit) {
        rfm12_start_tx();
        _delay_ms(50);
    }
}

uint16_t is_carrier_present() {
	RFM12_INT_OFF();
	uint16_t status = rfm12_read(RFM12_CMD_STATUS);
	RFM12_INT_ON();

	return status & RFM12_STATUS_RSSI;
}

void dll_timer() {
    if (rf_interrupt_occurred || ctrl.rfm12_state == STATE_RX_IDLE) {
        rf_interrupt_occurred = 0;
        rfm12b_last_interrupt = 0;
    } else if (rfm12b_last_interrupt < 0xffff) {
        rfm12b_last_interrupt++;
    }
}