#pragma once
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>

#define BUF_SIZE 20
#define MAX_LINE_LEN 20


/* Resources: 	USART 
 * 				Rx0, Tx1 pins on Arduino
 */

void init_usart();

/* write a byte as given to the usart */
void send_char(uint8_t byte);

/* write a 16 bit integer to the usart in ascii decimal */
void send_uint16(uint16_t num);

/* write a char string to the usart */
void send_char_string(char s[]);

/* Serial buffer size */
int num_in_serial_buffer();

/* Build line buffer from serial buffer, Check serial buffer for newline
 * and run line handler if so.*/
void handle_single_char_from_serial();

/* Parse the recieved line into command name and value */
void handle_line(char* line);

/* Take action on command name and command value */
void command_from_serial(char commandname, uint32_t commandvalue);

