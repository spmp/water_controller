#include <avr/io.h>
#include "usart.h"

/* usart enable send/recv mode: 8N1 @ 57600 */
void init_usart() {
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);
    UBRR0 = F_CPU / 16 / 57600 - 1;
}

/* write a single byte to the usart */
void send_char(uint8_t byte) {
    while (~UCSR0A & (1 << UDRE0));
    UDR0 = byte;
}

/* write a 16 bit integer to the usart in ascii decimal */
void send_uint16(uint16_t num) {
    uint16_t mult = 10000;
    for (uint8_t i=0; i<5; i++) {
        uint16_t face_value = num / mult;
        send_char('0' + face_value);
        num -= face_value * mult;
        mult /= 10;
    }
}
