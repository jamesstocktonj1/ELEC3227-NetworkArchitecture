#ifndef IO_H
#define IO_H

#define LED_BUILTIN 7
#define LED_BUILTIN_DDR DDRB
#define LED_BUILTIN_PORT PORTB


void init_pins(void);
void set_led(void);
void clear_led(void);

#endif