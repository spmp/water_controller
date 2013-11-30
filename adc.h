#pragma once
#include <avr/io.h>

/* Resources: 	16Bit ADC
 * 				ADC input's 1 to 8
 */
 
uint16_t read_analog_pin(uint8_t);
