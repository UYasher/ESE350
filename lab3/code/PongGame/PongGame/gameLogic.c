#include <time.h>
#include <stdlib.h>

int ball_x, ball_y, ball_vx, ball_vy;
int paddle1_y;
int paddle2_y;
int score1, score2;

int game_type; // 0 = accelerometer, 1 = 1 player, 2 = 2 player

int half_period  = 239/2;

void reset_positions(void){
	ball_x = 64;
	ball_y = 32;
	
	ball_vx = 3 - (rand() % 6);
	while(ball_vx == 0){
		ball_vx = 3 - (rand() % 6);
	}
	ball_vy = 3 - (rand() % 6);
	
	paddle1_y = 26;
	paddle2_y = 26;
}

int read_accelerometer(void){
	// Set ADC flags
	ADMUX |= (1 << REFS0); // AREF = AVcc
	// Set to read from X- (ie PC5)
	ADMUX |= (1 << 0);
	ADMUX &= ~(1 << 3) & ~(1 << 2) & ~(1 << 1);
	ADCSRA |= (1 << ADEN); // ADC enable
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // 128 Prescaler
	ADCSRA |= (1 << ADSC); // Start conversion

	// Read ADC
	while(ADCSRA & (1<<ADSC));
	char accel_char[20];
	sprintf(accel_char, "%d\n", ADC);
	USART_putstring(accel_char);
	return ADC; // Return the y value
}

void init_game(void){
	game_type = 0;
	
	srand(TCNT0);
	score1 = 0;
	score2 = 0;
	
	DDRB |= (1 << 1); // Set speaker to output
	DDRC &= ~(1 << 1); // Set accelerometer to input
	
	reset_positions();
}

void sound_buzzer(){
	// Sound buzzer
	PORTB |= (1 << 1);
	_delay_ms(100);
	PORTB &= ~(1 << 1);
}

void update_game_state(int cursor_x, int cursor_y){
	// Check if a player has lost
	if (score1 > 5 || score2 > 5){
		// Turn screen red
		DDRB |= (1 << 0) | (1 << 2);
		PORTB |= (1 << 0) | (1 << 2);
		_delay_ms(10000);
		PORTB &= ~(1 << 0) & ~(1 << 2);
		
		// Reset game
		init_game();
	}
	
	// Update ball velocity
	if ((ball_y <= 2) || (ball_y >= 62)){ // If ball hits a wall
		ball_vy *= -1;
		sound_buzzer();
	}
	if (ball_x <= 5){ // If ball hits player1 paddle
		if (ball_y >= paddle1_y && ball_y <= paddle1_y+12){
			sound_buzzer();
			ball_vx *= -1.5;
			ball_vy += 2 - (rand() % 4);
		}
	}
	if (ball_x >= 123){ // If ball hits player 2 paddle
		if (ball_y >= paddle2_y && ball_y <= paddle2_y+12){
			sound_buzzer();
			ball_vx *=-1.5;
			ball_vy += 2 - (rand() % 4);
		}
	}
	
	// If hits boundary
	if(ball_x < 3){
		score2 += 1;
		sound_buzzer();
		reset_positions();
	}
	if(ball_x > 125){
		sound_buzzer();
		score1 += 1;
		reset_positions();
	}
	
	// Update ball position
	ball_x += ball_vx;
	ball_y += ball_vy;
	
	// Update paddle position
	if (cursor_x >= 0 && cursor_x <= 127 && game_type != 0){ // If receiving valid input
		if (cursor_x <= 63){ // If first paddle
			if (cursor_y >= 64){ // If touching top of screen
				paddle1_y -= 6;
			}
			else{
				paddle1_y += 6;
			}
		}
		else{
			if (game_type == 2){
				if (cursor_y >= 64){
					paddle2_y -= 6;
				}
				else{
					paddle2_y += 6;
				}
			}
		}
	}
	if (game_type < 2){
		paddle2_y = ball_y - 6 + (rand() % 20);
	}
	if (game_type == 0){
		if (read_accelerometer() < 323){
			paddle1_y += 6;
		}
		if (read_accelerometer() > 335){
			paddle1_y -= 6;
		}
	}
	
	// Make sure paddle is not out of bounds
	if (paddle1_y <= 1){
		paddle1_y = 1;
	}
	if (paddle1_y >= 51){
		paddle1_y = 51;
	}
	if (paddle2_y <= 1){
		paddle2_y = 1;
	}
	if (paddle2_y >= 51){
		paddle2_y = 51;
	}
}

void render(void){
	clear_buffer(buff);
	
	// Draw Score
	char score_char[20];
	sprintf(score_char, "%d  %d", score1, score2);
	drawstring(buff, 54, 1, score_char);
	
	// Draw arena
	drawrect(buff, 0, 0, 128, 64, 0);
	drawrect(buff, 63, 0, 2, 64, 0);
	
	// Draw ball
	fillcircle(buff, ball_x, ball_y, 3, 0);
	
	// Draw paddles
	drawrect(buff, 1, paddle1_y, 2, 12, 0);
	drawrect(buff, 125, paddle2_y, 2, 12, 0);

	write_buffer(buff);
}