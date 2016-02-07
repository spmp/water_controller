#pragma once

/* Provides hardware abstraction to give water temperature in °C and 
 * calibrate the sensor.
 *  Hardware Dependency:
 *   TWI/I2C Bus
 *     SCL/SCA pins (Analog input 4 and 5) connected via pull up resistor to VCC
 */

#include <avr/io.h>
#include "AVR-lib/ds18x20.h"
#include "AVR-lib/usart.h"

#define TEMPERATUREMULTIPLIER 10000L

/** @brief      Temperate measure, hardware agnostic abstraction
    @retval     temperature Temperate in °CxTEMPERATUREMULTIPLIER
*/
int32_t temperature(void);

/** @brief      The change in temperature over one second
    @retval     delta_t The change in temperature over one second in °CxTEMPERATUREMULTIPLIER
*/
uint8_t deltat(void);

/* Hardware specific routines */
// None ATM
