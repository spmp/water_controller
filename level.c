#include "level.h"
#include <math.h>
#include "i2c_safe.h"

uint16_t level_sensor_zero = LEVEL_ZERO_DEFAULT;

/** @brief      Hardware abstraction for level
    @details    Reads the level from whatever device is being used. As much as 
                possible a placeholder for the hardware specific function.
                The linear factor for mV to mm of water is 1.400*code - 236.
                This is modified to not overflow uint16 and use arithmatic shifts
                There is an offset that must be zeroed out.
                Expect maximum code of 450 (400mm), can use:
                (code*140)/1000 with no overflow
                
    @retval     water_level in mm
*/
uint16_t level( void ){
//     uint16_t level = read_MCP3221()*1.74064 - level_sensor_zero;
//     if (level >= 40000 ) { //We have underflowed
//         return 0;
//     }
    uint16_t level = read_MCP3221();
    if (level <= level_sensor_zero/0.1384)
        level = 0;
    else {
        level = 0.1384*level-level_sensor_zero;
//         level = 0.13877*level-125 // Run 3 from data
    }
    return level;  
    //TODO: do this with bit shift, ie modify 133 etc
}

/** @brief      Hardware abstraction for volume
    @details    Using the level() function to determine the volume based on the
                profile of the tank.
                Based on a piecwise defined function for (I) level < radius and
                (II) level >= radius. with 
                theta = (I) 2*acos( (r-h)/r) , (II) 2*acos( (2r-h)/r )
                area = (I) r^2(theta - sin(theta))/2, (II) pi*r^2-r^2(theta - sin(theta))/2
    @param      Level in mm
    @retval     water_volume in litres, Tank maximum returned for volume >= 2*radius
*/
uint16_t volume( uint16_t level ){
     // This floating point code does a fantastic job, but can be approximated linearly without loss of usefulness (ie +/- 10 minutes)*
    double levelf = level;
    double theta;
    double volume;
    if (levelf <= 2*TANK_RADIUS){
        theta = 2*acos((TANK_RADIUS-levelf)/TANK_RADIUS);
        volume = TANK_LENGTH*( TANK_RADIUS*TANK_RADIUS*(theta - sin(theta))/2)/1000000;
    }
    else {
        volume = M_PI*TANK_RADIUS*TANK_RADIUS*TANK_LENGTH/1000000;
    }
    return volume;      //implicit type conversion

    
//     /* Linear approximation that takes a linear factor as (Volume full / level full)x1000 */
//     uint32_t volume = VOLUME_LINEAR_FACTOR * level / 1000;
//     return volume;
}


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
int16_t read_MCP3221( void ){
    return i2c_safe_read_sixteen(0x4D,0xFF); //read address 0x4D, with no register (0xFFF)
}

/** @brief sensor zeroing for linear offset
 */
void level_zero(void) {
    level_sensor_zero = 300;
//     level_sensor_zero = level();
}
    
