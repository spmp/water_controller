#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "usart.h"
#include "clock.h"

uint16_t distance;
uint16_t rising_edge;

ISR(TIMER1_CAPT_vect) {
    if (TCCR1B & (1 << ICES1)) { /* rising edge */
        TCCR1B &= ~(1 << ICES1); /* capture next on falling edge */
        rising_edge = ICR1;
    } else { /* falling edge */
        TCCR1B = 0; /* stop timer1 */
        TIMSK1 = 0;
        distance = ICR1 - rising_edge;
    }
}

void trigger_ultrasonic_measure() {
    DDRB |= (1 << DDB0);
    PORTB |= (1 << PORTB0);
    _delay_us(10);
    PORTB &= ~(1 << PORTB0);
    DDRB &= ~(1 << DDB0);
    /* timer 1: interrupt on ICP1 rising, 2MHz, dt=0.5us */
    TCCR1A = 0;
    TCNT1 = 0;
    TIMSK1 = (1 << ICIE1);
    TCCR1B = (1 << ICES1) | (1 << CS11);
}

uint16_t read_analog_pin(uint8_t adc_pin) {
    /* AVcc as reference voltage */
    ADMUX = (1 << REFS0) | (adc_pin & 0xF);
    /* start measure with prescaler 128 & wait until complete */
    ADCSRA = (1 << ADEN) | (1 << ADSC) | 0x7;
    while ((ADCSRA & (1 << ADSC)) != 0);
    return ADC;
}

struct Inputs {
    uint16_t distance;
    uint16_t pot_value;
};

struct Program {
    struct Inputs inputs;
};

struct Inputs read_inputs() {
    struct Inputs inputs;
    inputs.distance = distance;
    inputs.pot_value = read_analog_pin(3);
    return inputs;
}

void write_state_to_usart(struct Program *program) {
    send_char('a');
    send_char(' ');
    send_uint16(timestamp & 0xFFFF);
    send_char(' ');
    send_uint16(program->inputs.distance);
    send_char(' ');
    send_uint16(program->inputs.pot_value);
    send_char('\r');
    send_char('\n');
}

void loop(struct Program *program) {
    program->inputs = read_inputs();
    write_state_to_usart(program);

    trigger_ultrasonic_measure();
    _delay_ms(120);
}

void toggle_led() {
    PORTB ^= (1 << PORTB5);
}

int main() {
    cli();
    init_clock();
    init_usart();

    DDRB |= (1 << DDB5);
    oncePerSecondCallback = &toggle_led;
    sei();

    struct Program program;
    for (;;) {
        loop(&program);
    }

    return 0;
}

