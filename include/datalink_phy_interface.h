#ifndef DATALINK_PHY_INTERFACE_H
#define DATALINK_PHY_INTERFACE_H

#include "datalink.h"

/**
 * This function is responsible with transmitting the DLL frames with 0.05p-CSMA on the RFM12B module
*/
void dll_rf_tick();

#endif // DATALINK_PHY_INTERFACE_H