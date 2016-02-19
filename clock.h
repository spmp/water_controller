#pragma once
#include <avr/io.h>

/* Medium and fast time intervals in 1/125'th of a second */
#define CLOCK_TICKS_PER_SECOND  125 // DONT CHANGE, place holder not configable
#ifndef MEDIUM_TIME_INTERVAL
#define MEDIUM_TIME_INTERVAL    25
#endif
#ifndef FAST_TIME_INTERVAL
#define FAST_TIME_INTERVAL      5
#endif

/* Resources: Timer/Counter 2 */

/* unix timestamp: seconds since epoch */
extern uint32_t timestamp;

/* 125 ticks per second */
extern uint16_t ticks;

void init_clock();

/* callback that is called once every second. N.B. is called within an interrupt */
void clock_set_seconds_callback(void (* secondsCallback)());

/* callback that is called once every FAST_TIME_INTERVAL. For closer to realtime activity
 * N.B. is called within an interrupt */
void clock_set_fast_time_callback(void (* fast_Callback)());

/* callback that is called once every MEDIUM_TIME_INTERVAL. For closer to realtime activity
 * N.B. is called within an interrupt */
void clock_set_medium_time_callback(void (* medium_Callback)());