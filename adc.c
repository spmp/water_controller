#include <avr/io.h>


/*adc_pins 0:7 are externally externally on Arduino and can use various Vref
 * Whereas adc_pin 8 is the internal temperature sensor and must use the 
 * internal 1.1Vref
 */
 
uint16_t read_analog_pin(uint8_t adc_pin) {
	if (adc_pin == 8 ){
		/* int 1.1V as reference voltage */
		ADMUX = (1 << REFS0) | (1 << REFS1) | (adc_pin & 0xF);
	} else {
		/* AVcc as reference voltage */
		ADMUX = (1 << REFS0) |(adc_pin & 0xF);
	}		
    /* start measure with prescaler 128 & wait until complete */
    ADCSRA = (1 << ADEN) | (1 << ADSC) | 0x7;
    while ((ADCSRA & (1 << ADSC)) != 0);
    return ADC;
}
