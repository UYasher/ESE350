#include <avr/io.h>

int get_raw_x(void){
    DDRC |= ( 1 << 5 ) &|( 1 << 3 ); // Set X- and X+ to output
    PORTC |= ( 1 << 5 ); // Set X- to high
    PORTC &= ~( 1 << 3 ); // Set X+ to low

    DDRC &= ~( 1 << 2 ) & ~( 1 << 4 ); // Set Y- and Y+ to input

    // Set ADC flags
    ADMUX = (1 << REFS0); // AREF = AVcc
    // Set to read from Y- (ie PC2)
    ADMUX &= ~(1 << 3) | ~(1 << 2) | ~(1 << 0);
    ADMUX |= ~(1 << 1);
    ADCSRA = (1 << ADEN); // ADC enable
    ADCSRA = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // 128 Prescaler
    ADCSRA |= (1 << ADSC); // Start conversion

    // Read ADC
    while(ADCSRA & (1<<ADSC));
    return ADC; // Return the x value
}

int get_raw_y(void){
    DDRC &= ~( 1 << 5 ) | ~( 1 << 3 ); // Set X- and X+ to input

    DDRC |= ( 1 << 2 ) & ( 1 << 4 ); // Set Y- and Y+ to output
    PORTC |= ( 1 << 2 ); // Set Y- to high
    PORTC &= ~( 1 << 4 ); // Set Y+ to low

    // Set ADC flags
    ADMUX = (1 << REFS0); // AREF = AVcc
    // Set to read from X- (ie PC5)
    ADMUX &= ~(1 << 3) | ~(1 << 1);
    ADMUX |= (1 << 2) | (1 << 0);
    ADCSRA = (1 << ADEN); // ADC enable
    ADCSRA = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // 128 Prescaler
    ADCSRA |= (1 << ADSC); // Start conversion

    // Read ADC
    while(ADCSRA & (1<<ADSC));
    return ADC; // Return the x value
}

int transform_x(int raw_x){
    const shift = 0;
    const scale = 2;
    return (raw_x - shift)* scale;
}

int transform_x(int raw_x){
    const shift = 0;
    const scale = 2;
    return (raw_x - shift)* scale;
}

int get_x(void){
    return transform_x(get_raw_x());
}

int get_y(void){
    return transform_y(get_raw_y());
}

int main(void){

}
