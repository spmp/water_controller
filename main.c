#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "usart.h"
#include "clock.h"
#include "freq.h"
#include "adc.h"
#include "temperature.h"

float temperatureAcalibrationparam = (298.15*5.0)/(2.982*1023);

struct Inputs {
    //uint16_t distance;
    uint16_t pot_value;
};

struct Program {
    struct Inputs inputs;
};

struct Inputs read_inputs() {
    struct Inputs inputs;
    //inputs.distance = distance;
    //inputs.pot_value = read_analog_pin(3);
    inputs.pot_value = 12345;
    return inputs;
}

void write_state_to_usart(struct Program *program) {
    send_char('a');
    send_char(' ');
    send_uint16(timestamp & 0xFFFF);
    send_char(' ');
    send_char('D');
    send_char(' ');
    send_uint16(freqdiff());
    //send_char(' ');
    //send_char('P');
    //send_char(' ');
    //send_uint16((uint16_t)water_temperature(read_analog_pin(3), temperatureAcalibrationparam));
    //send_char(' ');
    //send_char('F');
    //send_char(' ');
    //send_uint16(freq());
  //  send_uint16(program->inputs.distance);
  //  send_char(' ');
  //  send_uint16(program->inputs.pot_value);
    send_char('\r');
    send_char('\n');
}

void loop(struct Program *program) {
    //program->inputs = read_inputs();
    //write_state_to_usart(program);
    handle_single_char_from_serial();

    //trigger_ultrasonic_measure();
    //_delay_ms(1000);
    _delay_ms(250);
}

void toggle_led() {
    PORTB ^= (1 << PORTB5);
}

int main() {
    cli();
    init_clock();
    init_freq();
    init_usart();

    DDRB |= (1 << DDB5);
    oncePerSecondCallback = &toggle_led;
    sei();

    struct Program program;
    for (;;) {
        loop(&program);
    }

    return 0;
}

