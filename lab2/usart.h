#ifndef USART
#define USART

#define F_CPU 16000000UL
#define BAUDRATE 9600
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 16UL))) - 1)
#define BUFFER 100

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>

void USART_init(void);
void USART_send(unsigned char);
void USART_putstring(char*);

#endif