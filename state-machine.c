#include "state-machine.h"

uint8_t read_inputs_flag = 0;

/* This is the STATE MACHINE
 * It will resmble a PLC in that it will do the following:
 *  Read inputs into input state buffer
 *  Take action based on input state buffer, writing changes to temporary buffer (ie HW state)
 *  Check limit/emergency conditions (modify HW state as need be)
 *  Set outputs
 * 
 * This is not a good flow for critical real time control which this is not.
 * 
 * This must be as hardware agnostic as possible, shilst being application
 * specific.
 *  For example if we are measuring light levels as fara as the state
 * machine is concerned we do not care how you get the measurement, just
 * that we can.
 */ 
 
 /* Solar hot water controller
  *     In this application we have a solar hot water controller whose
  *     functional input parameters are:
  *        water -Level-
  *        water -temperature-
  *       This could be extended to pump running from power feedback
  *     functional output parameters are
  *         -fill- valve control 
  *         -pump- enable/disable
  *         -heater- control
  */

struct Program;

/* The State Machine for the system. "state-machine" requires a pointer to the 
 * struct program as we may desire to run different programs in the 
 * future, or switch program or whatever */
void state_machine(struct Program *program) {
    while (num_in_serial_buffer()) { //check whether there is anything on the serial buffer, if there is, look at it
        handle_single_char_from_serial();
    }

    if (read_inputs_flag) { //if its time to read inputs, do it!
        read_inputs_flag = 0;

        read_inputs(&program->inputs);
        PORTB ^= (1 << PORTB5);
    }

    /* this will be true when all long-running measurements are complete */
    if (program->inputs.reading_inputs && !program->inputs.waiting_on_inputs) {
        program->inputs.reading_inputs = 0;

        calculate_outputs(program); // Calculate the changes to the state of the system
        
        check_limits(program); //Check whether any safety or limit conditions are breached and adjust program.outputs accordingly
        
        update_outputs(&program->outputs); // Set the outputs 

        log_to_serial(program); //send a log message
    }
}

// Read the inputs, triggered by read_inputs_flag 
void read_inputs(struct Inputs *inputs) {
    inputs->reading_inputs = 1;
    inputs->waiting_on_inputs = 0; // more if need be

    inputs->temperature = 5; // do something useful like: read_temperature();

    /* example of an input that takes a while to calc:

        get_level_takes_long_time(&inputs->level, &inputs->waiting_on_inputs);

    When this function is called, it will do some triggering and
    initialization. Some time in the future, an interrupt will be fired that
    contains the measurement. That interrupt will then do:

        *level = measurement;
        *waiting_on_inputs -= 1;

    Where $level and $waiting_on_inputs are the first and second parameters
    in that function respectively.
    */
}

// Calculate state changes to outputs
void calculate_outputs(struct Program *program) {
    /* TODO: do stuff here that reads from inputs,outputs,settings, and writes
       to outputs. */
}

// Check whether any safety or limit conditions are breached and adjust program.outputs accordingly
void check_limits(struct Program *program) {
    /* TODO: do stuff here that reads from inputs,outputs,settings, and writes
       to outputs. */
}

// Set calculated output states to hardware
void update_outputs(struct Outputs *outputs) {
    /* TODO: this is the reverse of read_inputs, so should contain things like

    set_heater(outputs->heater);

    */
}
