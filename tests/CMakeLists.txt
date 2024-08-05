cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(FitnessTrackerTests LANGUAGES C)

enable_testing()
add_subdirectory(unity)
set(SRC_DIR ../target/src)


add_unity_test(test_example test_example.c)
add_unity_test(test_fff test_fff.c)

add_unity_test(test_circular_buffer test_circular_buffer.c ${SRC_DIR}/circBufT.c)
target_include_directories(test_circular_buffer PRIVATE ${SRC_DIR})

set(TIVAWARE_DIR ../target/libs/tivaware)
set(MOCK_DIR mocks)

add_unity_test(test_adc_hal test_adc_hal.c ${SRC_DIR}/adc_hal.c)
target_include_directories(test_adc_hal PRIVATE ${SRC_DIR} ${TIVAWARE_DIR} ${MOCK_DIR})

add_unity_test(test_adc test_adc.c ${SRC_DIR}/ADC_read.c)
target_include_directories(test_adc PRIVATE ${SRC_DIR} ${TIVAWARE_DIR} ${MOCK_DIR})