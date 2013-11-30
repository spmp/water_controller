#pragma once
#include <avr/io.h>
#include <avr/interrupt.h>

#define FREQPRESCALE 1

/* Resources: 	16bit Timer/Counter 1
 * 				ICP1 pin/PB0/Arduino digital pin 8 */

/* Difference in ICR timer between pulses */
extern uint16_t diffICR;

void init_freq();

uint16_t freq();
