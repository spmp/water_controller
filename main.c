#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

#include "usart.h"
#include "clock.h"
#include "freq.h"
#include "adc.h"
#include "temperature.h"

float temperatureAcalibrationparam = (298.15*5.0)/(2.982*1023);

struct Inputs {
    /* This pair of values is for handling of inputs that take a long time to
    obtain.

    For example, it might be 50ms between when you ask for a level measurement
    and you actually get the value. $reading_inputs is set to 1 when you've
    just asked for various measurements to be taken. $waiting_on_inputs is set
    to the number of measurements that take some time to calculate.

    When a measurement is finally calculated, it's value is stored in the
    Inputs struct, and $waiting_on_inputs is decremented. Thus when
    $reading_inputs = 1 and $waiting_on_inputs = 0, we've got all of
    our inputs and are ready to process them further.
    */
    uint8_t reading_inputs;
    uint8_t waiting_on_inputs;

    uint16_t temperature; // <unit> TODO: specify units here
    uint16_t level; // <unit>
};

struct Outputs {
    /* TODO: fill me in */
};

struct Settings {
    /* TODO: fill me in */
};

struct Program {
    struct Inputs inputs;
    struct Outputs outputs;
    struct Settings settings;
};

void read_inputs(struct Inputs *inputs) {
    inputs->reading_inputs = 1;
    inputs->waiting_on_inputs = 0; // more if need be

    inputs->temperature = 5; // do something useful like: read_temperature();

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

void calculate_outputs(struct Program *program) {
    /* TODO: do stuff here that reads from inputs,outputs,settings, and writes
       to outputs. */
}

void update_outputs(struct Outputs *outputs) {
    /* TODO: this is the reverse of read_inputs, so should contain things like

    set_heater(outputs->heater);

    */
}

void log_to_serial(struct Program *program) {
    struct Inputs *inputs = &program->inputs;
    struct Outputs *outputs = &program->outputs;

    send_string("l "); // sending a log message
    send_uint16(timestamp & 0xFFFF);
    send_string(" t ");
    send_uint16(inputs->temperature);
    send_newline();
}

uint8_t read_inputs_flag = 0;

void once_per_second() {
    read_inputs_flag = 1;
}

void loop(struct Program *program) {
    while (num_in_serial_buffer()) {
        handle_single_char_from_serial();
    }

    if (read_inputs_flag) {
        read_inputs_flag = 0;

        read_inputs(&program->inputs);
        PORTB ^= (1 << PORTB5);
    }

    /* this will be true when all long-running measurements are complete */
    if (program->inputs.reading_inputs && !program->inputs.waiting_on_inputs) {
        program->inputs.reading_inputs = 0;

        calculate_outputs(program);
        update_outputs(&program->outputs);

        log_to_serial(program);
    }
}

int main() {
    cli();
    set_sleep_mode(SLEEP_MODE_IDLE);
    init_clock();
    init_freq();
    init_usart();
    DDRB |= (1 << DDB5);
    clock_set_seconds_callback(&once_per_second);
    sei();

    struct Program program;

    for (;;) {
        sleep_mode(); // blocked until after an interrupt has fired
        loop(&program);
    }

    return 0;
}

