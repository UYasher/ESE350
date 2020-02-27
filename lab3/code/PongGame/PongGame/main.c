#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <util/delay.h>
#include "usart.h"
#include "lcd.h"
#include "touchscreen.c"

#define FREQ 16000000
//#define BAUD 9600
#define HIGH 1
#define LOW 0
//#define BUFFER 1024
#define BLACK 0x000001

char displayChar = 0;

void USART_init(void){
	
	/*Set baud rate */
	UBRR0H = (unsigned char)(BAUD_PRESCALLER>>8);
	UBRR0L = (unsigned char)BAUD_PRESCALLER;
	//Enable receiver and transmitter
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	/* Set frame format: 8data, 2stop bit */
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

void USART_send( unsigned char data)
{
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = data;
}

void USART_putstring(char* StringPtr){
	while(*StringPtr != 0x00){
		USART_send(*StringPtr);
	StringPtr++;}
}


int main(void)
{
	USART_init(); //initializes the stuff for printing
	USART_putstring("Started \n");
	
	//setting up the gpio for backlight
	DDRD |= 0x80;
	PORTD &= ~0x80;
	PORTD |= 0x00;
	
	DDRB |= 0x05;
	PORTB &= ~0x05;
	PORTB |= 0x00;
	
	//lcd initialisation
	lcd_init();
	lcd_command(CMD_DISPLAY_ON);
	lcd_set_brightness(0x18);
	write_buffer(buff);
	_delay_ms(2000);
	clear_buffer(buff);
	
	// Touchscreen initialization
	int x = 0;
	int y = 0;
	
	while (1)
	{
		x = get_x();
		y = get_y();
		
		char out_char[10];
		sprintf(out_char, "%d \n", x);
		USART_putstring(out_char);
		
		//USART_putstring("Got past first print\n");
		
		clear_buffer(buff);	
		write_buffer(buff);
		//USART_putstring("Got past buffer stuff\n");
	}
}

