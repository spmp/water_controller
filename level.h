#pragma once
/*
 * This library proviced the hardware abstraction for Level to the state machine
 */

#include <avr/io.h>

/** @brief      Hardware abstraction for level
    @details    Reads the level from whatever device is being used. As much as 
                possible a placeholder for the hardware specific function.
    @retval     water_level in mm
*/
uint16_t level( void );

/** @brief      Hardware abstraction for volume
    @details    Using the level() function to determine the volume based on the
                profile of the tank.
    @retval     water_volume in milli litres
*/
uint16_t volume( void );


/***** Hardware Specific! *****
 * 
 * Current implimentation is a best effort from the wrong hardware!
 *      The pressure sensor has too large a range, with an output proportional to
 *      the Vdd, requiring an ADC which takes Vref from Vdd.
 *      Ideally I will find a 0-5kPa pressure sensor and a 16bit I2C ADC with Vref
 *      from Vdd
 * Currently the pressure sensor is a MPXV5050GP 0-50kPa, with transfer function
 *      V out = Vs(P x 0.018 + 0.04)±(Pressure Error x Temp.Factor x 0.018 x Vs )
 *              Vs = 5.0 V ± 0.25 Vdd
 * Currently the I2C ADC is microchip 12bit MCP3221, uses Vs as Vref, has no registers
 * requiring custom read library OR modification of I2C read such that 0xFF is no library at all...
 * will do that!
 * There is only one possible address for the MCP3221 unless specifically ordered
 * so address is hard coded. 
 *  Transfer function as code = Vdd/2^12 
 * Thus the final transfer function kPa->code is:
 *      code(12bit) = 2^12 x (P x 0.018 + 0.04)
 *      P (kPa)     = code/(2^12*0.018)
 *      Level (mm) = kpa x 98
 *      Level = 98*code/(2^12*0.018) = 98/74*code
 *      need to return micro meters = 1329 * code
 */


/***********MCP3221**************
 * I2C 12bit ADC, Vdd=Vref
 * Adress range         0b1001101
 * no writable registers
 ********************************/

/** @brief      Read temperature from an MPXV5050GP device
    @retval     code    12bit code
*/
int16_t read_MCP3221( void );