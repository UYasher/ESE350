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

void init_ADC(void){
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
}

// Set PORTB2-4 to output
void init_DAC(void){
	DDRB |= (1 << 2) + (1 << 3) + (1 << 4);
}

// Split signal from light sensitive resistor into bins
uint8_t ADC_to_8_bit(int ADC_val){
	if (ADC < 580 + 45*1){
		return 0;
	}
	if (ADC < 580 + 45*2){
		return 1;
	}
	if (ADC < 580 + 45*3){
		return 2;
	}
	if (ADC < 580 + 45*4){
		return 3;
	}
	if (ADC < 580 + 45*5){
		return 4;
	}
	if (ADC < 580 + 45*6){
		return 5;
	}
	if (ADC < 580 + 45*7){
		return 6;
	}
	else{
		return 7;
	}
}

// Output to PORT2-4
void DAC_out(uint8_t ADC_group){
	if (ADC_group == 0)
	{
		PORTB &= ~(1 << PORTB2);
		PORTB &= ~(1 << PORTB3);
		PORTB &= ~(1 << PORTB4);
	}
	else if (ADC_group == 1)
	{
		PORTB |= (1 << PORTB2);
		PORTB &= ~(1 << PORTB3);
		PORTB &= ~(1 << PORTB4);
	}
	else if (ADC_group == 2)
	{
		PORTB &= ~(1 << PORTB2);
		PORTB |= (1 << PORTB3);
		PORTB &= ~(1 << PORTB4);
	}
	else if (ADC_group == 3)
	{
		PORTB |= (1 << PORTB2);
		PORTB |= (1 << PORTB3);
		PORTB &= ~(1 << PORTB4);
	}
	else if (ADC_group == 4)
	{
		PORTB &= ~(1 << PORTB2);
		PORTB &= ~(1 << PORTB3);
		PORTB |= (1 << PORTB4);
	}
	else if (ADC_group == 5)
	{
		PORTB |= (1 << PORTB2);
		PORTB &= ~(1 << PORTB3);
		PORTB |= (1 << PORTB4);
	}
	else if (ADC_group == 6)
	{
		PORTB &= ~(1 << PORTB2);
		PORTB |= (1 << PORTB3);
		PORTB |= (1 << PORTB4);
	}
	else
	{
		PORTB |= (1 << PORTB2);
		PORTB |= (1 << PORTB3);
		PORTB |= (1 << PORTB4);
	}
}

int main(void){
	USART_init(); // Initializes the stuff for printing
	
	USART_putstring("Started \n");
	
	init_ADC();
	init_DAC();
	
	DDRD |= ( 1 << 6 );
	PORTD |= ( 1 << 6 );
	
	//DAC_out(7);
	
	while(1){
		sprintf(light_string, "%u", ADC);
		USART_putstring(light_string);
		USART_putstring("\n");
		sprintf(light_string, "%u", ADC_to_8_bit(ADC));
		USART_putstring(light_string);
		USART_putstring("\n");
		DAC_out(ADC_to_8_bit(ADC));
	}
}
