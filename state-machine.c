#include "state-machine.h"
/* The state machine does all the stuff, measuring input and output States, then decides what action to take based on this.
 *  The logging process picks up its information from the stored state.
 * 
 * The state machine has a notion of 'Programmes', these are a struct's of the Programe type.
 *  They are stored in an array aptly known as 'program'.
 *   Currently initial values are given at compile time, but the plan is to have these read from EEPROM at startup with the
 *   ability to save to EEPROM from the console.
 * 
 *  Solar hot water controller
 *     In this application we have a solar hot water controller whose
 *     functional input parameters are:
 *        water -Level-
 *        water -temperature-
 *       This could be extended to pump running from power feedback
 *     functional output parameters are
 *         -fill- valve control 
 *         -pump- enable/disable
 *         -heater- control
 * 
 * 
 * 
 * 
 * 
 */
#include "AVR-lib/usart.h"

uint32_t howlongtoheat = 0;
uint32_t filltimecount = 0;
uint16_t fill_init_level = 0;
uint8_t begin_state_machine_flag = 0;
uint8_t state_machine_running_flag = 0;
uint8_t state_machine_enable = 1;               //enabled by default
uint8_t error_state = 0;

/* Running program */
uint8_t state_machine_program = DEFAULT_PROGRAM; //Default start up program
uint8_t state_machine_config_program = DEFAULT_PROGRAM; //Default start up program

//BEGIN default configuration
/*
const struct Settings program_default PROGMEM = {
    .time_to_hot_1 = 64800,     // 1800
    .time_to_hot_2 = 21600,     // 0600
    .level_full = 350,
    .level_heater_min = 40,
    .level_min = 0,
    .level_fill = 350,
    .temperature_settemp = 65,
    .temperature_set_1 = 65,
    .temperature_set_2 = 60,
    .temperature_max = 80,
    .temperature_min = 10,
    .daily_heat_potential = 0,
    .midsun = 43200,            // 1200
    .pump_enable = 1,
    .fill_enable = 1,
    .heater_enable = 0
};
*/
//END default configuration

// struct Program program[NUM_PROGRAM] = {0};
// initialise and Populate `Programmes' with initial data:
struct Program program[NUM_PROGRAM] = {
    {{0},{0},{
        .time_to_hot_1 = 64800,     // Time (1) to have tank at set temp by
        .time_to_hot_2 = 21600,     // Time (2) to have tank at set temp by
        .morning_fill_time = 36000, // Time at which to check whether to fill the tank or not
        /* Level settings */
        .level_full = 286,        // The level of a full tank. Do not exceed 8) // possibly hard wired
        .level_heater_min = 40,  // The minimum safe level for running the heating elements
        .level_min = 5,         // The minimum allowable level of water in the tank
        .level_fill = 286,        // The level to fill the tank to\
        /* Temperature settings */
        .temperature_settemp = 70*TEMPERATUREMULTIPLIER,
        .temperature_set_1 = 70*TEMPERATUREMULTIPLIER,  // The temperature to which the system will automatically be heated
        .temperature_set_2 = 65*TEMPERATUREMULTIPLIER,
        .temperature_max = 90*TEMPERATUREMULTIPLIER,    // Maximum temperature to maintain
        .temperature_min = 5*TEMPERATUREMULTIPLIER,    // Minimum temperature to maintain
        /* Smarts */
        .daily_heat_potential = 0,      // How much energy to expect in a day
        .midsun = 43200,            // Time at which the sun was/is at its peak
        /* Output settings */
        .pump_enable = 1,
        .fill_enable = 1,
        .heater_enable = 0,
        .fill_max_time = FILL_MAX_TIME
        }
    }
//       64800,21600,350,40,5,350,65,65,60,80,10,0,43200,1,1,0}}/*,
//     {{0},{0},{64820,21600,350,41,0,350,66,65,60,80,10,0,43200,1,1,0}},
//     {{0},{0},{64830,21600,350,42,0,350,67,65,60,80,10,0,43200,1,1,0}}  */  
};

/* The State Machine for the system. "state-machine" requires a pointer to the 
 * struct program as we may desire to run different programs in the 
 * future, or switch program or whatever */
void state_machine(struct Program *program) {
    //make sure only one instance of the state machine is running, and is enabled
    if (state_machine_running_flag || !state_machine_enable) {
        return;
    }
    state_machine_running_flag = 1;     // The state machine is running
    
    //Read the inputs
    get_state(program);
    
    //Calculate the changes to the state of the system
    calculate_outputs(program);   
    
    //Check whether any safety or limit conditions are breached and adjust program.outputs accordingly
//    check_limits(program);  
    
    // Set the outputs TODO:BROKEN here and in hardware.c
   update_outputs(&program->outputs);  
    
    /* Disable state machine if not enabled */
    if (!state_machine_enable) {
        disable_state_machine();
    }
    state_machine_running_flag = 0;     //the state machine is finished
}

/* Get the state of the system, input values and output states */
void get_state(struct Program *program) {
    
    struct Inputs *inputs = &program->inputs;
    struct Outputs *outputs = &program->outputs;
    
    inputs->reading_inputs = 1;
    inputs->waiting_on_inputs = 0; // more if need be
    
    
    /* Application Specific: */
    //Get Water controller input states
#ifndef DEBUGGING
    inputs->temperature = temperature(); // do something useful like: read_temperature();
    inputs->level = level();
#endif
    inputs->volume = volume(inputs->level);
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
     *  Disable heating if heater is ... disabled 8)
     */
    
//BEGIN pump
    /* Do all the pump logic in one line! */
        outputs->pump = ( inputs->level >= settings->level_min)&&(settings->pump_enable);
//END pump
    
//BEGIN fill
    /* The Fill logic */
        // Fill is enabled, so we can do something
        if (settings->fill_enable) {
            // Turn on the filler if the level is low, fill now, or it is time
            if (
                inputs->level <= settings->level_min ||     // Low level
                inputs->fill_now ||                         // Fill now
                timestamp == settings->morning_fill_time    // Fill time
                //TODO: Or if analytics decides that we can fill and still
                // maintain temperature at TTH1
               ) 
            {
                // Enable filling
                outputs->fill = 1;
                //reset fill_now flag
                inputs->fill_now = 0;
            }
            // We are filling, better figure out when to turn off
            if (outputs->fill) {
                if (filltimecount == 0) fill_init_level = 0;
                filltimecount++;

                // Check filling, NOTE:Will fail if pump is stronger than filler
                // Check every FILL_CHECK_TIME seconds for increase level
                if (filltimecount%(FILL_CHECK_TIME *
                        (CLOCK_TICKS_PER_SECOND/MEDIUM_TIME_INTERVAL)) == 0
                ) {// Initialise fill_init_level if filltimecount is 0
                    send_string_p(PSTR("Prev Lev is: "));
                    send_uint16(fill_init_level);
                    send_string_p(PSTR(", fill time count is: "));
                    send_uint16(filltimecount);
                    send_newline();
                    // If level is not increasing disable filling and error
                    if (inputs->level <= fill_init_level) {
                        filltimecount = 0;
                        outputs->fill = 0;
                        settings->fill_enable = 0;
                        // Set the fill error
                        error_state |= (1<<5);
                    }
                    fill_init_level = inputs->level;
                }      
                // If we have been filling too long disable filling and error
                if (filltimecount >= (settings->fill_max_time)*    // Seconds
                        (CLOCK_TICKS_PER_SECOND/MEDIUM_TIME_INTERVAL)
                ) {
                    filltimecount = 0;
                    outputs->fill = 0;
                    settings->fill_enable = 0;
                    // Set the fill
                    error_state |= (1<<5);
                }
                
                // Full or overfull! Turn off
                if (
                    inputs->level >= settings->level_full || 
                    inputs->level >= settings->level_fill
                ) {
                    // Turn off filler
                    outputs->fill = 0;
                    // Reset fill counter
                    filltimecount = 0;
                }
            }
        // Filling is disabled, ensure the filler is off
        } else {
            outputs->fill = 0;          // Filling is disabled, turn off the filler
        }
//END fill
    
//BEGIN heater
    /* The heater logic */
    /* Calculate whether the heater should turn on, separately for each case
        * Should really use a function here that takes time, timetohot, and
        * settemp and returns... whether to turn on ... and changes settemp...
        * Question now is where does this function live? in state machine or in
        * temperateure?? lets do it in state machine. So only takes Tn, tn */

    /* Check the stop conditions */
    if (settings->heater_enable && ( inputs->level > settings->level_heater_min ) )       // Is heater enabled, and level high enough
    {
        /* Are we hot enough yet?  NOTE: This could be combined with the above statement, but kept separate for clarity*/
        if (inputs->temperature < settings->temperature_settemp)
        {
            /* Are we boing to boost? */
            if ( inputs->boost_now ) 
            {
            outputs->heater = 1;        // Set the heater on as we are boosting
            inputs->boost_now = 0;              // reset the boost flag
            }
            /* We are not boosting, but should we turn on? */
            else
            {
                /* Calculating the time to HOT, an Exposition
                *   What we have:
                *       Cp (water ) = 4.1855 J/(gK)     (SPECIFIC_HEAT_WATER/1000)
                *       Density water = 1000g/L
                *       Heating = 3000W                   (HEATER_SIZE)
                *       Volume = L
                *       deltaT = K*10000 (temperature measurements are in K x 10,000)
                *       W = J/s
                *  So we want to get Seconds out so need:
                *       s = (Cp(J/(gK)) x denisty(1000g/L) x Volume(L) x deltaT(K*10000) )/(W*10000)
                * In order to avoid floating point operations we want to ensure
                *  that the numberator never overflows then do integer devision
                *
                * With a 32bit unsinged integer
                * numberator_max = (4.186*1000*300*120) is all good,
                * but our maximum temperature and deltaT is x 10000 which is not
                *  so good.
                * Can it be broken down: (4186*300/3000) then x deltaT*10000 then
                *  divide by 10000
                * Lets take worst case again: deltaT is 120K, V is 300, W is 2400
                * With floats s = 62790
                * In groups of integer arithmatic:
                * (4186*300/2400) = 523 <2^32 x 1,200,000 = 6.27e8 , 2^32 / 10000
                * = 62760 < 2^32 Sweet and off by 60 seconds
                * 
                *  Now, the timing diagram looks like this:
                * 
                *         ^time    TTH1 Midnight TTH2
                * |-------|######--*----!--------*--------|
                *          Caclulated time to heat
                *  When the time to heat + the current time (modulo 24 hours) is greater than the current time we turn on the heaters!
                */
                int32_t deltaT = (settings->temperature_set_1 - inputs->temperature);
                howlongtoheat = ( SPECIFIC_HEAT_WATER * inputs->volume ) / HEATER_SIZE;
                howlongtoheat = howlongtoheat * (uint32_t)deltaT;
                howlongtoheat = howlongtoheat / TEMPERATUREMULTIPLIER;
                                
                uint32_t howlong = (settings->time_to_hot_1 + 86400L - timestamp)%(86400L);             //How long until time_to_hot_1 in seconds
                
                if ( howlongtoheat >= howlong) {
                    settings->temperature_settemp = settings->temperature_set_1;
                    outputs->heater = 1;
                }
            }      
        }
        /* Water is hot enough, set heater output off */
        else
        {
            outputs->heater = 0; 
            howlongtoheat = 0;
        }
    }
    /* Stop conditions not met, set heater output off */
    else
        outputs->heater = 0;
    
    
    
    
//         //How long until timetohot 1?
//         if ( !((inputs->temperature >= settings->temperature_set_1) | (outputs->heater))) {           //If heating already, or temperature greater than settemp do nothing!
//             uint32_t howlong = (settings->time_to_hot_1 + 24*60*60 - timestamp) % 24*60*60;             //How long until time_to_hot_1 in seconds
//             uint32_t howlongtoheat = (HEATER_SIZE) / 
//                 ( inputs->volume * SPECIFIC_HEAT_WATER * (settings->temperature_set_1 - inputs->temperature)) ;
//             if ( howlongtoheat >= howlong) {
//                 settings->temperature_settemp = settings->temperature_set_1;
//                 outputs->heater = 1;
//             }
//         }
//         //How long until timetohot 2?
//         if (!((inputs->temperature >= settings->temperature_set_2) | (outputs->heater))) {           //If heating already, or temperature greater than settemp do nothing!
//             uint32_t howlong = (settings->time_to_hot_2 + 24*60*60 - timestamp) % 24*60*60;             //How long until time_to_hot_2 in seconds
//             uint32_t howlongtoheat = (HEATER_SIZE) / 
//             (inputs->volume * SPECIFIC_HEAT_WATER * (settings->temperature_set_2 - inputs->temperature)) ;
//             if (howlongtoheat >= howlong) {
//                 settings->temperature_settemp = settings->temperature_set_2;
//                 outputs->heater = 1;
//             }
//         }
       
//         //Basic heating off control states
//         outputs->heater =  (settings->heater_enable    // stop heater if it is disabled
//         & (outputs->heater                             // the current state
//         & (inputs->temperature <= settings->temperature_settemp)))     // stop heater if temperature is above settemp 
//         | ((inputs->temperature <= settings->temperature_min)      // start heating if temperature falls below min
//         | inputs->boost_now);                        // start heating if boost_now flag is set
//         inputs->boost_now = 0  ;                      // clear boost flag
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

// Disable state_machine
void disable_state_machine(void) {
    /* Set hardware states OFF */
        state_machine_enable = 0;
        pump_set(0);
        fill_set(0);
        heater_set(0);
}

// Enable state_machine
void enable_state_machine(void) {
        state_machine_enable =1 ;
}
