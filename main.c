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
#include "i2cmaster.h"

float temperatureAcalibrationparam = (298.15*5.0)/(2.982*1023);

void once_per_second() {
    read_inputs_flag = 1;
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
    clock_set_seconds_callback(&once_per_second);
    usart_set_handle_char_string_from_serial(&handle_line);
    sei();

    struct Program program;

    for (;;) {
        sleep_mode(); // blocked until after an interrupt has fired
        state_machine(&program);
    }

    return 0;
}

