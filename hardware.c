#include "hardware.h"

void init_hardware(void ) {
    init_clock();
    init_usart(38400, F_CPU);
    i2c_init(400000,F_CPU);
}

void init_io_ports(void ) {
    //Actually this is an uninstructive way of doing it. Will use the macros given in 
    //  AVRFreaks
    
    //Set Internal LED pin to output
    DDRB |= (1 << ONBOARD_LED);
    
    //Set data direction for outputs
    DIRECTION_REG |= (1<<PUMP_PIN) & (1<<FILL_PIN) & (1<<HEAT1_PIN) & (1<<HEAT2_PIN);
    //Set outputs OFF
    OUTPUT_PORT &= ~(1<<PUMP_PIN) & ~(1<<FILL_PIN) & ~(1<<HEAT1_PIN) & ~(1<<HEAT2_PIN);

}


/* Hardware states */
uint8_t pump_state(void) {
    return ( INPUT_PORT & (1<<PUMP_PIN) );
}
uint8_t fill_state(void) {
    return ( INPUT_PORT & (1<<FILL_PIN) );
}
uint8_t heater_state(void) {
    return ( (INPUT_PORT & (1<<HEAT1_PIN)) |  (INPUT_PORT & (1<<HEAT2_PIN)) );
}

/* Set hardware states */
void pump_set(uint8_t state) {
    OUTPUT_PORT &=  0xFF & (state<<PUMP_PIN);
}
void fill_set(uint8_t state) {
    OUTPUT_PORT &=  0xFF & (state<<FILL_PIN);
}
void heater_set(uint8_t state) {
    OUTPUT_PORT &=  0xFF & (state<<HEAT1_PIN) & (state<<HEAT2_PIN) ;
}
