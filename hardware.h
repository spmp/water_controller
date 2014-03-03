#pragma once
#include <avr/io.h>

/* Header to bring together all hardware related functions and routines, set
 * things and stuff like that.
 */

/* Hardware specific defines: */
#define ONBOARD_LED     PB5

#define OUTPUT_PORT     PORTD
#define INPUT_PORT      PIND
#define DIRECTION_REG   DDRD
#define PUMP_PIN        PD3
#define FILL_PIN        PD7
#define HEAT1_PIN       PD4
#define HEAT2_PIN       PD5

/* Heater size */
#define HEATER_SIZE     3000L
#define SPECIFIC_HEAT_WATER 4186L

/* AVR specifics */
#include "i2c_safe.h"
#include "usart.h"
#include "clock.h"

/* Inputs */
#include "temperature.h"        //Temperature reading
#include "level.h"              //Level and volume reading

/* Initialise all hardware relevant to the project, usart, i2c, clock, external
 * sensors, Everyting!
 * One call to rule them all */
void init_hardware(void);

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
