cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(FitnessTrackerTests LANGUAGES C)

enable_testing()
add_subdirectory(unity)
set(SRC_DIR ../target/src)


add_unity_test(test_example test_example.c)
add_unity_test(test_fff test_fff.c)

add_unity_test(test_circular_buffer test_circular_buffer.c ${SRC_DIR}/circBufT.c)
target_include_directories(test_circular_buffer PRIVATE ${SRC_DIR})