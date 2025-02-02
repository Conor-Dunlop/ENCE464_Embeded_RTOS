/*
 * display_manager.h
 *
 *  Created on: 23/03/2022
 *      Authors: Matthew Suter
 * 
 *  Last Modified: 22/08/2024
 *      Authors: Flynn Underwood, Brennan Drach, Conor Dunlop
 *
 *  Handles drawing to the device's screen, given its current state
 *
 */

#ifndef DISPLAY_MANAGER_H_
#define DISPLAY_MANAGER_H_

#include <stdlib.h>
#include "utils/ustdlib.h"
#include "OrbitOLED/OrbitOLEDInterface.h"

#include "speed_tracker.h"
#include "step_counter_main.h"


typedef enum {
    ALIGN_LEFT = 0,
    ALIGN_CENTRE,
    ALIGN_RIGHT,
} textAlignment_t;

typedef struct {
    uint32_t stepsTaken;
    uint32_t currentGoal;
    uint32_t newGoal;
    uint16_t secondsElapsed;
    displayMode_t displayMode;
} stepsInfo_t;


void displayInit(void); // Initalize the display
void displayUpdate(deviceStateInfo_t deviceState, uint16_t secondsElapsed, bool err); // Update the display, given the device's state and the length of the current workout

#define DISPLAY_WIDTH 16

#endif /* DISPLAY_MANAGER_H_ */
