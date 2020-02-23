/*
 * GccApplication1.c
 *
 * Created: 1/31/2020 2:55:11 PM
 * Author : laurelin
 */ 

#include <avr/io.h>


int main(void)
{
    
	DDRB &= ~(1 << 0);
	DDRB |= (1 << 5);
	
	
    while (1) 
    {
		if( PINB & (1 << 0) ){
			PORTB |= (1 << 5);
		}
		else{
			PORTB &= ~(1 << 5);
		}
		
    }
}

