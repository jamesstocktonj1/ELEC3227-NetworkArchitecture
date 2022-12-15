#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>
#include <avr/interrupt.h>

#include "../include/network.h"
#include "../include/transport.h"
#include "../include/datalink_phy_interface.h"

#define msTIMER_VALUE (256-((F_CPU/64)/1000))

/**
 * This function initialises all the registers for a 1ms timer.
 */
void init_timer(void);

#endif