/*Application specific commands to handle USART input strings */
#pragma once
#include <avr/io.h>
#include <stdlib.h>
#include "usart.h"

/* Parse the recieved line into command name and value */
void handle_line(const char* line);

/* Take action on command name and command value */
void command_from_serial(char commandname, uint32_t commandvalue);


