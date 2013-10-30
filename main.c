#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "usart.h"

uint16_t distance;

/* the two following interrupts are used to measure the time for which
 * the echo signal of the range sensor is HI. The first interrupt fires
 * on the leading edge, and starts the 16bit timer1 at 0. The second interrupt
 * fires on the falling edge, just after the value of timer1 has been written
 * to the ICR1 register. */

void init_ultrasonic() {
    /* outputs: DDB4, DDB5; inputs: DDB0 */
    DDRB = (1 << DDB4) | (1 << DDB5);
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
    PORTB |= (1 << PORTB5);
}

/* falling edge of DDB0 only */
ISR(TIMER1_CAPT_vect) {
    /* turn off falling edge interrupt */
    distance = ICR1;
    TCCR1B &= ~(1 << CS11); /* disable timer1 */
    PORTB &= ~(1 << PORTB5);

    /* TODO: move out of interrupt */
    send_uint16(distance);
    send_char('\r');
    send_char('\n');
}

void run_distance_measure() {
    PCICR |= (1 << PCIE0); /* enable DDB0 rising interrupt */

    /* ping the range sensor for 20us to start a range measurement */
    PORTB |= (1 << PORTB4);
    _delay_us(20);
    PORTB &= ~(1 << PORTB4);
}

void init_analog() {
    /* turn on analog, division factor of 16 */
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t read_analog_pin(uint8_t adc_pin) {
    /* AVcc as reference voltage */
    ADMUX = (1 << REFS0) | (adc_pin & 0xF);
    /* start measure & wait until complete */
    ADCSRA |= (1 << ADSC);
    while ((ADCSRA & (1 << ADSC)) != 0);

    return ADC;
}

void run_analog_measure() {
    send_char('a');
    send_char(' ');
    send_uint16(read_analog_pin(3));
    send_char(' ');
}

void loop() {
    run_analog_measure();
    _delay_ms(50);
    run_distance_measure();
    _delay_ms(100);
}

int main() {
    cli();
    init_usart();
    init_analog();
    init_ultrasonic();
    sei();

    for (;;) loop();

    return 0;
}

