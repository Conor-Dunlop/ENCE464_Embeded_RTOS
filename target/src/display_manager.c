/*
 * display_manager.c
 *
 * Handles drawing to the device's screen, given its current state
 *
 *  Created on: 23/03/2022
 *      Author: Matthew Suter
 *
 *  FitnessThur9-1
 */


#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/debug.h"
#include "driverlib/pin_map.h"
#include "utils/ustdlib.h"
#include "stdio.h"
#include "stdlib.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "utils/ustdlib.h"

#include "serial_sender.h"
#include "display_manager.h"
#include "speed_tracker.h"


//********************************************************
// Constants and static vars
//********************************************************

#define KM_TO_MILES 62/100 // Multiply by 0.6215 to convert, this should be good enough
#define MS_TO_KMH 36/10
#define TIME_UNIT_SCALE 60


/*******************************************
 *      Local prototypes
 *******************************************/
static void displayLine(char* inStr, uint8_t row, textAlignment_t alignment);
static void displayValue(char* prefix, char* suffix, int32_t value, uint8_t row, textAlignment_t alignment, bool thousandsFormatting);
static void displayTime(char* prefix, uint16_t time, uint8_t row, textAlignment_t alignment);


/*******************************************
 *      Global functions
 *******************************************/
// Init the screen library
void displayInit(void)
{
    OLEDInitialise();
}



// Update the display, called on a loop
void displayUpdate(deviceStateInfo_t deviceState, uint16_t secondsElapsed, bool err)
{
    // Check for flash message override
    if (deviceState.flashTicksLeft != 0) {
        char* emptyLine = "                ";
        OLEDStringDraw (emptyLine, 0, 0);
        displayLine(deviceState.flashMessage, 1, ALIGN_CENTRE);
        OLEDStringDraw (emptyLine, 0, 2);
        OLEDStringDraw (emptyLine, 0, 3);
        return;
    }

    float mTravelled = deviceState.stepsTaken * deviceState.mPerStep;

    float speed = updateSpeed(&SpeedTracker, secondsElapsed, mTravelled) * MS_TO_KMH; 

    if (err) {
        displayInit();
        displayLine("ASSERT ERROR!", 0, ALIGN_CENTRE);
    } else {

        if ((speed) >= deviceState.runningSpeed) {
            displayLine("RUNNING", 3, ALIGN_CENTRE);
        } else if (speed == 0) {
            displayLine("STOPPED", 3, ALIGN_CENTRE);
        } else {
            displayLine("WALKING", 3, ALIGN_CENTRE);
        }
        
        switch (deviceState.displayMode) {
            case DISPLAY_STEPS:
                displayLine("", 0, ALIGN_CENTRE); // Clear the top line
                if (deviceState.displayUnits == UNITS_SI) {
                    displayValue("", "steps", deviceState.stepsTaken, 1, ALIGN_CENTRE, false);
                } else {
                    displayValue("", "% of goal", deviceState.stepsTaken * 100 / deviceState.currentGoal, 1, ALIGN_CENTRE, false);
                }
                displayTime("Time:", secondsElapsed, 2, ALIGN_CENTRE);
                break;
            case DISPLAY_DISTANCE:
                displayTime("Time:", secondsElapsed, 1, ALIGN_CENTRE);

                if (deviceState.displayUnits == UNITS_SI) {
                    displayValue("Dist:", "km", mTravelled, 0, ALIGN_CENTRE, true);
                    displayValue("Speed", "kph", speed, 2, ALIGN_CENTRE, false);
                } else {
                    displayValue("Dist:", "mi", mTravelled * KM_TO_MILES, 0, ALIGN_CENTRE, true);
                    displayValue("Speed", "mph", speed * KM_TO_MILES, 2, ALIGN_CENTRE, false);
                }

                break;
            case DISPLAY_SET_GOAL:
                displayLine("Set goal:", 0, ALIGN_CENTRE);
                displayValue("Current:", "", deviceState.currentGoal, 2, ALIGN_CENTRE, false);

                // Display the step/distance preview
                char toDraw[DISPLAY_WIDTH+1]; // Must be one character longer to account for EOFs
                uint16_t distance = deviceState.newGoal * deviceState.mPerStep;
                if (deviceState.displayUnits != UNITS_SI) {
                    distance = distance * KM_TO_MILES;
                }

                // if <10 km/miles, use a decimal point. Otherwise display whole units (to save space)
                if (distance < 10*1000) {
                    usnprintf(toDraw, DISPLAY_WIDTH + 1, "%d stps/%d.%01d%s", deviceState.newGoal, distance / 1000, (distance % 1000)/100, deviceState.displayUnits == UNITS_SI ? "km" : "mi");
                } else {
                    usnprintf(toDraw, DISPLAY_WIDTH + 1, "%d stps/%d%s", deviceState.newGoal, distance / 1000, deviceState.displayUnits == UNITS_SI ? "km" : "mi");
                }

                displayLine(toDraw, 1, ALIGN_CENTRE);

                break;
            case DISPLAY_TRAITS_EDITOR:
                displayLine("EDIT TRAITS", 0, ALIGN_CENTRE);
                if (deviceState.mPerStep > 0.8) {
                    displayValue("m/step:", "", deviceState.mPerStep * 1000, 1, ALIGN_CENTRE, true);
                } else {
                    displayValue("m/step:", "", deviceState.mPerStep * 1000 + 1, 1, ALIGN_CENTRE, true);
                }
                displayValue("RunSpeed:", "kph", deviceState.runningSpeed, 2, ALIGN_CENTRE, false);
        }
    }
}


/*******************************************
 *      Local Functions
 *******************************************/
// Draw a line to the OLED screen, with the specified alignment
static void displayLine(char* inStr, uint8_t row, textAlignment_t alignment)
{
    // Get the length of the string, but prevent it from being more than 16 chars long
    uint8_t inStrLength = 0;
    while (inStr[inStrLength] != '\0' && inStrLength < DISPLAY_WIDTH) {
        inStrLength++;
    }

    // Create a 16-char long array to write to
    uint8_t i = 0;
    char toDraw[DISPLAY_WIDTH+1]; // Must be one character longer to account for EOFs
    for (i = 0; i < DISPLAY_WIDTH; i++) {
        toDraw[i] = ' ';
    }
    toDraw[DISPLAY_WIDTH] = '\0'; // Set the last character to EOF

    // Set the starting position based on the alignment specified
    uint8_t startPos = 0;
    switch (alignment) {
    case ALIGN_LEFT:
        startPos = 0;
        break;
    case ALIGN_CENTRE:
        startPos = (DISPLAY_WIDTH - inStrLength) /  2;
        break;
    case ALIGN_RIGHT:
        startPos = (DISPLAY_WIDTH - inStrLength);
        break;
    }

    // Copy the string we were given onto the 16-char row
    for (i = 0; i < inStrLength; i++) {
        toDraw[i + startPos] = inStr[i];
    }

    OLEDStringDraw (toDraw, 0, row);
}



// Display a value, with a prefix and suffix
// Can optionally divide the value by 1000, to mimic floats without actually having to use them
static void displayValue(char* prefix, char* suffix, int32_t value, uint8_t row, textAlignment_t alignment, bool thousandsFormatting)
{
    char toDraw[DISPLAY_WIDTH+1]; // Must be one character longer to account for EOFs

    if (thousandsFormatting) {
        // Print a number/1000 to 3dp, with decimal point and sign
        // Use a mega cool ternary operator to decide whether to use a minus sign
        usnprintf(toDraw, DISPLAY_WIDTH + 1, "%s%c%d.%03d %s", prefix, value<0? '-':' ', abs(value / 1000), abs(value) % 1000, suffix);
    } else {
        usnprintf(toDraw, DISPLAY_WIDTH + 1, "%s %d %s", prefix, value, suffix); // Can use %4d if we want uniform spacing
    }

    displayLine(toDraw, row, alignment);
}



// Display a given number of seconds, formatted as mm:ss or hh:mm:ss
static void displayTime(char* prefix, uint16_t time, uint8_t row, textAlignment_t alignment)
{
    char toDraw[DISPLAY_WIDTH+1]; // Must be one character longer to account for EOFs
    uint16_t minutes = (time / TIME_UNIT_SCALE) % TIME_UNIT_SCALE;
    uint16_t seconds = time % TIME_UNIT_SCALE;
    uint16_t hours =   time / (TIME_UNIT_SCALE * TIME_UNIT_SCALE);

    if (hours == 0) {
        usnprintf(toDraw, DISPLAY_WIDTH + 1, "%s %01d:%02d", prefix, minutes, seconds);
    } else {
        usnprintf(toDraw, DISPLAY_WIDTH + 1, "%s %01d:%02d:%02d", prefix, hours, minutes, seconds);
    }

    displayLine(toDraw, row, alignment);
}

