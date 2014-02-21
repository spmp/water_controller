#include <avr/interrupt.h>
#include "clock.h"

void (* oncePerSecondCallback)();       // void function pointer
void (* fastCallback)();                // void function pointer
void (* mediumCallback)();                // void function pointer

uint32_t timestamp;
uint16_t ticks;
uint16_t mediumTime;
uint16_t fastTime;

void clock_set_seconds_callback(void (* secondsCallback)()) {
    oncePerSecondCallback = secondsCallback;
}

void clock_set_fast_time_callback(void (* fast_Callback)()) {
    fastCallback = fast_Callback;
}

void clock_set_medium_time_callback(void (* medium_Callback)()) {
    mediumCallback = medium_Callback;
}

ISR(TIMER2_COMPA_vect) {
    /* increment clock */
    ticks += 1;
    mediumTime +=1;
    fastCallback += 1; //Would be better to lump all this together and force medim and fast as a mod(125,fast/med time) check 
    
    if ( fastCallback && (fastTime >= FAST_TIME_INTERVAL) ) {
        fastTime = 0;
        fastCallback();
    }
    
    if (mediumCallback && (mediumTime >= MEDIUM_TIME_INTERVAL) ) {
        mediumTime = 0;
        mediumCallback();
    }
    
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

