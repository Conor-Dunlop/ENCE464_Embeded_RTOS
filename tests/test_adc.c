#include "unity.h"
#include "ADC_read.h"

#include "fff.h"
DEFINE_FFF_GLOBALS;
#define FFF_MOCK_IMPL // Includes mock implementations

#include "circBufT_mock.h"
#include "tiva_mocks/adc_mock.h"
#include "tiva_mocks/sysctl_mock.h"

#define ADC_BUF_SIZE 10
#define FAKE_ADC_VALUE 0xFACCEADC // No K's in hex

/* Helper functions */      
void reset_fff(void)
{
    FFF_ADC_FAKES_LIST(RESET_FAKE);
    FFF_SYSCTL_FAKES_LIST(RESET_FAKE);
    FFF_CIRCBUFT_FAKES_LIST(RESET_FAKE);
    FFF_RESET_HISTORY();
}

circBuf_t* get_circBuf_ptr_and_reset_fff(void)
{
    initADC();
    circBuf_t* buffer_ptr = initCircBuf_fake.arg0_val;
    reset_fff();
    return buffer_ptr;
}

void assert_f1_called_before_f2(void* f1, void* f2)
{
    int8_t i_f1 = -1;
    int8_t i_f2 = -1;

    for (uint8_t i = 0; i < FFF_CALL_HISTORY_LEN; i++)
    {
        if(fff.call_history[i] == NULL)
            break;

        if (i_f1 == -1 && fff.call_history[i] == f1) i_f1 = i;
        if (i_f2 == -1 && fff.call_history[i] == f2) i_f2 = i;        
    }
    
    TEST_ASSERT(i_f1 < i_f2);
}

/* Custom fakes */
int32_t ADCSequenceDataGet_fake_adc_value(uint32_t arg0, uint32_t arg1,
                                  uint32_t *arg2)
{
    (void)arg0;
    (void)arg1;
    *arg2 = FAKE_ADC_VALUE;
    return 0;
}

/* Unity setup and teardown */
void setUp(void)
{
    reset_fff();
}

void tearDown(void)
{
    
}

/* Test cases - initADC */
void test_adc_init_initialises_buffer(void)
{   
    // Act
    initADC();

    // Assert
    TEST_ASSERT_EQUAL(1, initCircBuf_fake.call_count);
    TEST_ASSERT_EQUAL(ADC_BUF_SIZE, initCircBuf_fake.arg1_val);
}

void test_adc_init_enables_adc(void)
{
    // Act
    initADC();

    // Assert
    TEST_ASSERT_EQUAL(1, SysCtlPeripheralEnable_fake.call_count);
    TEST_ASSERT_EQUAL(SYSCTL_PERIPH_ADC0, SysCtlPeripheralEnable_fake.arg0_val);
}

void test_adc_init_initialises_adc_sequence(void)
{
    // Act
    initADC();

    // Assert
    TEST_ASSERT_EQUAL(1, ADCSequenceConfigure_fake.call_count);
    TEST_ASSERT_EQUAL(ADC0_BASE, ADCSequenceConfigure_fake.arg0_val);
    TEST_ASSERT_EQUAL(3, ADCSequenceConfigure_fake.arg1_val);  
    TEST_ASSERT_EQUAL(ADC_TRIGGER_PROCESSOR, ADCSequenceConfigure_fake.arg2_val);
    TEST_ASSERT_EQUAL(0, ADCSequenceConfigure_fake.arg3_val);
}

void test_adc_init_initialises_adc_sequence_step(void)
{
    // Act
    initADC();

    // Assert
    TEST_ASSERT_EQUAL(1, ADCSequenceStepConfigure_fake.call_count);
    TEST_ASSERT_EQUAL(ADC0_BASE, ADCSequenceStepConfigure_fake.arg0_val);
    TEST_ASSERT_EQUAL(3, ADCSequenceStepConfigure_fake.arg1_val);  
    TEST_ASSERT_EQUAL(0, ADCSequenceStepConfigure_fake.arg2_val);
    TEST_ASSERT_EQUAL((ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END), ADCSequenceStepConfigure_fake.arg3_val);
}

void test_adc_init_enables_adc_sequence(void)
{
    // Act
    initADC();

    // Assert
    TEST_ASSERT_EQUAL(1, ADCSequenceEnable_fake.call_count);
    TEST_ASSERT_EQUAL(ADC0_BASE, ADCSequenceEnable_fake.arg0_val);
    TEST_ASSERT_EQUAL(3, ADCSequenceEnable_fake.arg1_val);  
}

void test_adc_init_registers_adc_interrupt(void)
{
    // Act
    initADC();

    // Assert
    TEST_ASSERT_EQUAL(1, ADCIntRegister_fake.call_count);
    TEST_ASSERT_EQUAL(ADC0_BASE, ADCIntRegister_fake.arg0_val);
    TEST_ASSERT_EQUAL(3, ADCIntRegister_fake.arg1_val);  
    TEST_ASSERT_EQUAL(ADCIntHandler, ADCIntRegister_fake.arg2_val);
}

void test_adc_init_enables_adc_before_other_adc_operations(void)
{
    // Act
    initADC();

    // Assert
    assert_f1_called_before_f2((void*)SysCtlPeripheralEnable, (void*)ADCSequenceConfigure);
    assert_f1_called_before_f2((void*)SysCtlPeripheralEnable, (void*)ADCSequenceStepConfigure);
    assert_f1_called_before_f2((void*)SysCtlPeripheralEnable, (void*)ADCSequenceEnable);
    assert_f1_called_before_f2((void*)SysCtlPeripheralEnable, (void*)ADCIntRegister);
    assert_f1_called_before_f2((void*)SysCtlPeripheralEnable, (void*)ADCIntEnable);
}

void test_adc_init_enables_adc_interrupt(void)
{
    // Act
    initADC();
    
    // Assert
    TEST_ASSERT_EQUAL(1, ADCIntEnable_fake.call_count);
    TEST_ASSERT_EQUAL(ADC0_BASE, ADCIntEnable_fake.arg0_val);
    TEST_ASSERT_EQUAL(3, ADCIntEnable_fake.arg1_val);  
}

/* Test cases - pollADC */
void test_adc_poll_triggers_adc(void)
{
    // Act
    initADC();
    pollADC();

    // Assert
    TEST_ASSERT_EQUAL(1, ADCProcessorTrigger_fake.call_count);
}

/* Test cases - ADCIntHandler */
void test_adc_correct_channel_and_sequence(void)
{
    // Act
    initADC();

    //Assert
    TEST_ASSERT_EQUAL(3, ADCSequenceConfigure_fake.arg1_val);  
    TEST_ASSERT_EQUAL(0, ADCSequenceStepConfigure_fake.arg2_val);
}

void test_adc_writes_to_correct_buffer(void)
{
    // Act
    initADC();
    circBuf_t* ptr = get_circBuf_ptr_and_reset_fff();
    ADCIntHandler();
    circBuf_t* buffer_written_to = writeCircBuf_fake.arg0_val;

    //Assert
    TEST_ASSERT_EQUAL(ptr, buffer_written_to);
}

void test_adc_writes_correct_value_to_buffer(void)
{
    // Act
    initADC();
    ADCSequenceDataGet_fake.custom_fake = ADCSequenceDataGet_fake_adc_value;

    ADCIntHandler();
    uint32_t written_value = writeCircBuf_fake.arg1_val;

    //Assert
    TEST_ASSERT_EQUAL(FAKE_ADC_VALUE, written_value);
}

void test_adc_clears_interrupt_signal(void)
{
    // Act
    initADC();
    ADCIntHandler();

    //Assert
    TEST_ASSERT_EQUAL(1, ADCIntClear_fake.call_count);

}

void test_adc_reads_from_correct_buffer(void)
{
    // Act
    initADC();
    circBuf_t* ptr = get_circBuf_ptr_and_reset_fff();
    //ADCIntHandler();
    (void) readADC();
    circBuf_t* buffer_read_from = readCircBuf_fake.arg0_val;

    //Assert
    TEST_ASSERT_EQUAL(ptr, buffer_read_from);
}

void test_adc_same_number_averaging(void)
{
    // Act
    int same_num_array[] = {3, 3, 3, 3, 3, 3, 3, 3, 3, 3};
    SET_RETURN_SEQ(readCircBuf, same_num_array, 10);
    //Assert
    TEST_ASSERT_EQUAL(readADC(), 3);
}

void test_adc_number_sequence_averaging(void)
{
    // Act
    int same_num_array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    SET_RETURN_SEQ(readCircBuf, same_num_array, 10);
    //Assert
    TEST_ASSERT_EQUAL(readADC(), 5.5);
}

/* Test cases - readADC */

