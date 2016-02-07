#include "temperature.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

int32_t temp_max = 100*TEMPERATUREMULTIPLIER; // Initialise temperature high to ensure heater does not turn on

/** 
 @brief      Temperate measure, hardware agnostic abstraction
 @retval     temperature Temperate in °Cx10000
 */
int32_t temperature(void){
    ds18x20_maxt_full_toggle(&temp_max);
    
    return temp_max;
}

/** @brief      The change in temperature over one second
    @retval     delta_t The change in temperature over one second in °Cx1000
*/
uint8_t deltat(void);

/* Hardware specific routines */