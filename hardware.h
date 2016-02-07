#pragma once
#include <avr/io.h>

/* AVR specifics */
#include "AVR-lib/ds18x20.h"
#include "AVR-lib/i2c_safe.h"
#include "AVR-lib/usart.h"
#include "clock.h"
#include "state-machine.h"
#include "wd.h"

/* Inputs */
#include "temperature.h"//Temperature reading
#include "level.h"      //Level and volume reading

/* Header to bring together all hardware related functions and routines, set
 * things and stuff like that.
 */

/* Data buss settings   */
// USART
#define USARTBAUD       38400
// I2C
#define I2CSPEED        1000
// OW bit bang
#define OWPIN           PB0
#define OWIN            PINB
#define OWOUT           PORTB
#define OWDDR           DDRB
#define OW_USE_INTERNAL_PULLUP     0

// ds18x20 defines
#define DS18X20TEMPRES  DS18B20_12_BIT 

// IO defines:
#define ONBOARD_LED     PB5

#define OUTPUT_PORT     PORTD
#define INPUT_PORT      PIND
#define DIRECTION_REG   DDRD
#define PUMP_PIN        PD3
#define FILL_PIN        PD7
#define HEAT1_PIN       PD5
#define HEAT2_PIN       PD6

/* Heater size */
#define HEATER_SIZE     2400L
#define SPECIFIC_HEAT_WATER 4186L

/* State machine defaults */
#define STATEMACHINEENABLEDONSTART 1
#define LOGGINGENABLEDONSTART 1
#define HEATERENABLEDONSTART 1

/* Initialise all hardware relevant to the project, usart, i2c, clock, external
 * sensors, Everyting!
 * One call to rule them all */
void init_hardware(struct Program *program);

/* Initialise I/O ports on the AVR */
void init_io_ports(void);

/* Hardware states */
uint8_t pump_state(void);
uint8_t fill_state(void);
uint8_t heater_state(void);

/* Set hardware states */
void pump_set(uint8_t state);
void fill_set(uint8_t state);
void heater_set(uint8_t state);
