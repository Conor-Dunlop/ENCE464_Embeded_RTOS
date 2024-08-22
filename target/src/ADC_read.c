/*
 * ADC_read.c - Based on 'ADCdemo1.c'
 *
 *  Created on: 08/02/2018
 *      Authors: P.J. Bones	UCECE
 * 
 *  Last Modified: 22/08/2024
 *      Authors: Flynn Underwood, Brennan Drach, Conor Dunlop
 *
 *  Simple interrupt driven program which samples with AIN0
 *
 */

#include "adc_hal.h"
#include "circ_buf_t.h"
#include "FreeRTOS.h"
#include "synch.h"


//*****************************************************************************
// Constants
//*****************************************************************************
#define ADC_BUF_SIZE 10
#define ADC_BUF_ID 3

//*****************************************************************************
// Local variables
//*****************************************************************************

static circBuf_t* ADC_inBuffer;		// Buffer of size BUF_SIZE integers (sample values)

//*****************************************************************************
//
// Callback function - used for testing
//
//*****************************************************************************
void callback(uint32_t value) {

    writeCircBuf(ADC_inBuffer, value);
    
    #ifndef TESTING // Disable xADCSemaphore during testing
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Give semaphore from the ISR
    xSemaphoreGiveFromISR(xADCSemaphore, &xHigherPriorityTaskWoken);

    // Perform context switch if required
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);  
    #endif // TESTING
}

//*****************************************************************************
//
// The interrupt handler for the for SysTick interrupt.
//
//*****************************************************************************
void pollADC(void)
{
    //
    // Initiate a conversion
    adc_hal_start_conversion (ADC_BUF_ID);
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
//
// Initialisation functions for the ADC
//
//*****************************************************************************

void initADC (void)
{
    ADC_inBuffer = initCircBuf (ADC_BUF_SIZE);
    adc_hal_register (ADC_BUF_ID, callback);
}

uint32_t readADC(void) 
{    
      uint32_t sum = 0;
      uint16_t i = 0;
      for (i = 0; i < ADC_BUF_SIZE; i++)
          sum = sum + readCircBuf(ADC_inBuffer);

      return sum/ADC_BUF_SIZE;
}

