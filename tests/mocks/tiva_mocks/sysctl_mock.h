#include "fff.h"
#include <stdbool.h>
#include "driverlib/sysctl.h"

#ifdef FFF_MOCK_IMPL
    #define VOID_FUNC FAKE_VOID_FUNC
    #define VALUE_FUNC FAKE_VALUE_FUNC
#else
    #define VOID_FUNC DECLARE_FAKE_VOID_FUNC
    #define VALUE_FUNC DECLARE_FAKE_VALUE_FUNC
#endif

#define FFF_SYSCTL_FAKES_LIST(FUNC)     \
    FUNC(SysCtlPeripheralEnable)        \
    FUNC(IntPrioritySet)            	\
    FUNC(SysCtlPeripheralReady) 

VOID_FUNC(SysCtlPeripheralEnable, uint32_t);
VOID_FUNC(IntPrioritySet, uint32_t, uint8_t);
VALUE_FUNC(bool, SysCtlPeripheralReady, uint32_t);

