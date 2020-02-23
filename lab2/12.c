/*
 * GccApplication2.c
 *
 * Created: 2/6/2020 7:20:59 PM
 * Author : laurelin
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

// Define length of period
unsigned int half_period = 142/2;

int main(void)
{

	DDRD |= ( 1 << 6 ); // Set PD6 (OC0A) to output
	
	// Set Output Compare 0 to toggle
	TCCR0A |= ( 1 << COM0A0); 
	TCCR0A &= ~( 1 << COM0A1 );
	
	// Set Timer0 prescaler to 256
	TCCR0B &= ~( 1 << CS01);
	TCCR0B &= ~( 1 << CS00);
	TCCR0B |= ( (1 << CS02));
	
	OCR0A += TCNT0 + half_period; // Set output compare value
	TIMSK0 |= ( 1 << OCIE0A ); // Turn on output compare
	sei();

    while (1);
}


ISR(TIMER0_COMPA_vect){
	OCR0A += half_period; // Increment for next half period
}
