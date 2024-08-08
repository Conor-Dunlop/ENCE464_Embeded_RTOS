//*****************************************************************************
//
// ADCdemo1.c - Simple interrupt driven program which samples with AIN0
//
// Author:  P.J. Bones	UCECE
// Last modified:	8.2.2018
//
//*****************************************************************************
// Based on the 'convert' series from 2016
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "adc_hal.h"


//*****************************************************************************
// Constants
//*****************************************************************************
#define ADC_BUF_SIZE 10
#define ADC_BUF_ID 3
//*****************************************************************************
// Global variables
//*****************************************************************************


//*****************************************************************************
//
// The interrupt handler for the for SysTick interrupt.
//
//*****************************************************************************
void pollADC(void)
{
    adc_hal_start_conversion (ADC_BUF_ID);
    //
    // Initiate a conversion
    //
    
//    g_ulSampCnt++;
}

//*****************************************************************************
//
// The handler for the ADC conversion complete interrupt.
// Writes to the circular buffer.
//
//*****************************************************************************
void ADCIntHandler(void)
{
    adc_hal_isr ();                    
}

//*****************************************************************************
// Initialisation functions for the ADC
//*****************************************************************************

void initADC (void)
{
    //
    adc_hal_register (ADC_BUF_ID, void(*callback)(uint32_t));
}

uint32_t readADC() {
    
      uint32_t sum = 0;
      uint16_t i = 0;
      for (i = 0; i < ADC_BUF_SIZE; i++)
          sum = sum + readCircBuf (ADC_inBuffer);

      return sum/ADC_BUF_SIZE;
}

