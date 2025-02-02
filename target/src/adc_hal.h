/*
 * adc_hal.h
 *
 *  Last Modified: 22/08/2024
 *      Authors: Conor Dunlop
 *
 *  The handler for the ADC hardware abstraction layer.
 *  Takes calls form ADC_read then calls tivaware to pass values between.
 *
 */

#ifndef adc_hal_H_
#define adc_hal_H_

#include <stdint.h>

enum ADC_ID{
    ADC_ID_1 = 1
};

void adc_hal_register (uint32_t adc_id, void(*callback)(uint32_t));

void adc_hal_start_conversion (uint32_t id);

void adc_hal_isr (void);

#endif //adc_hal_H_
