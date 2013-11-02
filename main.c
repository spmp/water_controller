#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "usart.h"
#include "clock.h"

uint16_t distance;

/* the two following interrupts are used to measure the time for which
 * the echo signal of the range sensor is HI. The first interrupt fires
 * on the leading edge, and starts the 16bit timer1 at 0. The second interrupt
 * fires on the falling edge, just after the value of timer1 has been written
 * to the ICR1 register. */

void init_ultrasonic() {
    PCMSK0 = (1 << PCINT0); /* interrupt mask for DDB0 */

    /* timer 1: interrupt on ICP1 falling, 2MHz, dt=0.5us */
    TCCR1A = 0;
    TCCR1B = (1 << CS11);
    TIMSK1 = (1 << ICIE1);
}

/* rising edge of DDB0 only */
ISR(PCINT0_vect) {
    TCNT1 = 0; /* reset timer1 */
    TCCR1B = (1 << CS11); /* enable timer1 */
    PCICR &= ~(1 << PCIE0); /* disable DDB0 interrupt */
}

/* falling edge of DDB0 only */
ISR(TIMER1_CAPT_vect) {
    distance = ICR1; /* turn off falling edge interrupt */
    TCCR1B &= ~(1 << CS11); /* disable timer1 */
}

void run_distance_measure() {
    PCICR |= (1 << PCIE0); /* enable DDB0 rising interrupt */

    /* ping the range sensor for 10us to start a range measurement */
    DDRB |= (1 << DDB4);
    PORTB |= (1 << PORTB4);
    _delay_us(10);
    PORTB &= ~(1 << PORTB4);
}

uint16_t read_analog_pin(uint8_t adc_pin) {
    /* AVcc as reference voltage */
    ADMUX = (1 << REFS0) | (adc_pin & 0xF);
    /* start measure & wait until complete; prescaler 128 */
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

    run_distance_measure();
    _delay_ms(120);
}

void toggle_led() {
    PORTB ^= (1 << PORTB5);
}

int main() {
    cli();
    init_clock();
    init_usart();
    init_ultrasonic();

    DDRB |= (1 << DDB5);
    oncePerSecondCallback = &toggle_led;
    sei();

    struct Program program;
    for (;;) {
        loop(&program);
    }

    return 0;
}

