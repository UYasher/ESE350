/*
 * GccApplication2.c
 *
 * Created: 1/31/2020 3:33:16 PM
 * Author : laurelin
 */ 

#include <avr/io.h>


int main(void)
{
    
	DDRD |= (1 << 6); // Set Pin PD6 to output
	TCNT0 = 0; // Initialize Timer
	TCCR0B = (1<<CS00) | (1<<CS02); // Set timer scaler to 1024
	PORTD |= (1 << 6);
	
    while (1) 
    {
		if(TIFR0 & (1 << 0)){ // If timer has overflowed
			PORTD ^= (1 << 6); // Flip output to speaker
			TIFR0 |= ( 1 << 0 ); // Reset Timer overflow flag
		}
    }
}

