#ifndef SPEED_TRACKER_H_
#define SPEED_TRACKER_H_

#include <stdint.h>

typedef struct {
    uint16_t previousTime;   // Previous time in milliseconds
    uint16_t previousDistance;  // Previous distance in meters (or other units)
    float currentSpeed;      // Speed in meters per second (or other units per second)
} SpeedTracker_t;

extern SpeedTracker_t SpeedTracker;

void initSpeedTracker(SpeedTracker_t* tracker);
float updateSpeed(SpeedTracker_t* tracker, uint16_t currentTime, float currentDistance);

#endif /* STEP_COUNTER_MAIN_H_ */