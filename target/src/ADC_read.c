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
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "utils/ustdlib.h"
#include "adc_hal.h"
#include "circBufT.h"
#include "synch.h"
#include "FreeRTOS.h"
#include "task.h"


//*****************************************************************************
// Constants
//*****************************************************************************
#define ADC_BUF_SIZE 10
#define ADC_BUF_ID 3
//*****************************************************************************
// Global variables
//*****************************************************************************

static circBuf_t* ADC_inBuffer;		// Buffer of size BUF_SIZE integers (sample values)

void callback(uint32_t value) {

    writeCircBuf(ADC_inBuffer, value);
    
    #ifndef ADC_TEST
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Give semaphore from the ISR
    xSemaphoreGiveFromISR(xADCSemaphore, &xHigherPriorityTaskWoken);

    // Perform context switch if required
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);  
    #endif
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
// Initialisation functions for the ADC
//*****************************************************************************

void initADC (void)
{
    ADC_inBuffer = initCircBuf (ADC_BUF_SIZE);
    adc_hal_register (ADC_BUF_ID, callback);
}

uint32_t readADC() 
{    
      uint32_t sum = 0;
      uint16_t i = 0;
      for (i = 0; i < ADC_BUF_SIZE; i++)
          sum = sum + readCircBuf(ADC_inBuffer);

      return sum/ADC_BUF_SIZE;
}

