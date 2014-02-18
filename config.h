/*******************************************************************************
 *                        Application Specific Configuration                   *
 *                                       for                                   *
 *                              Solar Hotwater Controller                      *
 *                                                                             *
 *                              Jasper Aorangi Mackenzie                       *
 *                                       2014                                  *
 ******************************************************************************/

/* This file contains definitions and settings related to the particular project
 */

//TODO: As much as possible have ifndef etc. in all .h files for defintions

/* State machine */
#define LOG_ENABLE = 0 //Enable logging at startup


/*General Hardware setttings */

/* USART */
#define BAUDE_RATE 57600 //USART Baude rate

/* I2C */
#define SCL_CLOCK  400000L //400kHz clock speed

/* Temperature */
#define TEMP_SENSOR1_ADDRESS 0x48 // 1001000, temporary for testing
// #define TEMP_SENSOR1_ADDRESS 0x4C // 1001100, I2C sensor addresses
#define TEMP_SENSOR2_ADDRESS 0x4E // 1001110
#define TEMP_SENSOR3_ADDRESS 0x4F // 1001111

/* Level */
#define PRESSURE_SENSOR_ADDRESS 0x48 //1001000, I2C ADC address

/* Outuput */
#define PUMP_PIN
#define FILL_PIN
#define HEATER1_PIN 
#define HEATER2_PIN



