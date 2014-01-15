#include "command.h"
/* TODO: move the functions below to outside usart.c (e.g. commands.c)
   Make the handle_line() above a function-pointer which you
   can set at the start of the program (see clock.h for an example)
*/

/* Parse null terminated string with expected format:
 * <single letter command><command value>
 * ie: t130400 to set time to 13:04:00
 *  Strip and check command
 *  Check that command value is a number. (strtol() )
 */
void handle_line(const char* line) {
    char *endptr = 0;

    //Check whether rest of line is a number and if so proceed to logic
    uint32_t argument_value = strtoul(line+1, &endptr, 10);
    if (*endptr == '\0') {
        command_from_serial(line[0], argument_value);
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
