#include "command.h"

uint8_t eloaded;
uint8_t wloaded;
uint8_t Wloaded;
uint8_t send_mcusr_flag = 0 ;

/* Fixed text strings */
const char string_LastLinePrefix[] PROGMEM = CRP;
const char string_LinePrefix[] PROGMEM = CRp;
const char string_LastLineSuffix[] PROGMEM = CRS;
const char string_mm[] PROGMEM = "mm"CRS"\r\n";
const char string_L[] PROGMEM = "L"CRS"\r\n";
const char string_s[] PROGMEM = "s"CRS"\r\n";
// Must manually edit
int16_t TEMPMULTIPLIER = TEMPERATUREMULTIPLIER;
// const char string_degc[] PROGMEM = "°Cx10"CRS"\r\n";
const char string_degc[] PROGMEM = "°C"CRS"\r\n";
const char string_kWh[] PROGMEM = "kWh"CRS"\r\n";


/**
 @brief send_string() wrapper for command response suffix
 */
void send_newline_crs(){
    send_string_p(PSTR(CRS"\r\n"));
}

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
        command_from_serial(line[0], argument_value, &program[state_machine_config_program]);
//         command_from_serial(line[0], argument_value, &program);
    }
}

/* The Giant mess that is the commands from serial */
void command_from_serial(char commandname, uint32_t commandvalue, struct Program *program) {
    
    
    struct Inputs *inputs = &program->inputs;
    struct Outputs *outputs = &program->outputs;
    struct Settings *settings = &program->settings;
    
    switch(commandname) {
        //Help!
        case 'h': //Disable logging
            send_string_p(PSTR(CRH"Help! Available commands.\r\n" \
CRH"            Commands are case sensetive letters followed by a number (commandvalue) with no space\r\n" \
CRH"            Default commandvalue is 0 if not given\r\n" \
CRH"            \t l: Level (mm)\r\n" \
CRH"            \t v: Volume (L)\r\n" \
CRH"            1 Enable/0 Disable:\r\n" \
CRH"            \t L: Logging\r\n" \
CRH"            \t S: State machine\r\n" \
CRH"            \t A: Analytics\r\n" \
CRH"            \t F: Fill\r\n" \
CRH"            \t H: Heat\r\n" \
CRH"            \t P: Pump\r\n" \
CRH"            0 get value/Other Set value:\r\n" \
CRH"            \t t: Time (s)\r\n" \
CRH"            \t a: Anaytics time interval (s)\r\n" \
CRH"            \t T: Temperature (°C)\r\n" \
CRH"            \t f: Fill now (or to level)\r\n" \
CRH"            \t b: Boost now (or to temp)\r\n" \
CRH"            \t Y: TimeToHot1\r\n" \
CRH"            \t y: temp TTH1\r\n" \
CRH"            \t U: TTH2\r\n" \
CRH"            \t u: temp TTH2\r\n" \
CRH"            \t M: Max level\r\n" \
CRH"            \t m: Min level\r\n" \
CRH"            \t J: Heater min level\r\n" \
CRH"            \t N: Fill level\r\n" \
CRH"            \t n: Max fill time\r\n" \
CRH"            \t G: Max temp\r\n" \
CRH"            \t g: Min temp\r\n" \
CRH"            \t s: Midsun\r\n" \
CRH"            \t d: DHP (kWh)\r\n" \
CRH"            \t z: Zero the level\r\n" \
CRH"            \t O: Progam to configure\r\n" \
CRH"            \t o: Running program\r\n" \
CRH"            \t r: reset error flag \r\n" \
CRH"            \t R: reset daily energy gain \r\n" \
CRH"            \t W: Watchdog reset status reg. "CRS"\r\n"));
            break;
            
        case 'L': //logging
            if (commandvalue != 1){
                disable_logging();
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Logging disabled, enable with 'L1'."));
                send_newline_crs();
            }
            else {
                enable_logging();
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Logging enabled, disable with 'L'."));
                send_newline_crs();
            }
            break;
            
        case 'S': //State machine
            if (commandvalue != 1){
                disable_state_machine();
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("State machine disabled, enable with 'S1'."));
                send_newline_crs();
            }
            else {
                enable_state_machine();
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("State machine enabled, disable with 'S'."));
                send_newline_crs();
            }
            break;
            
        case 'A': //Analytics
            if (commandvalue != 1){
                disable_analytics();
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Analytics disabled, enable with 'A1'."));
                send_newline_crs();
            }
            else {
                enable_analytics();
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Analytics enabled, disable with 'A[0]'."));
                send_newline_crs();
            }
            break;
            
        case 'F': //Filler
            if (commandvalue != 1){
                settings->fill_enable = 0;
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Filler disabled, enable with 'F1'."));
                send_newline_crs();
            }
            else {
                settings->fill_enable = 1;
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Filler enabled, disable with 'F0'."));
                send_newline_crs();
            }
            break;
            
        case 'H': //Heater
            if (commandvalue != 1){
                settings->heater_enable = 0;
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Heater disabled, enable with 'H1'."));
                send_newline_crs();
            }
            else {
                settings->heater_enable = 1;
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Heater enabled, disable with 'H0'."));
                send_newline_crs();
            }
            break;
            
        case 'P': //Pump
            if (commandvalue != 1){
                settings->pump_enable = 0;
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Pump disabled, enable with 'P1'."));
                send_newline_crs();
            }
            else {
                settings->pump_enable = 1;
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Pump enabled, disable with 'P0'."));
                send_newline_crs();
            }
            break;
            
        case 'f': //Fill the tank
            if (commandvalue == 0){
                inputs->fill_now = 1;
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Filling the tank to "));
                send_uint16(settings->level_fill);
                send_string_p(PSTR(" mm"CRS"\r\n"));
            }
            else {
                settings->level_fill = commandvalue;
                inputs->fill_now = 1;
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Filling the tank to "));
                send_uint16(commandvalue);
                send_string_p(PSTR(" mm"CRS"\r\n"));
                //TODO: Figure out how to temporarily set fill to the value.
            }
            break;
            /* WTF?
        case 'k':
            send_string_p(PSTR("Setting output fill (hw)\r\n"));
//             outputs->fill = 1;
            fill_set(commandvalue);
            break;
        case 'K':
            send_string_p(PSTR("Clearing output fill (hw)\r\n"));
//             outputs->fill = 0;
            OUTPUT_PORT &=  ~(1<<FILL_PIN);
            break;
            */
        case 'j':
            send_string_p( string_LastLinePrefix );
            send_string_p(PSTR("Heating the tank to "));
            heater_set(commandvalue);
            send_string_p(string_degc);
            break;
            
        case 'b': //Heat the tank
            if (commandvalue == 0){
                inputs->boost_now = 1;
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Heating the tank to "));
                send_uint16(settings->temperature_settemp/TEMPMULTIPLIER);
                send_string_p(string_degc);
            }
            else {
                settings->temperature_settemp = commandvalue*TEMPMULTIPLIER;
                inputs->boost_now = 1;
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Heating the tank to "));
                send_uint16(settings->temperature_settemp/TEMPMULTIPLIER);
                send_string_p(string_degc);
            }
            break;
            
        case 't': //Time
            if ( commandvalue == 0 ){
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("The time is: "));
                send_uint32_half(timestamp);
                send_newline_crs();
            }
            else {
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Setting the time to "));
                timestamp = commandvalue;
                send_uint32_half(timestamp);
                send_newline_crs();
            }
            break;      
        case 'T':  //Temperature
            if ( commandvalue == 0 ){
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("The set temperature is: "));
                send_uint16(settings->temperature_settemp/TEMPMULTIPLIER);
                send_string_p(string_degc);
            }
            else {
#ifndef DEBUGGING     
                settings->temperature_settemp = commandvalue*TEMPMULTIPLIER;
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Setting temperature setpoint to "));
                send_uint16(settings->temperature_settemp/TEMPMULTIPLIER);
                send_string_p(string_degc);
#else
                inputs->temperature = commandvalue*TEMPMULTIPLIER;
#endif
            }
            break;
            
        case 'l':  //Level
#ifndef DEBUGGING  
            send_string_p( string_LastLinePrefix );
            send_string_p(PSTR("The level is "));
            send_uint16(inputs->level);
            send_string_p( string_mm );
#else
            inputs->level = commandvalue;
#endif
            break;
            
        case 'a': // Anaytics time interval
            if ( commandvalue == 0 ){
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("The analytics time interval is: "));
                send_uint32_half(long_time_interval/CLOCK_TICKS_PER_SECOND);
                send_newline_crs();
            }
            else {
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Setting the analytics time interval to "));
                long_time_interval = commandvalue*CLOCK_TICKS_PER_SECOND;
                send_uint32_half(long_time_interval/CLOCK_TICKS_PER_SECOND);
                send_newline_crs();
            }
            break; 

        case 'v':  //Volume
            send_string_p( string_LastLinePrefix );
            send_string_p(PSTR("The volume is "));
            send_uint16(inputs->volume);
            send_string_p(string_L);
            break;
            
        //Time and temp to hot
        case 'Y': //Time to hot 1
            if ( commandvalue != 0 ){
                settings->time_to_hot_1 = commandvalue;
            }
            send_string_p( string_LastLinePrefix );
            send_string_p(PSTR("Setting time_to_hot_1 to "));
            send_uint16(settings->time_to_hot_1);
            send_string_p(string_s);
            break;
            
        case 'y': //time_to_hot_1 settemp
            if ( commandvalue == 0 ){
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Setpoint temperature 1 is: "));
                send_uint16(settings->temperature_set_1/TEMPMULTIPLIER);
                send_string_p(string_degc);
            }
            else {
                settings->temperature_set_1 = commandvalue*TEMPMULTIPLIER;
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Setting setpoint temperature 1 to "));
                send_uint16(settings->temperature_set_1/TEMPMULTIPLIER);
                send_string_p(string_degc);
            }
            break;
            
        case 'U': //Time to hot 2
            if ( commandvalue == 0 ){
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("The time_to_hot_2 is: "));
                send_uint16(settings->time_to_hot_2);
                send_string_p(string_s);
            }
            else {
                send_string_p( string_LastLinePrefix );
                settings->time_to_hot_2 = commandvalue;
                send_string_p(PSTR("Setting time_to_hot_2 to "));
                send_uint16(settings->time_to_hot_2);
                send_string_p(string_s);
            }
            break;
            
        case 'u': //time_to_hot_2 settemp
            if ( commandvalue == 0 ){
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Setpoint temperature 2 is: "));
                send_uint16(settings->temperature_set_2/TEMPMULTIPLIER);
                send_string_p(string_degc);
            }
            else {
                send_string_p( string_LastLinePrefix );
                settings->temperature_set_2 = commandvalue*TEMPMULTIPLIER;
                send_string_p(PSTR("Setting setpoint temperature 2 to "));
                send_uint16(settings->temperature_set_2/TEMPMULTIPLIER);
                send_string_p(string_degc);
            }
            break;
            
        case 'M': //Maximum level
            if ( commandvalue == 0 ){
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Current maximum level is: "));
                send_uint16(settings->level_full);
                send_string_p( string_mm );
            }
            else {
                send_string_p( string_LastLinePrefix );
                settings->level_full = commandvalue;
                send_string_p(PSTR("Setting maximum level to "));
                send_uint16(settings->level_full);
                send_string_p( string_mm );
            }
            break;
            
        case 'm': //Minimum level
            if ( commandvalue == 0 ){
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Current minimum level is: "));
                send_uint16(settings->level_min);
                send_string_p( string_mm );
            }
            else {
                settings->level_min = commandvalue;
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Setting minimum level to "));
                send_uint16(settings->level_min);
                send_string_p( string_mm );
            }
            break;
            
        case 'J': //Heater minimum level
            if ( commandvalue == 0 ){
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Current heater minimum level is: "));
                send_uint16(settings->level_heater_min);
                send_string_p( string_mm );
            }
            else {
                settings->level_heater_min = commandvalue;
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Setting heater minimum level to "));
                send_uint16(settings->level_heater_min);
                send_string_p( string_mm );
            }
            break;
            
        case 'N': //Current fill level
            if ( commandvalue == 0 ){
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Current fill level is: "));
                send_uint16(settings->level_fill);
                send_string_p( string_mm );
            }
            else {
                settings->level_fill = commandvalue;
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Setting fill level to "));
                send_uint16(settings->level_fill);
                send_string_p( string_mm );
            }
            break;
            
        case 'n': //Current maximum fill time in seconds
            if ( commandvalue == 0 ){
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Current maximum fill time is: "));
                send_uint16(settings->fill_max_time);
                send_string_p(string_s);
            }
            else {
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Setting max fill time to "));
                settings->fill_max_time = commandvalue;
                send_uint16(settings->fill_max_time);
                send_string_p(string_s);
            }
            break;
            
        case 'r': //Reset the error register
            send_string_p( string_LastLinePrefix );
            send_string_p(PSTR("Resetting error register"));
            error_state = 0;
            send_newline_crs();
            break;
            
        case 'R': //Reset the daily energy gain
            send_string_p( string_LastLinePrefix );
            send_string_p(PSTR("Resetting daily solar gain"));
            analytics_reset();
            send_newline_crs();
            break;
            
        case 'G': //Maximum temperature 
            if ( commandvalue == 0 ){
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Maximum temperature is: "));
                send_uint16(settings->temperature_max/TEMPMULTIPLIER);
                send_string_p(string_degc);
            }
            else {
                settings->temperature_max = commandvalue*TEMPMULTIPLIER;
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Setting maximum temperature to "));
                send_uint16(settings->temperature_max/TEMPMULTIPLIER);
                send_string_p(string_degc);
            }
            break;
            
        case 'g': //Minimum temperature 
            if ( commandvalue == 0 ){
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Minimum temperature is: "));
                send_uint16(settings->temperature_min/TEMPMULTIPLIER);
                send_string_p(string_degc);
            }
            else {
                settings->temperature_min = commandvalue*TEMPMULTIPLIER;
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Setting minimum temperature to "));
                send_uint16(settings->temperature_min/TEMPMULTIPLIER);
                send_string_p(string_degc);
            }
            break;
            
        case 's': //Midday sun
            if ( commandvalue == 0 ){
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Midday by the sun is at "));
                send_uint16(settings->midsun);
                send_string_p(string_s);
            }
            else {
                settings->midsun = commandvalue;
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Setting midday by the sun to "));
                send_uint16(settings->midsun);
                send_string_p(string_s);
            }
            break;
            
        case 'd': //Daily heating potential
            if ( commandvalue == 0 ){
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Daily heating potential is "));
                send_uint16(settings->daily_heat_potential);
                send_string_p(string_kWh);
            }
            else {
                settings->midsun = commandvalue;
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Setting daily heating potential to "));
                send_uint16(settings->daily_heat_potential);
                send_string_p(string_kWh);
            }
            break;
            
        case 'z': //Zero level
            if ( commandvalue == 0 ){
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Level zero is: "));
                send_uint32(level_sensor_zero);
                send_newline_crs();
            }
            else {
                level_sensor_zero = commandvalue;
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Level zero is: "));
                send_uint16(level_sensor_zero);
                send_newline_crs();
            }
            break;
//                 send_string_p(string_LastLinePrefix);
//                 send_string_p(PSTR("WTF!!\r\n"));
// //                 level();
// //                 level_zero();
//                 send_string_p(string_LastLinePrefix);
//                 send_string_p(PSTR("Zeroing the level.\r\n"));
//             break;
            
        case 'o': //Program to run
            if ( commandvalue == 0 || commandvalue > NUM_PROGRAM){
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("The running program is number "));
                send_char('1'+state_machine_program);
                send_newline_crs();
            }
            else {
                state_machine_program = commandvalue-1;
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Running program switched to "));
                send_char('1'+state_machine_program);
                send_newline_crs();
            }
            break;
            
        case 'O': //Program to configure
            if ( commandvalue == 0 || commandvalue > NUM_PROGRAM){
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("The program being configured is "));
                send_char('1'+state_machine_config_program);
                send_newline_crs();
            }
            else {
                state_machine_config_program = commandvalue-1;
                send_string_p( string_LastLinePrefix );
                send_string_p(PSTR("Now configuring program "));
                send_char('1'+state_machine_config_program);
                send_newline_crs();
            }
            break;
                
        case 'W':  //Watchdog reset
            send_mcusr_flag =1;
            break;
            
        ////////////////////////////////////////////////    
        //I2C related and debugging
        case 'I':
            send_string_p(PSTR("Scanning I2C bus in write mode: \r\n"));
            i2c_safe_write_scan_bus(0x00, 0x7F);
            send_string_p(PSTR("Scanning I2C bus in read mode: \r\n"));
            i2c_safe_read_scan_bus(0x00, 0x7F);
            send_newline();
            send_string_p(PSTR("completed\r\n"));
            break;
//         case 'R': //testing i2c_safe_read_sixteen
//             send_string_p(PSTR("I2C safe: Reading 16 bits from register 0x0, from address:"));
//             send_uint16(commandvalue);
//             send_newline();
//             send_uint16(i2c_safe_read_sixteen(commandvalue, 0x0) );
//             send_newline();
//             break;
        case 'X': //testing read_MCP3221
            send_string_p(PSTR("read_MCP3221 from pressure sensor. code is:"));
            send_uint16(read_MCP3221());
            send_newline();
            break;
        case 'q': //testing read_MCP3221
            send_string_p(PSTR("Level is:"));
            send_uint16(level());
            send_newline();
            break;
        case 'Q': //Go Crazy!
            send_string_p(PSTR("Going Crazy on processsed level!!!"));
            uint16_t poos;
            poos =10000;
            while (poos > 0){
                send_uint16(level());
                send_newline();
                wd_reset();
                _delay_ms(250);
                poos--;
            }
            break;
        case 'C': //Go Crazy!
            send_string_p(PSTR("Going Crazy on level!!!"));
            uint16_t poo;
            uint16_t fart;
            poo =10000;
            fart = read_MCP3221(); //give us 8x origina reading
            while (poo > 0){
                //Use averaging over 8 numbers
                //fart = read_MCP3221(); 
	        fart = ((fart*7 + read_MCP3221() )>>3);
                send_uint16(fart);
                send_newline();
	        wd_reset();
                _delay_ms(250);
                poo--;
            }
            break;
        case 'B': //Go Crazy!
            send_string_p(PSTR("Going Crazy on temperature!!!"));
            uint16_t wee;
            wee =10000;
            while (wee > 0){
                send_uint32(temperature());
                send_newline();
                poo--;
                wd_reset();
            }
            break;
        case 'V':
            send_string_p(PSTR("reading i2c_safe_sixteen with adress"));
            send_uint16(wloaded);
            send_string_p(PSTR(", and register "));
            send_uint16(commandvalue);
            send_newline();
            send_uint16(i2c_safe_read_sixteen(wloaded, commandvalue));
            send_newline();
            break;
            // data to write to register
        case 'w':
            wloaded = commandvalue ;
            send_string_p(PSTR("Loading data: "));
            send_uint16(wloaded);
            send_string_p(PSTR(" ready to write.\r\n"));
            break;
            // Toggle an output
        case 'c':
            send_string_p(PSTR("Toggling PortD pin"));
            send_uint16(commandvalue);
//             DDRD |= (1<<commandvalue); 
            send_newline();
            PORTD ^= (1<<commandvalue);
//             pump_set(commandvalue);
            break;
            
            // Toggle an output
        case 'x':
            send_string_p(PSTR("Turning on heaters"));
            send_uint16(commandvalue);
            //             DDRD |= (1<<commandvalue); 
            send_newline();
            outputs->heater = commandvalue ;
            //             pump_set(commandvalue);
            break;
    }
}
