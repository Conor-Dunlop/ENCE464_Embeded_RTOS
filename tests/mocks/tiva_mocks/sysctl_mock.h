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
    FUNC(SysCtlPeripheralEnable)        

VOID_FUNC(SysCtlPeripheralEnable, uint32_t);
