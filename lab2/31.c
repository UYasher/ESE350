#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include "usart.h"

char light_string[10];

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

int main(void){
	USART_init(); // Initializes the stuff for printing
	
	USART_putstring("Started \n");
	
	// Configure PORTC0 to an input
	DDRC &= ~( 1 << 0);
	
	ADCSRA|= ((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)); // Set ADC prescaler to 128
	
	// Set ADC voltage reference to AVCC
	ADMUX |= ( 1 << REFS0 );
	ADMUX &= ~( 1 << REFS1 );
	
	ADCSRB&= ~((1<<ADTS2)|(1<<ADTS1)|(1<<ADTS0));//ADC in free-running mode
	
	ADCSRA |= ( 1 << ADEN ); // Enable ADC
	ADCSRA |= ( 1 << ADATE ); // Set ADC Auto Trigger Enable
	ADCSRA |= ( 1 << ADSC ); // Start First ADC Conversion
	
	while(1){
		sprintf(light_string, "%u", ADC);
		USART_putstring(light_string);
		USART_putstring("\n");
	}
}
