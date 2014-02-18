
#include "log.h"

uint8_t logenable = LOG_ENABLE; //logging enabled/disabled at startup

// Send logging data over serial
void log_to_serial(struct Program *program) {
    struct Inputs *inputs = &program->inputs;
    struct Outputs *outputs = &program->outputs;
    
    //Temporarily disabled logging
    if ( logenable ){
        send_string("l "); // sending a log message
        send_uint16(timestamp & 0xFFFF);
        send_string(" t ");
        send_uint16(inputs->temperature);
        send_newline();
    }
}

// enable logging
void enable_logging(void){
    logenable = 1;
}

// disable logging
void disable_logging(void){
    logenable = 0;
}