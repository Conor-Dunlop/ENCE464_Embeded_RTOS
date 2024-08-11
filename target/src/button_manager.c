/*
 * Button_management.c
 *
 * Modifies the device's state according to the user's button and switch input
 *
 *  Created on: 31/03/2022
 *      Author: Daniel Rabbidge
 *
 *  FitnessThur9-1
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "inc/tm4c123gh6pm.h"
#include "buttons4.h"
#include "display_manager.h"
#include "button_manager.h"
#include "switches.h"
#include "synch.h"


//********************************************************
// Constants and static vars
//********************************************************
#define LONG_PRESS_CYCLES 20

static uint16_t longPressCount = 0;
static bool allowLongPress = true;


//********************************************************
// Init buttons and switch I/O handlers
//********************************************************
void btnInit(void)
{
    initButtons();
    initSwitch();
}


//********************************************************
// Run at a fixed rate, modifies the device's state depending on button presses
//********************************************************
void btnUpdateState(deviceStateInfo_t* deviceStateInfo, enum butNames button)
{
    displayMode_t currentDisplayMode = deviceStateInfo ->displayMode;

    // Changing screens
    switch (button) {
        case LEFT:
            if (checkButton(button) == PUSHED) {
                deviceStateInfo -> displayMode = (deviceStateInfo -> displayMode + 1) % DISPLAY_NUM_STATES;      //flicker when pressing button
            }
            break;
        case RIGHT: 
            if (checkButton(button) == PUSHED) {
                // Can't use mod, as enums behave like an unsigned int, so (0-1)%n != n-1
                if (deviceStateInfo -> displayMode > 0) {
                    deviceStateInfo -> displayMode--;
                } else {
                    deviceStateInfo -> displayMode = DISPLAY_NUM_STATES-1;
                }
            }
            break;
        default:
            break;
    }


    // Usage of UP and DOWN buttons
    if (deviceStateInfo -> debugMode) {
        // TEST MODE OPERATION
        switch (button) {
            case UP:
                if (checkButton(button) == PUSHED) {
                    deviceStateInfo -> stepsTaken = deviceStateInfo -> stepsTaken + DEBUG_STEP_INCREMENT;
                }
                break;
            case DOWN:
                if (checkButton(button) == PUSHED) {
                    if (deviceStateInfo -> stepsTaken >= DEBUG_STEP_DECREMENT) {
                        deviceStateInfo -> stepsTaken = deviceStateInfo -> stepsTaken - DEBUG_STEP_DECREMENT;
                    } else {
                        deviceStateInfo -> stepsTaken = 0;
                    }
                }
                break;
            default:
                break;
        }
    } else {
        // NORMAL OPERATION
        switch (button) {
            case UP: // Changing units
                if (checkButton(button) == PUSHED) {
                    if (deviceStateInfo -> displayUnits == UNITS_SI) {
                        deviceStateInfo -> displayUnits = UNITS_ALTERNATE;
                    } else {
                        deviceStateInfo -> displayUnits = UNITS_SI;
                    }
                }
                break;
            case DOWN: // Resetting steps and updating goal with long and short presses
                if ((isDown(button) == true) && (currentDisplayMode != DISPLAY_SET_GOAL) && (allowLongPress)) {
                    longPressCount++;
                    if (longPressCount >= LONG_PRESS_CYCLES) {
                        deviceStateInfo -> stepsTaken = 0;
                        flashMessage("Reset!");
                    }
                } else {
                    if ((currentDisplayMode == DISPLAY_SET_GOAL) && checkButton(button) == PUSHED) {
                        deviceStateInfo -> currentGoal = deviceStateInfo -> newGoal;
                        deviceStateInfo -> displayMode = DISPLAY_STEPS;

                        allowLongPress = false; // Hacky solution: Protection against double-registering as a short press then a long press
                    }
                    longPressCount = 0;
                }

                if (checkButton(button) == RELEASED) {
                    allowLongPress = true;
                }
                break;
            default:
                break;
        }
    }
}

//********************************************************
// Run at a fixed rate, modifies the device's state depending on switch states
//********************************************************
void swUpdateState(deviceStateInfo_t* deviceStateInfo, enum SWNames switches)
{
    if (xQueueReceive(switch_q, &switches, 0) == pdTRUE) {
        switch (switches) {
            case SW1: // Enable/Disable test mode
                if (isSwitchUp(switches) == UP) {
                    deviceStateInfo -> debugMode = true;
                } else if (isSwitchUp(switches) == DOWN) {
                    deviceStateInfo -> debugMode = false;
                }
            case SW2: // Additional functionality TBD
                break;
            default:
                break;
        }
    }
}


