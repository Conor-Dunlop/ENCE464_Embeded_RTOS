/*
 * ADC_read.h - Based on 'ADCdemo1.h'
 *
 *  Created on: 08/02/2018
 *      Authors: P.J. Bones	UCECE
 * 
 *  Last Modified: 22/08/2024
 *      Authors: Flynn Underwood, Brennan Drach, Conor Dunlop
 *
 */

#ifndef ADC_READ_H_
#define ADC_READ_H_

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
