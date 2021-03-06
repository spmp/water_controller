#include "log.h"

uint8_t logenable = LOG_ENABLE; //logging enabled/disabled at startup

// Send logging data over serial
void log_to_serial(struct Program *program) {
    struct Inputs *inputs = &program->inputs;
    struct Outputs *outputs = &program->outputs;
    struct Settings *settings = &program->settings;
    
    //Temporarily disabled logging
    if ( logenable ){
        send_string_p(PSTR("l ")); // sending a log message
        send_uint32_half(timestamp & 0xFFFFFFFF);
        send_string_p(PSTR(" t "));
        send_uint16(inputs->temperature/100);
        send_string_p(PSTR(" l "));
        send_uint16(inputs->level);
        send_string_p(PSTR(" v "));
        send_uint16(inputs->volume);
        send_string_p(PSTR(" p "));
        send_char('0'+outputs->pump);
        send_string_p(PSTR(" P "));
        send_char('0'+settings->pump_enable);
        send_string_p(PSTR(" f "));
        send_char('0'+outputs->fill);
        send_string_p(PSTR(" F "));
        send_char('0'+settings->fill_enable);
        send_string_p(PSTR(" h "));
        send_char('0'+outputs->heater);
        send_string_p(PSTR(" H "));
        send_char('0'+settings->heater_enable);
        send_string_p(PSTR(" W "));
        send_uint16(daily_solar_gain);
        send_string_p(PSTR(" E "));
        send_uint8(error_state);
        send_string_p(PSTR(" T2H "));
        send_uint32(howlongtoheat);
#ifdef DEBUGGING
        send_string_p(PSTR(" HL "));
        send_uint32((settings->time_to_hot_1 + 86400L - timestamp)%(86400L));
#endif
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