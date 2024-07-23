#include "fff.h"
#include <stdbool.h>
#include "driverlib/adc.h"
#include "inc/hw_memmap.h"

#ifdef FFF_MOCK_IMPL
    #define VOID_FUNC FAKE_VOID_FUNC
    #define VALUE_FUNC FAKE_VALUE_FUNC
#else
    #define VOID_FUNC DECLARE_FAKE_VOID_FUNC
    #define VALUE_FUNC DECLARE_FAKE_VALUE_FUNC
#endif

#define FFF_ADC_FAKES_LIST(FUNC)        \
    FUNC(ADCProcessorTrigger)           \
    FUNC(ADCSequenceDataGet)            \
    FUNC(ADCIntClear)                   \
    FUNC(ADCSequenceStepConfigure)      \
    FUNC(ADCSequenceConfigure)          \
    FUNC(ADCSequenceEnable)             \
    FUNC(ADCIntRegister)                \
    FUNC(ADCIntEnable)                  

typedef void (*void_function_ptr_type)(void);

VOID_FUNC(ADCProcessorTrigger, uint32_t, uint32_t);
VALUE_FUNC(int32_t, ADCSequenceDataGet, uint32_t, uint32_t, uint32_t *);
VOID_FUNC(ADCIntClear, uint32_t, uint32_t);
VOID_FUNC(ADCSequenceStepConfigure, uint32_t, uint32_t, uint32_t, uint32_t);
VOID_FUNC(ADCSequenceConfigure, uint32_t, uint32_t, uint32_t, uint32_t);
VOID_FUNC(ADCSequenceEnable, uint32_t, uint32_t);
VOID_FUNC(ADCIntRegister, uint32_t, uint32_t, void_function_ptr_type);
VOID_FUNC(ADCIntEnable, uint32_t, uint32_t);
