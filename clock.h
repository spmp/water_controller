#pragma once
#include <avr/io.h>

/* Resources: Timer/Counter 2 */

/* callback that is called once every second. N.B. is called within an interrupt */
extern void (* oncePerSecondCallback)();

/* unix timestamp: seconds since epoch */
extern uint32_t timestamp;

/* 125 ticks per second */
extern uint16_t ticks;

void init_clock();

