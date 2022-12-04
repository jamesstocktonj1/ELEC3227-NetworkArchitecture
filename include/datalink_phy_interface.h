#ifndef DATALINK_PHY_INTERFACE_H
#define DATALINK_PHY_INTERFACE_H

#include "datalink.h"

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
 * This function is responsible with transmitting the DLL frames with 0.05p-CSMA on the RFM12B module
*/
void dll_rf_tick();

#endif // DATALINK_PHY_INTERFACE_H