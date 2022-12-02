#include "../include/datalink_phy_interface.h"

#include "string.h"

// Include the library for the RFM12 module and the UART
#include "../include/rfm12_config.h"
#include "../lib/librfm12/src/rfm12.h"
#include "../lib/librfm12/src/include/rfm12_hw.h" // For RFM12_STATUS_RSSI
#include "../lib/librfm12/src/include/rfm12_spi.c" // For rfm12_read

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

uint16_t is_carrier_present();

int8_t dll_rf_tx(uint8_t length, uint8_t *data) {
    if (ctrl.txstate != STATUS_FREE) return -1; // TX buffer not free

    if (rfm12_tx(length, 0, data) != RFM12_TX_ENQUEUED) {
        return -2; // Failed to enqueue data
    }

    return 0;
}

uint8_t dll_rf_rx(uint8_t *data) {
    if (rfm12_rx_status() != STATUS_COMPLETE) return 0;
    uint8_t length = rfm12_rx_len();
	memcpy(data, rfm12_rx_buffer(), length);
	rfm12_rx_clear();
    return length;
}

void dll_rf_tick() {
    //don't disturb RFM12 if transmitting or receiving
	if (ctrl.rfm12_state != STATE_RX_IDLE) return;

    //do we have something to transmit?
    if (ctrl.txstate != STATUS_OCCUPIED) return;

	if (is_carrier_present()) return;

	rfm12_start_tx();
}

uint16_t is_carrier_present() {
	RFM12_INT_OFF();
	uint16_t status = rfm12_read(RFM12_CMD_STATUS);
	RFM12_INT_ON();

	return status & RFM12_STATUS_RSSI;
}