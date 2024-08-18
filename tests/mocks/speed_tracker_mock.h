#include "fff.h"
#include "speed_tracker.h"

#ifdef FFF_MOCK_IMPL
    #define VOID_FUNC FAKE_VOID_FUNC
    #define VALUE_FUNC FAKE_VALUE_FUNC
#else
    #define VOID_FUNC DECLARE_FAKE_VOID_FUNC
    #define VALUE_FUNC DECLARE_FAKE_VALUE_FUNC
#endif

#define FFF_speed_tracker_FAKES_LIST(FUNC)     \
    FUNC(initSpeedTracker)                  \
    FUNC(updateSpeed)  

VOID_FUNC(initSpeedTracker, SpeedTracker_t *);
VALUE_FUNC(float, updateSpeed, SpeedTracker_t *, uint16_t);
