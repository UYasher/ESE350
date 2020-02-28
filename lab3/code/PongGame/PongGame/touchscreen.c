#include <avr/io.h>

int get_raw_y(void){
	//USART_putstring("Getting raw x\n");
	DDRC &= ~( 1 << 2 ) & ~( 1 << 4 ); // Set Y- and Y+ to input
	
	DDRC |= ( 1 << 5 ) | ( 1 << 3 ); // Set X- and X+ to output
	PORTC |= ( 1 << 5 ); // Set X- to high
	PORTC &= ~( 1 << 3 ); // Set X+ to low

	// Set ADC flags
	ADMUX |= (1 << REFS0); // AREF = AVcc
	// Set to read from Y- (ie PC2)
	ADMUX &= ~(1 << 3) & ~(1 << 2) & ~(1 << 0);
	ADMUX |= (1 << 1);
	ADCSRA |= (1 << ADEN); // ADC enable
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // 128 Prescaler
	ADCSRA |= (1 << ADSC); // Start conversion

	// Read ADC
	while(ADCSRA & (1<<ADSC));
	return ADC; // Return the x value
}

int get_raw_x(void){
	//USART_putstring("Getting raw y\n");
	//PORTC &= ~( 1 << 5 ) | ~( 1 << 3 ); // Pull X- and X+ low
	DDRC &= ~( 1 << 5 ) & ~( 1 << 3 ); // Set X- and X+ to input

	DDRC |= ( 1 << 2 ) | ( 1 << 4 ); // Set Y- and Y+ to output
	PORTC |= ( 1 << 2 ); // Set Y- to high
	PORTC &= ~( 1 << 4 ); // Set Y+ to low

	// Set ADC flags
	ADMUX |= (1 << REFS0); // AREF = AVcc
	// Set to read from X- (ie PC5)
	ADMUX |= (1 << 2) | (1 << 0);
	ADMUX &= ~(1 << 3) & ~(1 << 1);
	ADCSRA |= (1 << ADEN); // ADC enable
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // 128 Prescaler
	ADCSRA |= (1 << ADSC); // Start conversion

	// Read ADC
	while(ADCSRA & (1<<ADSC));
	return ADC; // Return the y value
}

int transform_x(int raw_x){
	//USART_putstring("Transforming x\n");
	const int shift = 180;
	const int scale = 6;
	return (raw_x - shift)/scale;
}

int transform_y(int raw_y){
	//USART_putstring("Transforming y\n");
	const int shift = 0;
	const int scale = 1;
	const int re_scale = 0;
	return (raw_y - shift)/scale + re_scale;
}

int get_x(void){
	int x = transform_x(get_raw_x());
	if (x>=30 && x<=50){
		return -1;
	}
	return x;
}

int get_y(void){
	int y = transform_x(get_raw_y());
	return y;
}
