cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(FitnessTrackerTests LANGUAGES C)

enable_testing()
add_subdirectory(unity)
set(SRC_DIR ../target/src)

add_unity_test(test_example test_example.c)
add_unity_test(test_fff test_fff.c)

add_unity_test(test_circular_buffer test_circular_buffer.c ${SRC_DIR}/circ_buf_t.c)
target_include_directories(test_circular_buffer PRIVATE ${SRC_DIR})

set(TIVAWARE_DIR ../target/libs/tivaware)
set(RTOS_DIR ../target/libs/freertos/include ../target/libs/freertos/portable/GCC/ARM_CM4F ../target)
set(MOCK_DIR mocks)

add_unity_test(test_adc_hal test_adc_hal.c ${SRC_DIR}/adc_hal.c ${SRC_DIR}/circ_buf_t.c)
target_include_directories(test_adc_hal PRIVATE ${SRC_DIR} ${TIVAWARE_DIR} ${MOCK_DIR} ${RTOS_DIR})
target_compile_definitions(test_adc_hal PRIVATE PART_TM4C123GH6PM)

add_unity_test(test_adc test_adc.c ${SRC_DIR}/ADC_read.c ${SRC_DIR}/adc_hal.c)
target_include_directories(test_adc PRIVATE ${SRC_DIR} ${TIVAWARE_DIR} ${MOCK_DIR} ${RTOS_DIR})
target_compile_definitions(test_adc PRIVATE PART_TM4C123GH6PM TESTING)

