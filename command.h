/*Application specific commands to handle USART input strings */
#pragma once
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include "AVR-lib/i2c_safe.h"
#include "AVR-lib/usart.h"
#include "clock.h"
#include "hardware.h"
#include "level.h"
#include "log.h"
#include "state-machine.h"
#include "temperature.h"
#include "wd.h"

#define CRP "c "         //Response prefix, last line
#define CRp "c "         //Response prefix, n-1 lines
#define CRS " x"         //Response suffix (followed by /r/n)

extern uint8_t send_mcusr_flag;

/* Parse the recieved line into command name and value */
void handle_line(const char* line);

/* Take action on command name and command value */
void command_from_serial(char commandname, uint32_t commandvalue, struct Program *program);


