#pragma once
#include <avr/io.h>
#include <avr/interrupt.h>
#include "usart.h"
#include "clock.h"
#include "log.h"

extern uint8_t begin_state_machine_flag;
extern uint8_t state_machine_running_flag;

struct Inputs {
    /* This pair of values is for handling of inputs that take a long time to
    obtain.

    For example, it might be 50ms between when you ask for a level measurement
    and you actually get the value. $reading_inputs is set to 1 when you've
    just asked for various measurements to be taken. $waiting_on_inputs is set
    to the number of measurements that take some time to calculate.

    When a measurement is finally calculated, it's value is stored in the
    Inputs struct, and $waiting_on_inputs is decremented. Thus when
    $reading_inputs = 1 and $waiting_on_inputs = 0, we've got all of
    our inputs and are ready to process them further.
    */
    uint8_t reading_inputs;
    uint8_t waiting_on_inputs;

    uint16_t temperature; // degrees celcius (C)
    uint16_t level; // height (mm)
    uint16_t volume; // Litres
};

struct Outputs {
    /* TODO: fill me in */
};

struct Settings {
    /* TODO: fill me in */
};

struct Program {
    struct Inputs inputs;
    struct Outputs outputs;
    struct Settings settings;
};

// The state machine
void state_machine(struct Program *program);

// Read the inputs, triggered by read_inputs_flag 
void read_inputs(struct Inputs *inputs);

// Calculate state changes to outputs
void calculate_outputs(struct Program *program);

// Check whether any safety or limit conditions are breached and adjust program.outputs accordingly
void check_limits(struct Program *program);

// Set calculated output states to hardware
void update_outputs(struct Outputs *outputs);
