#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include "usart.h"

unsigned int pulse_interval = 160/8;
unsigned int wait_interval = 45000;
unsigned int i;
unsigned int width = 0;
int times_in_capt = 0;
int falling_edge_detected = 0;
char width_string[10];

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

int main(void){ //main function
	USART_init(); //initializes the stuff for printing
	
	USART_putstring("Started \n");
	
	DDRB |= ( 1 << 1 ); // Set PB1 to Output
	DDRB &= ~( 1 << 0 ); // Set PB0 to Input
	
	// Set Compare Output Mode to Toggle
	TCCR1A |= ( 1 << COM1A0);
	TCCR1A &= ~( 1 << COM1A1 );
	
	// Init timer and set prescaler
	TCCR1B &= ~( 1 << CS10);
	TCCR1B |= ( 1 << CS11);
	TCCR1B &= ~( 1 << CS12);
	
	
	OCR1A = TCNT1 + pulse_interval; // Set output compare value
	
	TIMSK1 |= ( 1 << OCIE1A ); // Turn on output compare
	TIMSK1 |= ( 1 << ICIE1 ); // Enable Timer Input Compare
	
	sei();
	while(1){
		
		// We need this for reasons beyond our comprehension
		char in_capt_string[1];
		sprintf(in_capt_string, "%d \n", times_in_capt);
		
		// Print most recent pulse width measurement
		if (width){
			USART_putstring("___WIDTH___\n");
			//char width_string[10];
			sprintf(width_string, "%d \n", width);
			USART_putstring(width_string);
			width = 0;
		}
	}
}

ISR(TIMER1_COMPA_vect){ //when timer = ocr1a value
	//USART_putstring("In output compare vector");
	if (DDRB & (1 << 1)){ // if pb1 is output
		DDRB &= ~( 1 << 1 ); // Set PB1 as input
		TCCR1B |= ( 1 << ICES1 ); // Configure Timer1 for input capture on rising edge
		OCR1A = TCNT1 + wait_interval;
	}
	else{
		// Generate next 10us signal
		DDRB |= ( 1 << 1 ); // Set PB1 as output
		OCR1A = TCNT1 + pulse_interval; // Set output compare value
		PORTB |= ( 1 << 1 ); // Set PB1 to High
		TIMSK1 |= ( 1 << OCIE1A ); // Turn on output compare
	}
}

ISR(TIMER1_CAPT_vect){ //when an edge is detected; init detect rising
	//times_in_capt += 1;
	if ( TCCR1B & ( 1 << ICES1 ) ){ //if rising edge detected
		falling_edge_detected = 0;
		i = ICR1; // Record the time when the rising edge was detected
		TIMSK1 |= ( 1 << ICIE1 ); // Enable Timer Input Compare
		TCCR1B &= ~( 1 << ICES1 ); // Set to detect a falling edge
	}
	else{
		width = ICR1 - i;
		falling_edge_detected = 1;
		
		// Generate next 10us signal
		DDRB |= ( 1 << 1 ); // Set PB1 as output
		OCR1A = TCNT1 + pulse_interval; // Set output compare value
		PORTB |= ( 1 << 1 ); // Set PB1 to High
		TIMSK1 |= ( 1 << OCIE1A ); // Turn on output compare
	}
}
