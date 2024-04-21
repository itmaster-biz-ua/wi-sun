/*
 * adc_app.h
 *
 *  Created on: 20 квіт. 2024 р.
 *      Author: itmaster
 */

#ifndef APPLICATION_ADC_APP_H_
#define APPLICATION_ADC_APP_H_

// ADC init tasks
void init_adc_tasks(void);
uint16_t get_adc_corrected_value(uint16_t adcValue);

#endif /* APPLICATION_ADC_APP_H_ */
