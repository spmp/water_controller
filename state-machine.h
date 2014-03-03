#pragma once
#include <avr/io.h>
#include <avr/interrupt.h>
#include "hardware.h"

extern uint8_t begin_state_machine_flag;
extern uint8_t state_machine_running_flag;

struct Inputs {
    
    uint8_t reading_inputs;     //This pair of values is for handling of inputs that take a long time to obtain.
    uint8_t waiting_on_inputs;
    
    /* These values contain the input metrics for the state machine */
    uint16_t temperature; // degrees celcius (C)
    uint16_t level; // height (mm)
    uint16_t volume; // Litres
    
};

struct Outputs {
    /* Pump state, on or off */
    uint8_t pump;
    /* Fill state, on or off */
    uint8_t fill;
    /* Heating state */
    uint8_t heater;
    /* Heater 1 state */
    uint8_t heat1;
    /* Heater 2 start */
    uint8_t heat2;
    /* Intermediary states */
    uint8_t filling;    // Is the system filling? This could be acertained by the fill output state... may go soon!
    uint8_t heating;
//     uint8_t boosting;
};

struct Settings {
    /* Time settings */
    uint32_t time_to_hot_1;     // Time (1) to have tank at set temp by
    uint32_t time_to_hot_2;     // Time (2) to have tank at set temp by
    uint32_t midsun;            // Time at which the sun was/is at its peak
    /* Level settings */
    uint16_t level_full;        // The level of a full tank. Do not exceed 8) // possibly hard wired
    uint16_t level_heater_min;  // The minimum safe level for running the heating elements
    uint16_t level_min;         // The minimum allowable level of water in the tank
    uint16_t level_fill;        // The level to fill the tank to
    /* Volume settings */
//     uint16_t 
    /* Temperature settings */
    uint8_t temperature_settemp;
    uint8_t temperature_set_1;  // The temperature to which the system will automatically be heated
    uint8_t temperature_set_2;
    uint8_t temperature_max;    // Maximum temperature to maintain
    uint8_t temperature_min;    // Minimum temperature to maintain
    /* Smarts */
    uint16_t daily_heat_potential;      // How much energy to expect in a day
    /* Output settings */
    uint8_t pump_enable;
    uint8_t fill_enable;
    uint8_t heater_enable;
    /* Intermediate states */
    uint8_t fill_now;
    uint8_t boost_now;
    /* Configuration settings */
};

struct Program {
    struct Inputs inputs;
    struct Outputs outputs;
    struct Settings settings;
};

extern struct Program program;

// The state machine
void state_machine(struct Program *program);

// Read the inputs, triggered by read_inputs_flag 
void get_state(struct Program *program);

// Calculate state changes to outputs
void calculate_outputs(struct Program *program);

// Check whether any safety or limit conditions are breached and adjust program.outputs accordingly
void check_limits(struct Program *program);

// Set calculated output states to hardware
void update_outputs(struct Outputs *outputs);

// Disable state_machine
void disable_state_machine( void );
// Enable state_machine
void enable_state_machine( void );
