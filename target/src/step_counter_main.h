/*
 * display_manager.h
 *
 *  Created on: 10/04/2022
 *      Author: Matthew Suter, Tim Preston-Marshall, Daniel Rabbidge
 *
 *  Last Modified: 22/08/2024
 *      Authors: Flynn Underwood, Brennan Drach, Conor Dunlop
 *
 *  Main header file for the ENCE464 Fitness Monitor v2 project
 *
 */

#ifndef STEP_COUNTER_MAIN_H_
#define STEP_COUNTER_MAIN_H_

#include <stdbool.h>

#define M_PER_STEP_DEFAULT 0.900
#define M_PER_STEP_INCREMENT 0.100
#define MAX_M_STEP 2

#define RUN_SPEED_DEFAULT 4
#define RUN_SPEED_INCREMENT 1
#define MAX_RUN_SPEED 13

#define MAX_STR_LEN 16

#define DEBUG_STEP_INCREMENT 100
#define DEBUG_STEP_DECREMENT 500

#define DISPLAY_NUM_STATES 3
typedef enum {
    DISPLAY_STEPS = 0,
    DISPLAY_DISTANCE,
    DISPLAY_SET_GOAL,
    DISPLAY_TRAITS_EDITOR,
} displayMode_t;


typedef enum {
    UNITS_SI = 0,       // Steps  /km
    UNITS_ALTERNATE,    // Percent/miles
    UNITS_NUM_TYPES,
} displayUnits_t;


typedef struct {
    uint32_t stepsTaken;
    uint32_t currentGoal;       // Goal the user is aiming for
    uint32_t newGoal;           // Value from the potentiometer, processed to give a new goal
    float mPerStep;
    float runningSpeed;
    bool debugMode;             // Enable/disable debug functionality
    bool setParamsMode;
    displayMode_t displayMode;
    displayUnits_t displayUnits;
    unsigned long workoutStartTick;

    // For displaying temporary messages over the top of everything else
    char *flashMessage;
    unsigned long flashTicksLeft;

    bool workoutBegun; // Additional trigger to begin workout
} deviceStateInfo_t;

void flashMessage(char* toShow);
void vAssertCalled( const char * pcFile, unsigned long ulLine );

#endif /* STEP_COUNTER_MAIN_H_ */
