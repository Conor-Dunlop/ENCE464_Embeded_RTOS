#ifndef SWITCHES_H_
#define SWITCHES_H_

// *******************************************************
// Header file for switch reading module
// Daniel Rabbidge
//
// FitnessThur9-1
//
// *******************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

//*****************************************************************************
// Constants
//*****************************************************************************
enum SWNames {SW1 = 0, SW2, NUM_SW};
enum SWStates {SW_UP = 0, SW_DOWN, SW_NO_CHANGE};

// Switch peripheral
#define SW_PERIPH  SYSCTL_PERIPH_GPIOA

// SW1 (RIGHT)
#define SW1_PORT_BASE  GPIO_PORTA_BASE
#define SW1_PIN  GPIO_PIN_7
#define SW1_NORMAL  false
// SW2 (LEFT)
#define SW2_PORT_BASE  GPIO_PORTA_BASE
#define SW2_PIN  GPIO_PIN_6
#define SW2_NORMAL  false


#define NUM_SW_POLLS 3

// Debounce algorithm: A state machine is associated with the switch.
// A state change occurs only after NUM_SW_POLLS consecutive polls have
// read the pin in the opposite condition, before the state changes and
// a flag is set.  Set NUM_SW_POLLS according to the polling rate.

// *******************************************************
// initSwitch: Initialise the variables associated with the switch
// defined by the constants above.
void initSwitch (void);


// *******************************************************
// updateSwitch: Function designed to be called regularly. It polls the switch
// and updates variables associated with the switch if
// necessary.
void updateSwitch (void);


// *******************************************************
// Function that returns the state of the switch. Used to abstract GPIO function calls in other modules
bool isSwitchUp(uint8_t SWNames);


// *******************************************************
// checkSwitch: Function returns the new switch logical state if the switch
// logical state (SW_UP or SW_DOWN) has changed since the last call,
// otherwise returns SW_NO_CHANGE.
uint8_t checkSwitch (uint8_t butName);

#endif /*SWITCHES_H_*/
