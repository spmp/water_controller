#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

#include "hardware.h"

#include "analytics.h"
#include "state-machine.h"
#include "command.h"
#include "log.h"

#define VERSION "2.7.3"

// Watchdog and reset state.
/*
 * First, we need a variable to hold the reset cause that can be written before
 * early sketch initialization (that might change r2), and won't be reset by the
 * various initialization code.
 * avr-gcc provides for this via the ".noinit" section.
 */
uint8_t resetFlags __attribute__ ((section(".noinit")));

/*
 * Next, we need to put some code to save reset cause from the bootload (in r2)
 * to the variable.  Again, avr-gcc provides special code sections for this.
 */
void resetFlagsInit(void) __attribute__ ((naked))
                          __attribute__ ((section (".init0")));
void resetFlagsInit(void)
{
  /*
   * save the reset flags passed from the bootloader
   * This is a "simple" matter of storing (STS) r2 in the special variable
   * that we have created.  We use assembler to access the right variable.
   */
  __asm__ __volatile__ ("sts %0, r2\n" : "=m" (resetFlags) :);
}

uint8_t send_log = 0;

void long_timestep() {
    analytics_begin_flag = 1;
}

void once_per_second() {
    send_log = 1;
    PORTB ^= (1 << PORTB5); //Toggle LED
    if (timestamp == 0) {
        analytics_reset();
    }
}

void medium_timestep() {
    begin_state_machine_flag = 1;
}

int main() {    
    cli();
    init_hardware(&program[state_machine_program]);
    set_sleep_mode(SLEEP_MODE_IDLE);
    wd_reset();
    
    //TODO: Read settings from EEprom
    
    //Send initialisation message:
    send_string_p(PSTR("Rx Solar Hotwater Controller. V:"VERSION".\r\nJasper Aorangi/Brendan Bycroft 2014. Have a nice day, and a great shower 8): "));
    send_uint16(resetFlags);
    send_string_p(PSTR(" x\r\n"));

    // Watchdog reset status
    /*
    * check for the usual bits.  Note that the symnbols defined in wdt.h are
    * bit numbers, so they have to be shifted before comparison.
    */
/*
    if (resetFlags & (1<<WDRF))
    {
        send_string_p(PSTR(" Watchdog"));
        resetFlags &= ~(1<<WDRF);
    }
    if (resetFlags & (1<<BORF))
    {
        send_string_p(PSTR(" Brownout"));
        resetFlags &= ~(1<<BORF);
    }
    if (resetFlags & (1<<EXTRF))
    {
        send_string_p(PSTR(" External"));
        resetFlags &= ~(1<<EXTRF);
    }
    if (resetFlags & (1<<PORF))
    {
        send_string_p(PSTR(" PowerOn"));
        resetFlags &= ~(1<<PORF);
    }
    if (resetFlags != 0x00)
    {
        // It should never enter here
        send_string_p(PSTR(" Unknown"));
    }
    send_newline();
*/    
    
    //Set timer callbacks
    clock_set_long_time_callback(&long_timestep);
    clock_set_seconds_callback(&once_per_second);
    clock_set_medium_time_callback(&medium_timestep);
    
    //USART line handler
    usart_set_handle_char_string_from_serial(&handle_line);
    
    //Enable Global interrupts
    sei();

    if (STATEMACHINEENABLEDONSTART) {
        enable_state_machine();
    } else {
        disable_state_machine();        // Debugging. Will do this if MCUSR is not 0
    }
    if (LOGGINGENABLEDONSTART) {
        enable_logging();
    } else {
        disable_logging();              // Debugging
    }
    if (ANALYTICSENABLEDONSTART) {
        enable_analytics();
    } else {
        disable_analytics();              // Debugging
    }
        
    for (;;) {
        sleep_mode(); // blocked until after an interrupt has fired
        
        //USART
        while (num_in_serial_buffer()) { //check whether there is anything on the serial buffer, if there is, look at it
            handle_single_char_from_serial();
        }
        
        //Analytics
        if (analytics_begin_flag) {
            analytics_begin_flag = 0;
            analytics_run(&program[state_machine_program]);
        }
        
        //State machine 
        if (begin_state_machine_flag) {
            begin_state_machine_flag = 0;
            state_machine(&program[state_machine_program]);
        }
        
        //Logging
        if (send_log) {
            send_log = 0;
            log_to_serial(&program[state_machine_program]);
        }
        
        //Reset the watchdog
        wd_reset();
        
    }

    return 0;
}

