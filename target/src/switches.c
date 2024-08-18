/*
 * switches.c - Based on the structure of buttons4.c by P.J Bones
 *
 *  Created on: 21/04/2022
 *      Authors: Daniel Rabbidge
 * 
 *  Last Modified: 22/08/2024
 *      Authors: Flynn Underwood
 *
 *  Module to read values from the Orbit Board toggle switches
 *
 */

#include "switches.h"


// *******************************************************
// Globals to module
// *******************************************************
static bool sw_state[NUM_SW];	// Corresponds to the electrical state
static uint8_t sw_count[NUM_SW];
static bool sw_flag[NUM_SW];
static bool sw_normal[NUM_SW];   // Corresponds to the electrical state

// *******************************************************
// initSwitch: Initialise the variables associated with SW1
void initSwitch (void)
{
    SysCtlPeripheralEnable (SW_PERIPH);

	// RIGHT Switch
    GPIOPinTypeGPIOInput (SW1_PORT_BASE, SW1_PIN);
    GPIOPadConfigSet (SW1_PORT_BASE, SW1_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);

    sw_normal[SW1] = SW1_NORMAL;
    sw_state[SW1] = SW1_NORMAL;
    sw_count[SW1] = 0;
    sw_flag[SW1] = false;

    // LEFT Switch
    GPIOPinTypeGPIOInput (SW2_PORT_BASE, SW2_PIN);
    GPIOPadConfigSet (SW2_PORT_BASE, SW2_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
    
    sw_normal[SW2] = SW2_NORMAL;
    sw_state[SW2] = SW2_NORMAL;
    sw_count[SW2] = 0;
    sw_flag[SW2] = false;
}


// *******************************************************
// updateSwitch: Function designed to be called regularly. It polls the switch and changes its state with debouncing
// Debounce algorithm: A state machine is associated with each button.
// A state change occurs only after NUM_SW_POLLS consecutive polls have
// read the pin in the opposite condition, before the state changes and
// a flag is set.  Set NUM_SW_POLLS according to the polling rate.
void updateSwitch (void)
{
    bool sw_value[NUM_SW];
	int i;
	
	// Read the pins; true means HIGH, false means LOW
	sw_value[SW2] = (GPIOPinRead (SW2_PORT_BASE, SW2_PIN) == SW2_PIN);
	sw_value[SW1] = (GPIOPinRead (SW1_PORT_BASE, SW1_PIN) == SW1_PIN);
	// Iterate through the switches, updating switch variables as required
	for (i = 0; i < NUM_SW; i++)
	{
        if (sw_value[i] != sw_state[i])
        {
        	sw_count[i]++;
        	if (sw_count[i] >= NUM_SW_POLLS)
        	{
        		sw_state[i] = sw_value[i];
        		sw_flag[i] = true;	   // Reset by call to checkSwitch()
        		sw_count[i] = 0;
        	}
        }
        else
        	sw_count[i] = 0;
	}
}


// *******************************************************
// checkSwitch: Function returns the new switch logical state if the switch
// logical state (SW_UP or SW_DOWN) has changed since the last call,
// otherwise returns SW_NO_CHANGE.
uint8_t
checkSwitch (uint8_t butName)
{
	if (sw_flag[butName])
	{
		sw_flag[butName] = false;
		if (sw_state[butName] == sw_normal[butName])
			return SW_UP;
		else
			return SW_DOWN;
	}
	return SW_NO_CHANGE;
}


// A function that returns the switch state in order to abstract GPIO functions in other modules
bool isSwitchUp(uint8_t SWNames) {
    return sw_state[SWNames];
}
