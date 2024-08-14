#include "unity.h"
#include "adc_hal.h"

#include "fff.h"
DEFINE_FFF_GLOBALS
#define FFF_MOCK_IMPL // Includes mock implementations

#define ADC_INVALID_ID 42

#include "tiva_mocks/adc_mock.h"
#include "tiva_mocks/sysctl_mock.h"
#include "circBufT.h"

/* Helper functions */
void reset_fff(void)
{
    FFF_ADC_FAKES_LIST(RESET_FAKE);
    FFF_SYSCTL_FAKES_LIST(RESET_FAKE);
    FFF_RESET_HISTORY();
}

void dummy_callback(uint32_t val) {
    ;
}

/* Unity setup and teardown */
void setUp(void)
{
    reset_fff();
}

void test_adc_hal_registers_an_adc(void)
{
    adc_hal_register(ADC_ID_1, dummy_callback);
}

void test_adc_hal_registers_correct_adc(void)
{
    adc_hal_register(ADC_ID_1, dummy_callback);

    TEST_ASSERT_EQUAL(1, SysCtlPeripheralEnable_fake.call_count);
    TEST_ASSERT_EQUAL(SYSCTL_PERIPH_ADC0, SysCtlPeripheralEnable_fake.arg0_val);
}

void test_adc_hal_configures_correct_adc_sequence(void)
{
    adc_hal_register(ADC_ID_1, dummy_callback);

    TEST_ASSERT_EQUAL(1, ADCSequenceConfigure_fake.call_count);
    TEST_ASSERT_EQUAL(ADC0_BASE, ADCSequenceConfigure_fake.arg0_val);
    TEST_ASSERT_EQUAL(1, ADCSequenceConfigure_fake.arg1_val);
    TEST_ASSERT_EQUAL(ADC_TRIGGER_PROCESSOR, ADCSequenceConfigure_fake.arg2_val);
    TEST_ASSERT_EQUAL(0, ADCSequenceConfigure_fake.arg3_val);
}

void test_adc_hal_does_not_register_invalid_id(void)
{
    adc_hal_register(ADC_INVALID_ID, dummy_callback);

    TEST_ASSERT_EQUAL(0, SysCtlPeripheralEnable_fake.call_count);
}



void test_adc_hal_triggers_adc(void)
{
    adc_hal_register(ADC_ID_1, dummy_callback);
    adc_hal_start_conversion(ADC_ID_1);

    TEST_ASSERT_EQUAL(1, ADCProcessorTrigger_fake.call_count);
}

void test_adc_hal_does_not_trigger_invalid_id(void)
{
    adc_hal_register(ADC_ID_1, dummy_callback);
    adc_hal_start_conversion(ADC_INVALID_ID);

    TEST_ASSERT_EQUAL(0, ADCProcessorTrigger_fake.call_count);
}

void test_adc_hal_isr_reads_correct_channel(void)
{
    adc_hal_register(ADC_ID_1, dummy_callback);
    adc_hal_isr();

    TEST_ASSERT_EQUAL(ADC0_BASE, ADCSequenceDataGet_fake.arg0_val);
    TEST_ASSERT_EQUAL(3, ADCSequenceDataGet_fake.arg1_val);
}

void test_adc_hal_isr_clears_interrupt(void)
{
    adc_hal_register(ADC_ID_1, dummy_callback);
    adc_hal_isr();

    TEST_ASSERT_EQUAL(1, ADCIntClear_fake.call_count);
}

void tearDown(void)
{
    
}
