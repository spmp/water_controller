#pragma once
#include <avr/io.h>
#include <avr/interrupt.h>

#define FREQPRESCALE 1

/* Resources: 	16bit Timer/Counter 1
 * 				ICP1 pin/PB0/Arduino digital pin 8 */

/* Initialise the frequency counter.
 * 16bit timer in input capture mode  */
void init_freq();

/* Convert the current difference in time between rising edges to a
 * frequency in Hz*10  */
uint16_t freq();

/* Temproary for testing */
uint16_t freqdiff();
