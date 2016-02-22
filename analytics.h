#pragma once
#include <avr/io.h>
#include "hardware.h"
#include "state-machine.h"

/**
 * Analytics functions for the Solar Hotwater project.
 * The functions within this library are intended to be run on a long timescale
 * and do calculations that are not immediatly nescessary for the running of
 * the system. 
 * The intended list of functions is:
 * analytics_solar_gain, a routine to calculate the solar gain in Watts in a day
 * analytics_fill_gain, a routine to calculate how much water can be added to
 *  the system given the current solar gain such that the required temperature
 *  is maintained
 * analytics_midday_sun, a routine to calculate the middle of the solar day
 * anayltics_reset, a function to be run once a day to reset gain etc.
 **/

// Whether to run the analytics
extern uint8_t analytics_begin_flag;
extern uint8_t analytics_running_flag;

// Approximate daily solar gain in Watt hours (Wh)
extern uint32_t daily_solar_gain;

// Enable/disable analytics
void disable_analytics( void );
void enable_analytics( void );

void analytics_run(struct Program *program);

void analytics_solar_gain(struct Program *program);

void analytics_reset(void);
