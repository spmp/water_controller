#include "temperature.h"

#include <avr/io.h>
#include "i2c_safe.h"
#include "usart.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

/** @brief      Temperate measure, hardware agnostic abstraction
 *    @retval     temperature Temperate in °Cx1000
 */
uint16_t temperature(void){
    /*Strategies for temperature measurment from three sensors:
     1.    Use only highest sensor -- requires level
     2.    Average all submerged sensors -- requires level
     3.    Average all sensors
     4.    Use the maximal value
     5.    Average all sensors within some error of maximum
     */ 
    int32_t temp1 = read_AT30TSE758(TEMP_SENSOR1_ADDRESS);
    int32_t temp2 = read_AT30TSE758(TEMP_SENSOR2_ADDRESS);
    int32_t temp3 = read_AT30TSE758(TEMP_SENSOR3_ADDRESS);
//1.

//2.

//3.
//     uint16_t temp_avr = ((temp1+temp2+temp3)/3);
//4.
    int32_t temp_max = MAX(temp3,MAX(temp1,temp2));
//5.
//     uint16_t temp_graded_average = ;
    if (temp_max <=0){
        return 0;
    }
    
    return temp_max;
//     return temp1;

}

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
    @retval     2       NVRam write failed. OMG!
*/
uint8_t init_AT30TSE758(uint8_t address){
    uint8_t error = 0;
    // Do we need to write the changes -- If register = AT30TSE758_INIT then do nothing
    send_string("Reading config from address: ");
    send_uint16(address);
    send_newline();
    
    //Write config register, Error on no device.
    if( i2c_safe_write_word(address, AT30TSE758_CONFIG_REG, AT30TSE758_INIT)){
        return 1;
    }
    send_string("writing config to address: ");
    send_uint16(address);
    send_newline();
    
    //Check whether the config register is set to the initialisation parameter.
    if ( !(i2c_safe_read_word(address, AT30TSE758_CONFIG_REG) == AT30TSE758_INIT) ){
        return 3;
    }
    send_string("Reading NVRam from address: ");
    send_uint16(address);
    send_newline();
    
    //Write NVRam, padding for 16bit, error on no device.
    if ( i2c_safe_write_sixteen(address, AT30TSE758_NVRAM_REG,(AT30TSE758_INIT << 8)& 0xFFFF) != 0 ){
        return 4;
    }
    send_string("Writing NVRam to address: ");
    send_uint16(address);
    send_newline();
    
    //Check if NVRam matches initialisation parameter
    if ( i2c_safe_read_sixteen(address, AT30TSE758_NVRAM_REG) != (AT30TSE758_INIT << 8)& 0xFFFF ){
        return 5;
    }
    return 0;
}

/** @brief      Read temperature from an AT30TSE758 device
    @details    Temperature based on 11bits, first bit is sign (1 negative), then 10 temperature bits
                10 bit value is then shifted right 5 bits and multiplied by 0.125 to get °C. 
    @param      address         I2C address of the AT30TSE758
    @retval     temperature     in °Cx1000, or
    @retval     0xFFFF          if read failed
*/
int32_t read_AT30TSE758(uint8_t address){
    uint16_t tempraw = i2c_safe_read_sixteen(address, 0x0);
    if ( tempraw >= 0x8000 ) { //Temperature is negative
        // TODO: How do we know its negative!! aaarg crap, what bad coding!!!x
        return ( -((tempraw & 0x7FFF)>>5)*125);
    }
    else {
        return ( (tempraw>>5)*125);
    }
    return tempraw;
}

