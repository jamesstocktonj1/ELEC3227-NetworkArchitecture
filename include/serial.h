#ifndef SERIAL_H
#define SERIAL_H

#include <stdio.h>
#include <avr/io.h>


#ifndef BAUD_RATE
#define BAUD_RATE 9600
#endif

#ifndef BAUD_RATE1
#define BAUD_RATE1 9600
#endif

extern FILE serialOut0;
extern FILE serialIn0;

void init_serial(void);

int uputchar0(char c, FILE *stream);
int ugetchar0(FILE *stream);

#ifdef UDR1
extern FILE serialOut1;
extern FILE serialIn1;

void init_serial1(void);

int uputchar1(char c, FILE *stream);
int ugetchar1(FILE *stream);
#endif

#endif