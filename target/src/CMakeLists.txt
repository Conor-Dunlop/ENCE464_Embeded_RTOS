add_tiva_executable(fitness-tracker
    accl_manager.c
    ADC_read.c
    button_manager.c
    buttons4.c
    circBufT.c
    circBufV.c
    display_manager.c
    i2c_driver.c
    serial_sender.c
    step_counter_main.c
    switches.c
)
target_compile_options(fitness-tracker PRIVATE -Wall)
target_link_libraries(fitness-tracker PRIVATE tivaware OrbitOLED freertos::heap-4 m)
add_program_target(fitness-tracker PROGRAM_TARGET program DEBUG_TARGET debug)
