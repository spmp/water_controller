#include "temperature.h"
#include "adc.h"
 
/*
 * In this implimentaion temperature is sampled using an
 * LM335 temperature sensor IC connected to an ADC channel.
 * 
 * The sensor is essentially a highly stable linear Zener diode. 
 *          Output (to ADC_IN)
 *            |
 * Vcc---R1------GND	Where R1 is calculated as:
 *            |			Resistor=(Vcc-3)*1000
 *          LM335-Adj.	In this application we use a 2k resistor
 *            |
 *           GND
 * 
 *  The LM335 gives 0.01V per °K.
 *  Calibrate to correct for slope error
 *  Vout_T= Vout_T0 * T / T_0
 *  Vout  = (Vref/1024) * ADC
 *  Temp  = Vout/10 in K = Vout/10 - 273.15 in °C.
 *  Vcal  = (T0/Vout_T0)  this soaks up the 10mv/°C
 *  T_cal = Vout*Vcal - 273.15 
 * 
 * With an 10bit ADC we get 0.488 degree resolution. Good enough.
 *
 * Now, how to do all this with 16bit integers and no floats!
 *  First step is make temperature=temperature*100 to give two
 *  dp accuracy - make this 1000 if this is needed....
 * 
 */


/* Calibrate the temperature sensor at known temperature in °C*/
 float calibrate_temperature(uint8_t calibrationtemp, uint8_t temppin){
    float calibrationparameter;
    calibrationparameter = ( (float)calibrationtemp -273.15) / (float)read_analog_pin(temppin);
    return calibrationparameter;
 }

/* Water temperature in floating point °C */
 float water_temperature_f(uint16_t adctemperature, float calibrationparameter){
    float watertemp;
    watertemp = calibrationparameter * (float)adctemperature - 273.15;
    return watertemp;
 }
 
/* Water temperature in signed integer °C, -128->128°C*/
 int8_t water_temperature(uint16_t adctemperature, float calibrationparameter){
    float watertemp;
    watertemp = calibrationparameter * (float)adctemperature - 273.15;
    return (int8_t)watertemp;
 }
 

