#pragma once
#include "adc.h"

/* Provides hardware abstraction to give water temperature in °C and 
 * calibrate the sensor. One hardware dependency, what pin is the 
 * sensor attached to */
 
/* Water temperature in floating point degrees celcius
 * First parameter is adc value from sensor, second is calibration parameter */
 float water_temperature_f(uint16_t, float);
 
/* Water temperature in integer degrees celcius
 * First parameter is adc value from sensor, second is calibration parameter */
int8_t water_temperature(uint16_t, float);

/* Calibrate the temperature sensor at known temperature in °C
 * Input temperature to the nearest degree, and ADC pin (0-7) */
float calibrate_temperature(uint8_t, uint8_t);
 
