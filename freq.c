#include <avr/interrupt.h>
#include "freq.h"

/* Frequency counter for pressure sensor -- only frequency counting here!
 *  Need to measure 20-30kHz with a 16bit timer.
 * Longest time 20kHz = 50e-6 seconds
 * 16MHz with 16bits gives ~4.1e-3 seconds
 * no prescalar required.
 * 
 * Using 16bit timer/counter with input capture to measure frequqncy
 * To simplify operations we will use an /8 prescalar such that we have
 * a 2Mhz clock.
 * 
 * Resources:16 timer counter 
 * Digital input ICP1/Arduino pin 8
 * 
 * */
uint16_t currentICR;	/* May not need this if using second ISR method */
uint16_t previousICR;
uint16_t diffICR;

/* Setup 16bit counter/timer */
void init_freq() {
    /* Normal mode (default, nothing required) */
    TCCR1B = (1 <<  ICES1) | (1 << CS10); /*Rising edge input capture, no pre-scalar */
    TIMSK1 = (1 << ICIE1); /* Input capture interrupt enable */
}

/* Input Capture ISR */
ISR(TIMER1_CAPT_vect) { 
    currentICR = ICR1;
    /* Check if timer/counter1 has overflowed */
    if ( (TIFR1 & (1 << TOV1)) == 0 ){ 
       diffICR = currentICR  - previousICR;    /* calc difference */
    }
    else {
    /* If overflowed calculate diff as followes */
       diffICR = 65535 - previousICR + currentICR;
       TIFR1 &= ~(1 << TOV1);
    } 
    previousICR = ICR1;    /* update previous value, even if changed during ISR. i.e. dont use currentICR */
}

/* Convert the edge delta to a frequency */
uint16_t freq() {
    uint16_t frequency;
    frequency = ( F_CPU / (diffICR*FREQPRESCALE));
    return frequency;
}
