#pragma once

/* Provides hardware abstraction to give water temperature in °C and 
 * calibrate the sensor.
 *  Hardware Dependency:
 *   TWI/I2C Bus
 *     SCL/SCA pins (Analog input 4 and 5) connected via pull up resistor to VCC
 */

#include <avr/io.h>
#include "i2c_safe.h"

// I2C sensor addresses
#ifndef TEMP_SENSOR1_ADDRESS
#define TEMP_SENSOR1_ADDRESS    0x49 // 1001100
#endif
#ifndef TEMP_SENSOR2_ADDRESS
#define TEMP_SENSOR2_ADDRESS    0x4B // 1001110
#endif
#ifndef TEMP_SENSOR3_ADDRESS
#define TEMP_SENSOR3_ADDRESS    0x4F // 1001111
#endif

// AT30TSE758 initialisation data to be written to NVRam (first byte)
#ifndef AT30TSE758_INIT
#define AT30TSE758_INIT         0x40 // 11 bit, all else default
#endif

#define AT30TSE758_TEMP_REG     0x00
#define AT30TSE758_CONFIG_REG   0x01
#define AT30TSE758_NVRAM_REG    0x11


/** @brief      Temperate measure, hardware agnostic abstraction
    @retval     temperature Temperate in °Cx1000
*/
int16_t temperature(void);

/** @brief      The change in temperature over one second
    @retval     delta_t The change in temperature over one second in °Cx1000
*/
uint8_t deltat(void);

/* Hardware specific routines */

/***********AT30TSE758************
 * I2C temperature sensor and
 * eeeprom.
 * Adress range         0b1001XXX
 * Temperate reg.       0x00
 * Configuration reg.   0x01
 * NVRam config reg.    0x11
 ********************************/

/** @brief      Initialise AT30TSE758. Only needed once per device.
    @details    Set config and NVRam registers. Only needs to be run once per device, as the NVRam settings are loaded on power up.
                Configuration word is taken from AT30TSE758_INIT
    @param      address I2C address of the AT30TSE758
    @retval     0       Success, configuration and NVRam registers updated
    @retval     1       Failure , configuration and NVRam registers not correctly updated! Check their state!
*/
uint8_t init_AT30TSE758(uint8_t address);

/** @brief      Read temperature from an AT30TSE758 device
    @param      address         I2C address of the AT30TSE758
    @retval     temperature     in °Cx1000, or
    @retval     0xFFFF          if read failed
*/
int16_t read_AT30TSE758(uint8_t address);
