#ifndef ADC_READ_H_
#define ADC_READ_H_

#include <stdint.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

//*****************************************************************************
//
// The handler for the ADC conversion complete interrupt.
// Writes to the circular buffer.
//
//*****************************************************************************
void ADCIntHandler(void);

void initADC (void);
void pollADC(void);         // Called periodically at high frequency
uint32_t readADC(void);     // Call to read the averaged readings from the buffer

#endif //ADC_READ_H_
