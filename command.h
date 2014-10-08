/*Application specific commands to handle USART input strings */
#pragma once
# include <avr/pgmspace.h>
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include "usart.h"
#include "temperature.h"
#include "i2c_safe.h"
#include "log.h"
#include "clock.h"
#include "level.h"
#include "state-machine.h"
#include "hardware.h"

extern uint8_t send_mcusr_flag;

/* Parse the recieved line into command name and value */
void handle_line(const char* line);

/* Take action on command name and command value */
void command_from_serial(char commandname, uint32_t commandvalue, struct Program *program);


