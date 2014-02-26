#include "state-machine.h"

uint8_t begin_state_machine_flag = 0;
uint8_t state_machine_running_flag = 0;

uint8_t filling_flag = 0;

// TODO: Brendan: A function that points/redirects program ... then does it make sense to 
//     have struct imput to functions if struct program is always defined?
struct Program program = {0};


/* This is the STATE MACHINE
 * It will resemble a PLC in that it will do the following:
 *  Get the state: Read inputs and outputs into their respective state buffers
 *  Take action based on state buffers, changing the values of the state buffers
 *  Check limit/emergency conditions (modify HW state as need be)
 *  Set state from the output state buffer
 * 
 * This is not a good flow for critical real time control.
 * 
 * This must be as hardware agnostic as possible, whilst being application
 * specific.
 *  For example if we are measuring light levels as far as the state
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

/* The State Machine for the system. "state-machine" requires a pointer to the 
 * struct program as we may desire to run different programs in the 
 * future, or switch program or whatever */
void state_machine(struct Program *program) {
    //:BRENDAN Is this nescesarry? I think there is no way we could run it twice unles we had multicore etc.
    if (state_machine_running_flag ) { //make sure only one instance of the state machine is running
        return;
    }
    state_machine_running_flag = 1;     // The state machine is running
    
    get_state(program);      //Read the inputs
    
    calculate_outputs(program);         //Calculate the changes to the state of the system
    
    check_limits(program);              //Check whether any safety or limit conditions are breached and adjust program.outputs accordingly
    
    update_outputs(&program->outputs);  // Set the outputs 
    
/*
    if (read_inputs_flag) { //if its time to read inputs, do it!
        read_inputs_flag = 0;

        read_inputs(&program->inputs);
        PORTB ^= (1 << PORTB5);
    }

    // this will be true when all long-running measurements are complete 
    if (program->inputs.reading_inputs && !program->inputs.waiting_on_inputs) {
        program->inputs.reading_inputs = 0;

        calculate_outputs(program); // Calculate the changes to the state of the system
        
        check_limits(program); //Check whether any safety or limit conditions are breached and adjust program.outputs accordingly
        
        update_outputs(&program->outputs); // Set the outputs 
        
    }
*/
    state_machine_running_flag = 0;     //the state machine is finished
}

// Get the state of the system, input values and output states (from hardware!)
void get_state(struct Program *program) {
    
    struct Inputs *inputs = &program->inputs;
    struct Outputs *outputs = &program->outputs;
    
    inputs->reading_inputs = 1;
    inputs->waiting_on_inputs = 0; // more if need be
    
    
    /* Application Specific: */
    //Get Water controller input states
    inputs->temperature = temperature(); // do something useful like: read_temperature();
    inputs->level = level();
    inputs->volume = volume();
    //Get Water controller output states
    outputs->pump = pump_state();
    outputs->fill = fill_state();
    outputs->heater = heater_state();    

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
    
    struct Inputs *inputs = &program->inputs;
    struct Outputs *outputs = &program->outputs;
    struct Settings *settings = &program->settings;
    
    /*Application specific:
     * Water controller logic:
     * BASIC STAGE 1
     * Pump
     *  Pump on when level > levelmin, off otherwise
     *          TODO: Current sensor or pressure sensor - detect error:
     *                          pump running, no pressure change, no level change
     *  Pump off if disabled
     * Fill
     *  Enable filling if:
     *          Fillflag is set
     *          level <= levelmin
     *  Stop filliling if outputs->fill and level >= filllevel
     *  Disable fill if disabled
     * Heater
     *  Turn on heater if boostflag is set or 
     *          timetohot <= difference between now and the time to be hot (based on volume and heating potential)
     *  Turn heater off if outputs->heater(1 or 2) and temperateure >= settemp(1 or 2)
     *  Disable heating if heating is ... disabled 8)
     */
    
    //BEGIN pump
    /* Do all the pump logic in one line! */
        outputs->pump = ( inputs->level >= settings->level_min )&(settings->pump_enable);
    //END pump
    
    //BEGIN fill
    /* The Fill logic */
        outputs->fill = ( settings->fill_enable                         // stop filling if fill is disabled
                        & ( outputs->fill                               // the current state
                        & (inputs->level <= settings->level_full) )     // stop filling if level is above level_full
                        & (inputs->level <= settings->level_fill) )     // stop filling if level is above level_fill 
                        | ( (inputs->level <= settings->level_min)      // start filling if level falls below level_min
                        | settings->fill_now );                         // start filling if fill_now flag
        settings->fill_now = 0  ;                                       //reset fill_now flag
    //END fill
    
    //BEGIN heater
    /* The heater logic */
        /*Calculate whether the heater should turn on, separately for each case
         * Should really use a function here that takes time, timetohot, and
         * settemp and returns... whether to turn on ... and changes settemp...
         * Question now is where does this function live? in state machine or in
         * temperateure?? lets do it in state machine. So only takes Tn, tn */
        //How long until timetohot 1?
        if ( !((inputs->temperature >= settings->temperature_set_1) | (outputs->heating)) ) {           //If heating already, or temperature greater than settemp do nothing!
            uint32_t howlong = (settings->time_to_hot_1 + 24*60*60 - timestamp) % 24*60*60;             //How long until time_to_hot_1 in seconds
            uint32_t howlongtoheat = (HEATER_SIZE) / 
                ( inputs->volume * SPECIFIC_HEAT_WATER * (settings->temperature_set_1 - inputs->temperature) ) ;
            if ( howlongtoheat >= howlong ) {
                settings->temperature_settemp = settings->temperature_set_1;
                outputs->heater = 1;
            }
        }
        //How long until timetohot 2?
        if ( !((inputs->temperature >= settings->temperature_set_2) | (outputs->heating)) ) {           //If heating already, or temperature greater than settemp do nothing!
            uint32_t howlong = (settings->time_to_hot_2 + 24*60*60 - timestamp) % 24*60*60;             //How long until time_to_hot_2 in seconds
            uint32_t howlongtoheat = (HEATER_SIZE) / 
            ( inputs->volume * SPECIFIC_HEAT_WATER * (settings->temperature_set_2 - inputs->temperature) ) ;
            if ( howlongtoheat >= howlong ) {
                settings->temperature_settemp = settings->temperature_set_2;
                outputs->heater = 1;
            }
        }
       
        //Basic heating off control states
        outputs->heater =  ( settings->heater_enable    // stop heater if it is disabled
        & ( outputs->heater                             // the current state
        & (inputs->temperature <= settings->temperature_settemp) ) )     // stop heater if temperature is above settemp 
        | ( (inputs->temperature <= settings->temperature_min)      // start heating if temperature falls below min
        | settings->boost_now );                        // start heating if boost_now flag is set
        settings->boost_now = 0  ;                      // clear boost flag
    //END heater
}

// Check whether any safety or limit conditions are breached and adjust program.outputs accordingly
void check_limits(struct Program *program) {
    /* TODO: do stuff here that reads from inputs,outputs,settings, and writes
       to outputs. */
}

// Set calculated output states to hardware
void update_outputs(struct Outputs *outputs) {
    /* Set hardware states from output buffer */
    pump_set(outputs->pump);
    fill_set(outputs->fill);
    heater_set(outputs->heater);
}
