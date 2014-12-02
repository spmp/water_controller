#include "hardware.h"

void init_hardware(void ) {
    wd_reset();
    WD_SET(WD_OFF);
    init_clock();
    init_usart(38400, F_CPU);   //Initialise USART with speed 38400baude
    i2c_init(1000,F_CPU);     //Initilise I^2C with speed 100kHz
    init_io_ports();            //Initialise IO ports
    WD_SET(WD_RST,WDTO_4S);
}

void init_io_ports(void ) {
    //Actually this is an uninstructive way of doing it. Will use the macros given in 
    //  AVRFreaks
    
    //Set Internal LED pin to output
    DDRB |= (1 << ONBOARD_LED);
    
    //Set data direction for outputs
    DIRECTION_REG |= (1<<PUMP_PIN);
    DIRECTION_REG |= (1<<FILL_PIN);
    DIRECTION_REG |= (1<<HEAT1_PIN);
    DIRECTION_REG |= (1<<HEAT2_PIN);
    
//     DIRECTION_REG |= (1<<PUMP_PIN) & (1<<FILL_PIN) & (1<<HEAT1_PIN) & (1<<HEAT2_PIN);
    //Set outputs OFF
    OUTPUT_PORT &= ~((1<<PUMP_PIN) | (1<<FILL_PIN) | (1<<HEAT1_PIN) | (1<<HEAT2_PIN));

}


/* Get hardware states */
uint8_t pump_state(void) {
    return ((INPUT_PORT & (1<<PUMP_PIN)) == (1<<PUMP_PIN));
}
uint8_t fill_state(void) {
    return ((INPUT_PORT & (1<<FILL_PIN))==(1<<FILL_PIN));
//     return ((INPUT_PORT & (1<<FILL_PIN)) == (1<<FILL_PIN));
}
uint8_t heater_state(void) {
    if ((INPUT_PORT & (1<<HEAT1_PIN)) == (1<<HEAT1_PIN))
        return 1;
    else if  ((INPUT_PORT & (1<<HEAT2_PIN)) == (1<<HEAT2_PIN))
        return 1;
    else
        return 0;
    
//     return ( (INPUT_PORT & ((1<<HEAT1_PIN) | (1<<HEAT2_PIN))) == ((1<<HEAT1_PIN) | (1<<HEAT2_PIN)));
}

/* Set hardware states */
void pump_set(uint8_t state) {
    if (state == 1) {   // Set bit
        OUTPUT_PORT |= (1<< PUMP_PIN);
    }
    else { // Clear bit
        OUTPUT_PORT &=  ~(1<<PUMP_PIN);
    }
}

void fill_set(uint8_t state) {
    if (state == 1) {   // Set bit
        OUTPUT_PORT |= (1<< FILL_PIN);
    }
    else { // Clear bit
        OUTPUT_PORT &=  ~(1<<FILL_PIN);
    }
}

void heater_set(uint8_t state) {

    if (state == 1) {   // Set bits
        OUTPUT_PORT |= (1<< HEAT1_PIN);
        OUTPUT_PORT |= (1<< HEAT2_PIN);
    }
    else { // Clear bits
        OUTPUT_PORT &=  ~(1<<HEAT1_PIN);
        OUTPUT_PORT &=  ~(1<<HEAT2_PIN);
    }
}

/* Reset states and Watchdog ISR */