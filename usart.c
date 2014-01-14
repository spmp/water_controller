#include <avr/io.h>
#include "usart.h"

char incoming_buffer[BUF_SIZE];
int truncating_data = 0; // hopefully this doesn't become 1!
int serial_buffer_count = 0;
int in_start = 0;

char line_buffer[MAX_LINE_LEN+1];
int num_char = 0;

/* usart enable send/recv mode: 8N1 @ 57600 */
void init_usart() {
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
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

/* write a char string to the usart */
void send_char_string(char s[]){
    int j=0; 
    while (s[j] != 0x00)
    {
        send_char(s[j++]);
    } 
}

/* Handle USART reading. So much more _fun! */
ISR (USART_RX_vect) {
    incoming_buffer[(in_start + serial_buffer_count) % BUF_SIZE] = UDR0;
    if (serial_buffer_count >= BUF_SIZE) {
        truncating_data = 1; // b-b-b-add!
    } else {
        serial_buffer_count++;
    }
    //UDR0 = 1;
}

/* blocks until chars have been received, return the last recieved byte */
char serial_read() {
    if (serial_buffer_count == 0)
        return '\0';
    char result = incoming_buffer[in_start];
    serial_buffer_count--;
    in_start = (in_start + 1) % BUF_SIZE;
    return result;
}

/* Serial buffer size */
int num_in_serial_buffer() {
    return serial_buffer_count;
}

/* Build line buffer from serial buffer, Check serial buffer for newline
 * and run line handler if so.*/
void handle_single_char_from_serial() {
    char next_char = serial_read();
    switch(next_char) {
        case '\r': // This is what the terminal sends as newline
            line_buffer[num_char] = '\0'; //terminate the line_buffer
            if (num_char > 0) { // Does the line contain any information
                handle_line(line_buffer);
            }
            num_char = 0;
            line_buffer[num_char] = '\0'; //clear the first and second characters
            line_buffer[num_char+1] = '\0'; //command and argument
            break;
        case '\n': // now we've got a full line, so we can process it further
            line_buffer[num_char] = '\0'; //terminate the line_buffer
            if (num_char > 0) { // Does the line contain any information
                handle_line(line_buffer);
            }
            num_char = 0;
            line_buffer[num_char] = '\0'; //clear the first and second characters
            line_buffer[num_char+1] = '\0'; //command and argument
            break;
        default: // save characters to line buffer
            if ((num_char < MAX_LINE_LEN)&&(next_char!='\0')) //make sure that we dont overflow line buffer and that there is something to put in it.
                line_buffer[num_char++] = next_char;
            break;
        }
}

/* Parse null terminated string with expected format:
 * <single letter command><command value>
 * ie: t130400 to set time to 13:04:00
 *  Strip and check command
 *  Check that command value is a number. (strtol() )
 */
void handle_line(char* line) {
    char * endptr = 0;
    char argument_string[MAX_LINE_LEN] = {};
    
    //Separate argument and value
    char argument = line[0];
    strcpy(argument_string, line+1);
    
    //Check whether the argument is a number and if so proceed to logic
    uint32_t argument_value = strtoul(line+1, &endptr, 0);
    if(*endptr==0) {
        command_from_serial(argument, argument_value);
    }
 }

/* The Giant mess that is the commands from serial */
void command_from_serial(char commandname, uint32_t commandvalue) {
    switch(commandname) {
        case 'b': //boost - no commandvalue
            send_char('B');
            send_char('\r');
            send_char('\n');
            break;
        case 'f': //fill the tank
            send_char('F');
            send_char('\r');
            send_char('\n');
            break;
        case 'F': //fill the tank to value in Litres or mm
            send_char('f');
            send_char(' ');
            send_uint16((uint16_t)commandvalue);
            send_char('\r');
            send_char('\n');
            break;
        case 't': //set the time
            send_char('t');
            send_char(' ');
            send_uint16((uint16_t)commandvalue);
            send_char('\r');
            send_char('\n');
            //timestamp = commandvalue;
            break;
        case 'p': //disable pump
            send_char('p');
            send_char('\r');
            send_char('\n');
            break;
        case 'P': //enable pump
            send_char('P');
            send_char('\r');
            send_char('\n');
            break;
    }
}
