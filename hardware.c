#include "hardware.h"

void init_hardware(struct Program *program) {
    struct Settings *settings = &program->settings;
    
    wd_reset();
    WD_SET(WD_OFF);
    // Initialise clock
    long_time_interval = LONG_TIME_INTERVAL;
    init_clock();
    // Initialise USART with speed USARTBAUDbaud
    init_usart(USARTBAUD, F_CPU);
    // Initilise I^2C with speed I2CSPEED
    i2c_init(I2CSPEED, F_CPU);
    // Initialise DS18X20 devices
    send_string_p(PSTR("DS18X20 initialising: "));
    if (ds18x20_init(DS18X20TEMPRES) == 0) {
        send_string_p(PSTR("Success, "));
        send_string_p(PSTR("The number of sensors is: "));
        send_uint8(ds18x20nSensors);
        send_newline();
        // enable the heater
        if (HEATERENABLEDONSTART) {
            settings->heater_enable = 1;
        }
    } else {
        send_string_p(PSTR("Failure"));
    }
    send_newline();
    // Initialise IO ports
    init_io_ports();
    // Set watchdog timer
    WD_SET(WD_RST,WDTO_4S);
}

void init_io_ports(void ) {
    
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
