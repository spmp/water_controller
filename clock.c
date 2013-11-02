#include <avr/interrupt.h>
#include "clock.h"

void (* oncePerSecondCallback)();
uint32_t timestamp;
uint16_t ticks;

ISR(TIMER2_COMPA_vect) {
    /* increment clock */
    ticks += 1;
    if (ticks >= 125) {
        ticks = 0;
        timestamp += 1;
        if (oncePerSecondCallback) {
            oncePerSecondCallback();
        }
    }
}

void init_clock() {
    ticks = 0;
    timestamp = 0;

    DDRB = (1 << DDB5);

    TCCR2A = (1 << WGM21); /* clear timer on compare match */
    TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20); /* 1024 pre-scaler */
    TIMSK2 = (1 << OCIE2A); /* compare match interrupt */
    OCR2A = 125 - 1;
}

