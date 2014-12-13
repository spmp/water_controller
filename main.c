#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

#include "hardware.h"
#include "wd.h"

#include "state-machine.h"
#include "command.h"
#include "log.h"

// Watchdog and reset state.
// "You're not suppose to call get_mcusr() in main().
// "the attribute section(".init3") puts the code in before main() so it runs automatically before entering main().
// NOTE: The following code _MAY_ work without bootloader, but WILL NOT work with bootlaoder
/*
uint8_t mcusr_mirror __attribute__ ((section (".noinit")));
void get_mcusr(void) \
__attribute__((naked)) \
__attribute__((section(".init3")));
void get_mcusr(void)
{
mcusr_mirror = MCUSR;
MCUSR = 0;
wdt_disable();
}
*/
// NOTE: This code will work with a recent version of 'optiboot' as the bootloader:
uint8_t resetFlags __attribute__ ((section(".noinit")));
void resetFlagsInit(void) __attribute__ ((naked)) __attribute__ ((section (".init0")));
void resetFlagsInit(void)
{
  // save the reset flags passed from the bootloader
  __asm__ __volatile__ ("mov %0, r2\n" : "=r" (resetFlags) :);
}

uint8_t send_log = 0;

void once_per_second() {
    send_log = 1;
    PORTB ^= (1 << PORTB5); //Toggle LED
}

void medium_timestep() {
    begin_state_machine_flag = 1;
}


int main() {    
    cli();
    init_hardware();
    set_sleep_mode(SLEEP_MODE_IDLE);
    wd_reset();
    
    //TODO: Read settings from EEprom
    
    //Send initialisation message:
    // TODO: This will show how many resets... after the R
    send_string_p(PSTR("Rx Solar Hotwater Controller. V:2.5.1. Jasper Aorangi/Brendan Bycroft 2014. Have a nice day, and a great shower 8): "));
    send_uint16(resetFlags);
    send_string_p(PSTR(" x\r\n"));

    
    //AT30TSE758_init(0x48);
    
//     DDRB |= (1 << DDB5);
    
    //Set timer callbacks
    clock_set_seconds_callback(&once_per_second);
    clock_set_medium_time_callback(&medium_timestep);
    
    //USART line handler
    usart_set_handle_char_string_from_serial(&handle_line);
    
    //Enable Global interrupts
    sei();

    disable_state_machine();        // Debugging. Will do this if MCUSR is not 0
    disable_logging();              // Debugging

//     // Debugging
//     if(send_mcusr_flag){
//         send_string_p(PSTR("c Watchdog Status MCUSR:"));
//         send_uint16(resetFlags);
//         send_string_p(PSTR(" x"));
//     }
        
    for (;;) {
        sleep_mode(); // blocked until after an interrupt has fired
        
        //USART
        while (num_in_serial_buffer()) { //check whether there is anything on the serial buffer, if there is, look at it
            handle_single_char_from_serial();
        }
        
        //State machine 
        if ( begin_state_machine_flag ) {
            begin_state_machine_flag = 0;
            state_machine(&program[state_machine_program]);
//             state_machine(&program);
        }
        
        //Logging
        if (send_log) {
            send_log = 0;
            log_to_serial(&program[state_machine_program]);
//             log_to_serial(&program);
        }
        
        //Reset the watchdog
        wd_reset();
        
    }

    return 0;
}

