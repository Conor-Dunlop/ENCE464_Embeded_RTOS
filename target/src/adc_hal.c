//*****************************************************************************
//
// adc_hal.c - Abstraction file to stand between ADC_read and the tivaware functions
//              and pass values between them
// Author:  Conor Dunlop
//
//*****************************************************************************

#include <stdint.h>

void adc_hal_register(uint32_t adc_id, void(*callback)(uint32_t))
{
    ; //do things
}

void adc_hal_start_conversion(uint32_t id)
{
    ; // do more things
}

void adc_hal_isr (void(*callback)(uint32_t))
{
    ; // the cake is a lie
}