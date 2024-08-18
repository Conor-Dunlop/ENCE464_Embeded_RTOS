/*
 *  speed_tracker.c
 * 
 *  Last Modified: 22/08/2024
 *      Authors: Flynn Underwood
 *
 *  Real-time speed tracker
 *
 */

#include "speed_tracker.h"

SpeedTracker_t SpeedTracker;

// Initialize the SpeedTracker
void initSpeedTracker(SpeedTracker_t* tracker) {
    tracker->previousTime = 0;
    tracker->previousDistance = 0;
    tracker->currentSpeed = 0.0f;
}

float updateSpeed(SpeedTracker_t* tracker, uint16_t currentTime, float currentDistance) 
{
    // Calculate time difference in seconds
    uint32_t timeDifference = currentTime - tracker->previousTime;

    // Avoid division by zero
    if (timeDifference > 0) {
        // Calculate distance difference
        uint32_t distanceDifference = currentDistance - tracker->previousDistance;

        // Calculate speed (distance over time)
        tracker->currentSpeed = (float)distanceDifference / (float)timeDifference;       

        // Update the previous time and distance
        tracker->previousTime = currentTime;
        tracker->previousDistance = currentDistance;
    }

    return tracker->currentSpeed;
}