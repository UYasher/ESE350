//**** LCD Pong Lab
/* This sample main file gives some example functions which can be implemented for the pong game
Following are the criterion of the game :
a. Two paddles along Y axis. Paddle height between 8-12 pixels. Width ~2pixel
b. Gridlines for the visible LCD region/game area
c. Ball < 3pixel radius, preferably originating at center of display
d. Score keeping for each player
e. Paddles move on touch screen input. Keep in mind to have bounded input region for each for simplicity
f. Ball follows proper game dynamics. Starts with random direction and speed
g. Generate sounds in game : when balls hits surfaces, hits paddles or misses paddles
h. Reset game if a player loses

These are not the exhaustive limits of the game. There can be more features to make the game interesting 
as long as these criterion are met.

Example functions and variables are provided to get you started. You structure your code independently from that.
*/

#define F_CPU 16000000UL
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <util/delay.h>
#include "lcd.h"

#define BAUD 9600
#define HIGH 1
#define LOW 0
//*** Size of the buffer = 128 bytes * 8pages
#define BUFFER 1024
//*** Use for colour
#define BLACK 0x000001

//Define Screen size variables
//Define paddle parameter variables
//Define ball parameter variables

//**** Example function to generate different game tones
void beep_tone()

//***** Example function checks for Touchscreen inputs and changes the paddle positions  *****//
void checkInput()
{}

//***** Example function to update game parameters
void update()
{}

//**** Example draw function to draw all the game sprites/graphics
void draw()
{}

//**** Example function checks for Horizontal and Vertical collisions of the Ball
void checkCollisions()
{}

//**** Example function which reads the X coordinate for the touchscreen 
int readX()
{}

//**** Example function which reads the Y coordinate for the touchscreen 
int readY()
{}


int main(void)
{

//** Setup the backlight of the LCD - Red, Blue and Green - all on
	DDRD |= 0x80; 
	PORTD &= ~0x80;
	PORTD |= 0x00;
	
	DDRB |= 0x05;
	PORTB &= ~0x05;
	PORTB |= 0x00;

	sei();
	
	lcd_init();
	lcd_command(CMD_DISPLAY_ON);
	lcd_set_brightness(0x18);
	write_buffer(buff);
	_delay_ms(1500);
	
	while (1)
	{	
		checkInput();
  	checkCollisions(); 
  	update();          
 		draw();
	}
}

