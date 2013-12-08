#pragma once
#include <avr/io.h>

/* Resources: 	USART 
 * 				Rx0, Tx1 pins on Arduino
 */

void init_usart();

/* write a byte as given to the usart */
void send_char(uint8_t byte);

/* write a 16 bit integer to the usart in ascii decimal */
void send_uint16(uint16_t num);

