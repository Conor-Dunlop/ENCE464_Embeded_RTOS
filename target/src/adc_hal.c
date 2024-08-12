//*****************************************************************************
//
// adc_hal.c - Abstraction file to stand between ADC_read and the tivaware functions
//              and pass values between them
// Author:  Conor Dunlop
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"
#include "driverlib/adc.h"
#include "circBufT.h"

#define ADC_BUF_SIZE 10


static circBuf_t* ADC_inBuffer;		// Buffer of size BUF_SIZE integers (sample values)

void adc_hal_isr (void)
{
    uint32_t ulValue;
	
	// Get the single sample from ADC0.  ADC_BASE is defined in
	// inc/hw_memmap.h
	ADCSequenceDataGet(ADC0_BASE, 3, &ulValue);
	//
	// Place it in the circular buffer (advancing write index)
	writeCircBuf (ADC_inBuffer, ulValue);
	//
	// Clean up, clearing the interrupt
	ADCIntClear(ADC0_BASE, 3);      
}

void adc_hal_register(uint32_t adc_id)
{
    if (adc_id < 4)
    {
        ADC_inBuffer = initCircBuf (ADC_BUF_SIZE);

        // The ADC0 peripheral must be enabled for configuration and use.
        SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
        
        // Enable sample sequence 3 with a processor signal trigger.  Sequence 3
        // will do a single sample when the processor sends a signal to start the
        // conversion.
        ADCSequenceConfigure(ADC0_BASE, adc_id, ADC_TRIGGER_PROCESSOR, 0);
    
        //
        // Configure step 0 on sequence 3.  Sample channel 0 (ADC_CTL_CH0) in
        // single-ended mode (default) and configure the interrupt flag
        // (ADC_CTL_IE) to be set when the sample is done.  Tell the ADC logic
        // that this is the last conversion on sequence 3 (ADC_CTL_END).  Sequence
        // 3 has only one programmable step.  Sequence 1 and 2 have 4 steps, and
        // sequence 0 has 8 programmable steps.  Since we are only doing a single
        // conversion using sequence 3 we will only configure step 0.  For more
        // on the ADC sequences and steps, refer to the LM3S1968 datasheet.
        ADCSequenceStepConfigure(ADC0_BASE, adc_id, 0, ADC_CTL_CH0 | ADC_CTL_IE |
                                ADC_CTL_END);    
                                
        //
        // Since sample sequence 3 is now configured, it must be enabled.
        ADCSequenceEnable(ADC0_BASE, adc_id);
    
        //
        // Register the interrupt handler
        ADCIntRegister (ADC0_BASE, adc_id, adc_hal_isr);
    
        //
        // Enable interrupts for ADC0 sequence 3 (clears any outstanding interrupts)
        ADCIntEnable(ADC0_BASE, adc_id);
    }
}

void adc_hal_start_conversion(uint32_t id)
{
    if (id < 4) 
    {
        ADCProcessorTrigger(ADC0_BASE, id);
    }
}

uint32_t readAdcBuf (void)
{
    readCircBuf (ADC_inBuffer);
}