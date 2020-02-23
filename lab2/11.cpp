/*
 * GccApplication1.cpp
 *
 * Created: 2/6/2020 6:17:07 PM
 * Author : laurelin
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

int main() {
	
	DDRB |= (1 << 5); // SET PORTB5 to output
	DDRB |= ~(1 << 0); // SET PINB0 to input
	
	TCCR1B |= ( 1 << ICES1 ); // Set Input Compare 1 to detect rising edge
	TIMSK1 |= ( 1 << ICIE1 ); // Enable Timer Input Compare
	sei(); // Allow interrupts

	while(1); // Don't terminate
}

ISR(TIMER1_CAPT_vect){
	PORTB ^= (1 << 5); // Toggle LED
}

