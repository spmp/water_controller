#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

#include "config.h"

#include "usart.h"
#include "clock.h"
#include "freq.h"
#include "adc.h"
#include "temperature.h"
#include "command.h"
#include "state-machine.h"
#include "i2c_safe.h"
#include "log.h"

uint8_t send_log = 0;

void once_per_second() {
    send_log = 1;
}

void medium_timestep() {
    begin_state_machine_flag = 1;
}


int main() {
    cli();
    set_sleep_mode(SLEEP_MODE_IDLE);
    init_clock();
    init_freq();
    init_usart();
    i2c_init();
    //AT30TSE758_init(0x48);
    
    DDRB |= (1 << DDB5);
    
    //Set timer callbacks
    clock_set_seconds_callback(&once_per_second);
    clock_set_medium_time_callback(&medium_timestep);
    
    //USART line handler
    usart_set_handle_char_string_from_serial(&handle_line);
    
    sei();

    struct Program program;

    for (;;) {
        sleep_mode(); // blocked until after an interrupt has fired
        
        //Separate out USART, State machine, and Logging
        while (num_in_serial_buffer()) { //check whether there is anything on the serial buffer, if there is, look at it
            handle_single_char_from_serial();
        }
        
        //State machine 
        if ( begin_state_machine_flag ) {
            begin_state_machine_flag = 0;
            state_machine(&program);
        }
        
        //Loggin
        if (send_log) {
            send_log = 0;
            log_to_serial(&program);
        }
    }

    return 0;
}

