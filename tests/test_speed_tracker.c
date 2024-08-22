#include "unity.h"
#include "speed_tracker.h"
#include "stdlib.h"

static SpeedTracker_t tracker;

void setUp(void)
{
    initSpeedTracker(&tracker);
}

void test_initialisation(void)
{
    // Assert
    TEST_ASSERT_EQUAL(tracker.currentSpeed, 0);
}

void test_speed_calculation(void)
{
    // Act
    float speed = updateSpeed(&tracker, 2, 4);

    // Assert
    TEST_ASSERT_EQUAL(speed, 2);
}