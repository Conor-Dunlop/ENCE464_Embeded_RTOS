/*
 * display_manager.h
 *
 *  Created on: 10/04/2022
 *      Author: Matthew Suter, Tim Preston-Marshall, Daniel Rabbidge
 *
 *  FitnessThur9-1
 */

#ifndef STEP_COUNTER_MAIN_H_
#define STEP_COUNTER_MAIN_H_

#define M_PER_STEP 9/10
#define MAX_STR_LEN 16

#define DEBUG_STEP_INCREMENT 100
#define DEBUG_STEP_DECREMENT 500

typedef enum {
    DISPLAY_STEPS = 0,
    DISPLAY_DISTANCE,
    DISPLAY_SET_GOAL,
    DISPLAY_NUM_STATES, // Automatically enumerates to the number of display states there can be
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
    bool debugMode;             // Is enable/disable debug functionality
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
