#ifndef DATALINK_PHY_INTERFACE_H
#define DATALINK_PHY_INTERFACE_H

#include "datalink.h"

// How long we wait before resetting the RFM12B module
#define RFM12B_INTERRUPT_TIMEOUT_MS 100

/**
 * This function checks if the RFM12B module is ready for more data to transmit
 * @returns 0 - Not ready, 1 - Ready
*/
uint8_t dll_rf_can_tx();

/**
 * This function queues data to transmit on the RFM12B module
 * @param data A pointer to the data
 * @param length The length of the data
 * @returns 0 - Success, otherwise error code
*/
int8_t dll_rf_tx(uint8_t *data, uint8_t length);

/**
 * This function checks and receives data when available from the RFM12B module
 * @param data A pointer to the receive buffer to write to
 * @returns 0 - No data, otherwise the length of the received data
*/
uint8_t dll_rf_rx(uint8_t *data);

/**
 * This function initialises the RFM12B module
*/
void dll_rf_init();

/**
 * This function is responsible with transmitting the DLL frames with 0.05p-CSMA on the RFM12B module and also resetting the module if it stops responding
*/
void dll_rf_tick();

/**
 * This function is called every millisecond to update the timer of how long we have not receieved an interrupt from the RFM12B module
*/
void dll_timer();

#endif // DATALINK_PHY_INTERFACE_H