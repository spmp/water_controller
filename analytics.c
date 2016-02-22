#include "analytics.h"

/**
 * Analytics functions for the Solar Hotwater project.
 **/


uint8_t analytics_begin_flag = 0;
uint8_t analytics_running_flag = 0;
uint8_t analytics_enable = 1;               //enabled by default

uint32_t daily_solar_gain = 0;
uint32_t previous_energy = 0;

// Disable analytics
void disable_analytics(void) {
    /* Set hardware states OFF */
    analytics_enable = 0;
}

// Enable analytics
void enable_analytics(void) {
    analytics_enable =1 ;
}

void analytics_run(struct Program *program) 
{
    //make sure only one instance of the analytics is running, and is enabled
    if (analytics_running_flag || !analytics_enable) {
        return;
    }
    analytics_running_flag = 1;
    
    analytics_solar_gain(program);
    
    // Run the analytics
    analytics_running_flag = 0;     //the analytics is finished
}

/**
 * Calculate the heat energy in a volume of water.
 * Cp (water ) = 4186 J/(kgK)     (SPECIFIC_HEAT_WATER)
 * Density water = 1000g/L
 * Mass = Volume x Density = L * 1 (kg)
 * Temerature in K
 * J = 4186 J/(kgK) * L (kg) * T (K)
 * @param temperature, the temperature of water in KxTEMPERATUREMULTIPLIER
 * @param voume, the volume of water in L
 */
uint32_t heat_energy(uint32_t temperature, uint16_t volume) {
    //Operation divided into two parts to maximise accuracy and not overflow
    uint32_t energy = (volume * temperature)/TEMPERATUREMULTIPLIER;
    return energy * SPECIFIC_HEAT_WATER;
}

/* Solar gain
 * The idea of the solar gain algorithm is to produce a cumulative measure of
 * the energy gained by the solar hot water system.
 * The idea goes like this
 * # Reset the gain counter (daily_solar_gain) to zero at midnight via
 * `analytics_reset`
 * # If the heater is not on calculate the energy in the solar system compare
 *  to the energy of the previous run.
 * # If the current energy is higher than the previous then append the 
 *  difference to `daily_solar_gain`.
 * NOTE: This accounts for energy loss due to water use - we may lose some time
 *  blocks due to this, but is overall quite robust
 * # Save the calculated solar gain as analytics_previous_energy
 */
void analytics_solar_gain(Program* program)
{
    struct Inputs *inputs = &program->inputs;
    struct Outputs *outputs = &program->outputs;
    
    if (outputs->heater == 0 && outputs->fill == 0) {
        uint32_t current_energy;
        
        current_energy = heat_energy((uint32_t) inputs->temperature, inputs->volume);
        if (current_energy > previous_energy && previous_energy != 0) {
            // Joules to Wh: 1/3600
            daily_solar_gain += (current_energy - previous_energy)/3600;
        }
        previous_energy = current_energy;
    } else {
        previous_energy = 0;
    }
}

void analytics_reset(void)
{
    daily_solar_gain = 0;
}

