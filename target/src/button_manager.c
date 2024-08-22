/*
 * button_manager.c
 *
 *  Created on: 31/03/2022
 *      Authors: Daniel Rabbidge
 * 
 *  Last Modified: 22/08/2024
 *      Authors: Flynn Underwood, Brennan Drach, Conor Dunlop
 *
 *  Modifies the device's state according to the user's button and switch input
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include "buttons4.h"
#include "button_manager.h"
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
    // Update button state
    updateButtons();

    displayMode_t currentDisplayMode = deviceStateInfo ->displayMode;

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
    } else if (deviceStateInfo -> setParamsMode) {
        // SET PARAMETERS MODE OPERATION
        switch (button) {
            case UP: // Changing units
                if (checkButton(button) == PUSHED) {
                    // Increment m/steps
                    if (deviceStateInfo -> mPerStep < MAX_M_STEP) {
                        deviceStateInfo -> mPerStep += M_PER_STEP_INCREMENT;
                    }
                }
                break;
            case DOWN: // Resetting steps and updating goal with long and short presses
                if (checkButton(button) == PUSHED) {
                    // Decrement m/steps
                    if (deviceStateInfo -> mPerStep > M_PER_STEP_INCREMENT) {
                        deviceStateInfo -> mPerStep -= M_PER_STEP_INCREMENT;
                    }
                }
                break;
            case LEFT:
                if (checkButton(button) == PUSHED) {
                    // Decrement running speed
                    if (deviceStateInfo -> runningSpeed > RUN_SPEED_INCREMENT) {
                        deviceStateInfo -> runningSpeed -= RUN_SPEED_INCREMENT;
                    }
                }
                break;
            case RIGHT: 
                if (checkButton(button) == PUSHED) {
                    // Increment running speed
                    if (deviceStateInfo -> runningSpeed < MAX_RUN_SPEED) {
                        deviceStateInfo -> runningSpeed += RUN_SPEED_INCREMENT;
                    }
                }
                break;
            default:
                break;
        }
    } else if (!(deviceStateInfo -> debugMode) && !(deviceStateInfo -> setParamsMode)) {
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
                        xSemaphoreGive(xResetSemaphore);
                    }
                } else {
                    if ((currentDisplayMode == DISPLAY_SET_GOAL) && checkButton(button) == PUSHED) {
                        deviceStateInfo -> currentGoal = deviceStateInfo -> newGoal;
                        deviceStateInfo -> displayMode = DISPLAY_STEPS;

                        allowLongPress = false;
                    }
                    longPressCount = 0;
                }

                if (checkButton(button) == RELEASED) {
                    allowLongPress = true;
                }
                break;
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
    }
}

//********************************************************
// Run at a fixed rate, modifies the device's state depending on switch states
//********************************************************
void swUpdateState(deviceStateInfo_t* deviceStateInfo, enum SWNames switches)
{
    updateSwitch();

    switch (switches) {
        case SW1: // Enable/Disable debug mode
            if (isSwitchUp(switches) == SW_UP) {
                deviceStateInfo -> debugMode = true;
            } else if (isSwitchUp(switches) == SW_DOWN) {
                deviceStateInfo -> debugMode = false;
            }
        case SW2: // Enable/Disable set parameters mode
            static bool DISP_LOCK = false;
            if (isSwitchUp(switches) == SW_UP) {
                deviceStateInfo -> setParamsMode = true;
                deviceStateInfo -> displayMode = DISPLAY_TRAITS_EDITOR;
                DISP_LOCK = false;
            } else if (isSwitchUp(switches) == SW_DOWN) {
                deviceStateInfo -> setParamsMode = false;
                if (!DISP_LOCK) {
                    deviceStateInfo -> displayMode = DISPLAY_STEPS;
                    DISP_LOCK = true;
                }
            }
        default:
            break;
    }
}