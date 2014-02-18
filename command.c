#include "command.h"
#include "temperature.h"
#include "i2c_safe.h"
#include "log.h"
#include "clock.h"


uint8_t eloaded;
uint8_t wloaded;
uint8_t Wloaded;

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
        //Help!
        case 'h': //Disable logging
            send_string("Help! Available commands."); send_newline();
            send_char('\t'); send_string("l: disable logging"); send_newline();
            send_char('\t'); send_string("L: enable logging"); send_newline();
            send_char('\t'); send_string("b: boost! heat the tank to the set hotpoint immediatly"); send_newline();
            send_char('\t'); send_string("f: Fill the water tank"); send_newline();
            send_char('\t'); send_string("F: Fill the tank to specified litres"); send_newline();
            send_char('\t'); send_string("t: current time (no parameter), or set time parameter in hhmmss"); send_newline();
            send_char('\t'); send_string("p: disable the pump"); send_newline();
            send_char('\t'); send_string("P: enable the pump"); send_newline();
            send_newline();
            break;
        //logging
        case 'l': //Disable logging
            disable_logging();
            send_string("Logging disabled, enable with 'L'.");
            send_newline();
            break;
        case 'L': //Enable logging
            enable_logging();
            send_string("Logging enabled, disable with 'l'.");
            send_newline();
            break;
            
        //State machine
        case 'b': //boost - no commandvalue
            send_string("Boosting!");
            send_newline();
//             boost(1);
            break;
        case 'f': //fill the tank
            send_string("Filling!");
            send_newline();
//             fill(FULL);
            break;
        case 'F': //fill the tank to value in Litres or mm
            send_string("Filling to ");
            send_uint16(commandvalue);
            send_string(" litres.");
            send_newline();
//             fill(commandvalue);
            break;
        case 't': //set the time
            if ( commandvalue == 0 ){
                send_string("The time is: ");
                send_uint16(timestamp);
                send_newline();
            }
            else {
                send_string("Setting the time to ");
                send_uint16(commandvalue);
                send_newline();
                timestamp = commandvalue;
            }
            break;
        case 'p': //disable pump
            send_string("Disabling the pump, enable with P.");
            send_newline();
//             pump(0);
            break;
        case 'P': //enable pump
            send_string("Enabling the pump, disable with p. ");
            send_newline();
//             pump(1);
            break;
            
        //Temperature sensors
        case 'A': //Intitialise AT30TSE758
            send_string("Initilaising AT30TSE758 sensor 1...");
            send_newline();
            if ( !init_AT30TSE758(TEMP_SENSOR1_ADDRESS) ){
                send_string("Success.");
            }
            else {
                send_string("Failed!");
            }
            send_newline();
            send_string("Initilaising AT30TSE758 sensor 2...");
            send_newline();
            if ( !init_AT30TSE758(TEMP_SENSOR2_ADDRESS) ){
                send_string("Success.");
            }
            else {
                send_string("Failed!");
            }
            send_newline();
            send_string("Initilaising AT30TSE758 sensor 3...");
            send_newline();
            if ( !init_AT30TSE758(TEMP_SENSOR3_ADDRESS) ){
                send_string("Success.");
            }
            else {
                send_string("Failed!");
            }
            send_newline();
            break;
        case 'T': //Display temperature
            send_string("Temperature is: ");
            send_uint16(temperature());
            send_newline();
            break;
        case 'S':
            send_string("Scanning I2C bus in read mode: ");
            send_newline();
            i2c_safe_write_scan_bus(0x00, 0x7F);
            send_string("Scanning I2C bus in read mode: ");
            send_newline();
            i2c_safe_read_scan_bus(0x00, 0x7F);
            send_newline();
            send_string("completed");
            send_newline();
            break;
        case 'R': //testing i2c_safe_read_sixteen
            send_string("I2C safe: Reading 16 bits from register 0x0, from address:");
            send_uint16(commandvalue);
            send_newline();
            send_uint16(i2c_safe_read_sixteen(commandvalue, 0x0) );
            send_newline();
            break;
        case 'Z': //testing read_AT30TSE758
            send_string("read_AT30TSE758 , from address:");
            send_uint16(commandvalue);
            send_newline();
            send_uint16(read_AT30TSE758(commandvalue));
            send_newline();
            break;

    }
}
