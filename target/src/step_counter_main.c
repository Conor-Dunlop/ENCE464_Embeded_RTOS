/*
 * step_counter_main.c
 *
 *  Created on: 23/03/2022
 *      Authors: Matthew Suter, Daniel Rabbidge, Timothy Preston-Marshall
 * 
 *  Last Modified: 22/08/2024
 *      Authors: Flynn Underwood, Brennan Drach, Conor Dunlop
 *
 *  Main code for the ENCE464 Fitness Monitor v2 project
 *
 */

// Comment this out to disable serial plotting
#define SERIAL_PLOTTING_ENABLED


#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "circ_buf_v.h"
#include "ADC_read.h"
#include "synch.h"
#include "buttons4.h"
#include "display_manager.h"
#include "button_manager.h"

#ifdef SERIAL_PLOTTING_ENABLED
#include "serial_sender.h"
#endif //SERIAL_PLOTTING_ENABLED

#include "step_counter_main.h"

/**********************************************************
 * Constants and types
 **********************************************************/
#define RATE_SYSTICK_HZ 250 /4
#define RATE_IO_HZ 150
#define POT_HZ 50
#define RATE_ACCL_HZ 200
#define RATE_BLINK_HZ 10
#define RATE_RESET_HZ 10
#define RATE_DISPLAY_UPDATE_HZ 5
#define FLASH_MESSAGE_TIME 3/2 // seconds

#ifdef SERIAL_PLOTTING_ENABLED
#define RATE_SERIAL_PLOT_HZ 2
#endif // SERIAL_PLOTTING_ENABLED

#define MOVE_PROMPT_PERIOD 5000 // 5 seconds
#define DEVICE_MUTEX_DELAY pdMS_TO_TICKS(100)

#define STEP_GOAL_ROUNDING 100
#define STEP_THRESHOLD_HIGH 270
#define STEP_THRESHOLD_LOW 235

#define TARGET_DISTANCE_DEFAULT 1000

#define POT_SCALE_COEFF 20000/4095 // in steps, adjusting to account for the potentiometer's maximum possible reading


/*******************************************
 *      Globals
 *******************************************/
deviceStateInfo_t deviceState; // Stored as one global so it can be accessed by other helper libs within this main module

/*******************************************
 *      Locals
 *******************************************/
// Converted frequencies for vTaskDelayUntil
const TickType_t xAcclFrequency = pdMS_TO_TICKS(1000 / RATE_ACCL_HZ);
const TickType_t xIOFrequency = pdMS_TO_TICKS(1000 / RATE_IO_HZ);
const TickType_t xPotFrequency = pdMS_TO_TICKS(1000 / POT_HZ);
const TickType_t xDispFrequency = pdMS_TO_TICKS(1000 / RATE_DISPLAY_UPDATE_HZ);
const TickType_t xBlinkFrequency = pdMS_TO_TICKS(1000 / RATE_BLINK_HZ);
const TickType_t xResetFrequency = pdMS_TO_TICKS(1000 / RATE_RESET_HZ);

#ifdef SERIAL_PLOTTING_ENABLED
const TickType_t xSerialFrequency = pdMS_TO_TICKS(1000 / RATE_SERIAL_PLOT_HZ);
#endif // SERIAL_PLOTTING_ENABLED

// Global process tracking variables. Updated by tasks, used for protection
unsigned long lastIoProcess= 0;
unsigned long lastPotProcess = 0;
unsigned long lastAcclProcess = 0;
unsigned long lastDisplayProcess = 0;
unsigned long lastBlinkProcess = 0;
unsigned long lastResetProcess = 0;

#ifdef SERIAL_PLOTTING_ENABLED
unsigned long lastSerialProcess = 0;
#endif // SERIAL_PLOTTING_ENABLED

volatile bool timerResetAfterExpiry = false;  // Flag to check if the timer was reset after expiry


/***********************************************************
 * Helper functions
 ***********************************************************/
// Flash a message onto the screen, overriding everything else
void flash_message(char* toShow)
{
    if (xSemaphoreTake(xDeviceStateMutex, DEVICE_MUTEX_DELAY) == pdTRUE) {
        deviceState.flashTicksLeft = RATE_DISPLAY_UPDATE_HZ * FLASH_MESSAGE_TIME;

        uint8_t i = 0;
        while (toShow[i] != '\0' && i < MAX_STR_LEN) {
            (deviceState.flashMessage)[i] = toShow[i];

            i++;
        }

        deviceState.flashMessage[i] = '\0';

        xSemaphoreGive(xDeviceStateMutex);
    }
}

// Code to execute when the timer expires
void vTimerCallback(TimerHandle_t xTimer) {
    xSemaphoreGive(xPromptSemaphore);
}


/***********************************************************
 * Initialisation functions
 ***********************************************************/
void init_clock (void)
{
    // Set the clock rate to 80 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
}

void init_timer (void)
{
    // Initialise 'prompt to move' timer
    const TickType_t xTimerPeriod = pdMS_TO_TICKS(MOVE_PROMPT_PERIOD);

    xMoveTimer = xTimerCreate("MovementTimer", // Name of the timer
                            xTimerPeriod,      // Period of the timer
                            pdFALSE,            // Auto-reload (pdFALSE for single-shot)
                            (void*)0,          // Timer ID (not used here)
                            vTimerCallback);   // Callback function
     
    xTimerStart(xMoveTimer, 0);
}

void init_LED (void)
{   
    // Initialise red LED GPIO peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
}

void initDevice (void)
{
    // Device state
    // Omnibus struct that holds loads of info about the device's current state, so it can be updated from any function
    deviceState.displayMode = DISPLAY_STEPS;
    deviceState.stepsTaken = 0;
    deviceState.currentGoal = TARGET_DISTANCE_DEFAULT;
    deviceState.mPerStep = M_PER_STEP_DEFAULT;
    deviceState.runningSpeed = RUN_SPEED_DEFAULT;
    deviceState.debugMode = false;
    deviceState.setParamsMode = false;
    deviceState.workoutBegun = false;
    deviceState.displayUnits= UNITS_SI;
    deviceState.workoutStartTick = 0;
    deviceState.flashTicksLeft = 0;
    deviceState.flashMessage = calloc(MAX_STR_LEN + 1, sizeof(char));
}

/***********************************************************
 * FreeRTOS System Tasks
 ***********************************************************/

// Check for RESET
static void check_reset(void* args)
{
    static TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        // Delay task
        vTaskDelayUntil(&xLastWakeTime, xResetFrequency);

        lastResetProcess = xLastWakeTime;

        if (xSemaphoreTake(xResetSemaphore, 0) == pdTRUE) {
            flash_message("Reset!");
        }  
    }
} 

// Blink Red LED
static void blink(void* args) 
{
    static TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    for(;;) {
        // Delay task
        vTaskDelayUntil(&xLastWakeTime, xBlinkFrequency);

        lastBlinkProcess = xLastWakeTime;

        // Turn on red LED if given semaphore from the prompt timer
        if (xSemaphoreTake(xPromptSemaphore, 0) == pdTRUE) {
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, -1);
        }  
    }
}

// Poll the buttons and switches
static void poll_butt_and_switch(void *arg)
{
    static TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        // Delay task
        vTaskDelayUntil(&xLastWakeTime, xIOFrequency);

        lastIoProcess = xLastWakeTime;

        if (xSemaphoreTake(xDeviceStateMutex, DEVICE_MUTEX_DELAY) == pdTRUE) {
            // Update deviceState depending on button states
            for (enum butNames button = 0; button < NUM_BUTS; button++) {
                btnUpdateState(&deviceState, button);
            }

            // Update deviceState depending on switch states
            for (enum SWNames switches = 0; switches < NUM_SW; switches++) {
                swUpdateState(&deviceState, switches);
            }

            xSemaphoreGive(xDeviceStateMutex);
        }
    }
}

// Update newGoal based on potentiometer state
static void update_new_goal(void* args) 
{
    static TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount ();

    for (;;) {
        // Delay task
        xTaskDelayUntil(&xLastWakeTime, xPotFrequency);

        lastPotProcess = xLastWakeTime;

        pollADC();

        if (xSemaphoreTake(xDeviceStateMutex, DEVICE_MUTEX_DELAY) == pdTRUE) {
            // Check for the signal from the ADC ISR
            if (xSemaphoreTake(xADCSemaphore, 0) == pdTRUE) {
                deviceState.newGoal = readADC() * POT_SCALE_COEFF; // Set the new goal value, scaling to give the desired range
            } 

            deviceState.newGoal = (deviceState.newGoal / STEP_GOAL_ROUNDING) * STEP_GOAL_ROUNDING; // Round to the nearest 100 steps

            if (deviceState.newGoal == 0) { // Prevent a goal of zero, instead setting to the minimum goal (this also makes it easier to test the goal-reaching code on a small but non-zero target)
                deviceState.newGoal = STEP_GOAL_ROUNDING;
            }

            xSemaphoreGive(xDeviceStateMutex);
        }
    }
}

// Read and process the accelerometer
static void read_process_accl(void* args) 
{
    static TickType_t xLastWakeTime;
    static uint8_t stepHigh = false;
    uint16_t combined;

    xLastWakeTime = xTaskGetTickCount ();
    for (;;) {
        xTaskDelayUntil(&xLastWakeTime, xAcclFrequency);

        lastAcclProcess = xLastWakeTime;

        acclProcess();

        if (xQueueReceive(accl_q, &combined, 0) == pdPASS) {
            if (combined >= STEP_THRESHOLD_HIGH && stepHigh == false) {
                stepHigh = true;
                if (xSemaphoreTake(xDeviceStateMutex, DEVICE_MUTEX_DELAY) == pdTRUE) {
                    // Access and modify deviceState
                    deviceState.stepsTaken++;
                    // Release mutex
                    xSemaphoreGive(xDeviceStateMutex);
                }
                // flash a message if the user has reached their goal
                if (deviceState.stepsTaken == deviceState.currentGoal && deviceState.flashTicksLeft == 0) {
                    flash_message("Goal reached!");
                }

            } else if (combined <= STEP_THRESHOLD_LOW) {
                stepHigh = false;
            }
            // Don't start the workout until the user begins walking
            if (deviceState.stepsTaken != 0) {
                if (xSemaphoreTake(xDeviceStateMutex, DEVICE_MUTEX_DELAY) == pdTRUE) {
                    // Access and modify deviceState
                    deviceState.workoutBegun = true;
                    // Release mutex
                    xSemaphoreGive(xDeviceStateMutex);
                }
            } else {
                if (xSemaphoreTake(xDeviceStateMutex, DEVICE_MUTEX_DELAY) == pdTRUE) {
                    // Access and modify deviceState
                    deviceState.workoutStartTick = xLastWakeTime;
                    // Release mutex
                    xSemaphoreGive(xDeviceStateMutex);
                }
            }
        }
    }
}

// Write to the display
static void write_to_display(void* args) {
    static TickType_t xLastWakeTime;
    uint16_t secondsElapsed;

    xLastWakeTime = xTaskGetTickCount ();
    for (;;) {

        xTaskDelayUntil(&xLastWakeTime, xDispFrequency);

        lastDisplayProcess = xLastWakeTime;

        if (deviceState.flashTicksLeft > 0) {
            if (xSemaphoreTake(xDeviceStateMutex, DEVICE_MUTEX_DELAY) == pdTRUE) {
                // Access and modify deviceState
                deviceState.flashTicksLeft--;
                // Release mutex
                xSemaphoreGive(xDeviceStateMutex);
            }
        }

        if (deviceState.workoutBegun) {
            secondsElapsed = (xLastWakeTime - deviceState.workoutStartTick)/RATE_SYSTICK_HZ;
        } else {
            secondsElapsed = 0;
        }

        displayUpdate(deviceState, secondsElapsed, false);
    }
}

// Send to USB via serial
#ifdef SERIAL_PLOTTING_ENABLED
static void send_USB_via_serial(void* args) {
    static TickType_t xLastWakeTime;
    // vector3_t mean;
    uint16_t combined = 0;

    xLastWakeTime = xTaskGetTickCount ();

    for (;;) {

        xTaskDelayUntil(&xLastWakeTime, xSerialFrequency);

        lastSerialProcess = xLastWakeTime;
        
        SerialPlot(deviceState.stepsTaken, deviceState.currentGoal, deviceState.runningSpeed, combined);
    }
}
#endif // SERIAL_PLOTTING_ENABLED

// Protection in the unlikely case the device is left running for long enough for the system tick counter to overflow
// This prevents the last process ticks from being 'in the future', which would prevent the update functions from being called,
// rendering the device inoperable.
// This would take ~49 days, but is not impossible if the user forgets to turn it off before they put it away (assuming the battery lasts that long)
static void left_running_protection(void* args) 
{
    static TickType_t xLastWakeTime;
    for (;;)
    {
        xLastWakeTime = xTaskGetTickCount ();

        if (xLastWakeTime < lastBlinkProcess) {
            lastBlinkProcess = 0;
        }

        if (xLastWakeTime < lastIoProcess) {
            lastIoProcess = 0;
        }

        if (xLastWakeTime < lastIoProcess) {
            lastPotProcess = 0;
        }

        if (xLastWakeTime < lastAcclProcess) {
            lastAcclProcess = 0;
        }

        if (xLastWakeTime < lastDisplayProcess) {
            lastDisplayProcess = 0;
        }

        #ifdef SERIAL_PLOTTING_ENABLED
        if (xLastWakeTime < lastSerialProcess) {
            lastSerialProcess = 0;
        }
        #endif // SERIAL_PLOTTING_ENABLED
    }
}

/***********************************************************
 * Main Function
 ***********************************************************/

int main(void)
{
    // Initialisation
    init_clock();
    displayInit();
    btnInit();
    acclInit();
    initADC();
    init_timer();
    init_LED();
    initDevice();

    #ifdef SERIAL_PLOTTING_ENABLED
    SerialInit ();
    #endif // SERIAL_PLOTTING_ENABLED

    // Create mutexes and semaphores
    createSemaphores();

    // Create tasks
    xTaskCreate(check_reset, "CheckToReset", 32, NULL, 2, NULL);

    xTaskCreate(blink, "Blink", 16, NULL, 3, NULL);

    xTaskCreate(write_to_display, "WriteToDisplay", 128, NULL, 1, NULL);

    xTaskCreate(update_new_goal, "UpdateNewGoal", 64, NULL, 2, NULL);

    xTaskCreate(poll_butt_and_switch, "PollButtAndSW", 64, NULL, 2, NULL);

    xTaskCreate(read_process_accl, "ReadProcessAccl", 128, NULL, 1, NULL);

    xTaskCreate(left_running_protection, "TicksProtection", 32, NULL, 1, NULL);

    #ifdef SERIAL_PLOTTING_ENABLED
    xTaskCreate(send_USB_via_serial, "USBViaSerial", 64, NULL, 3, NULL);
    #endif // SERIAL_PLOTTING_ENABLED
 
    // Start the scheduler
    vTaskStartScheduler();

    return 0; // Should never reach here
}

void vAssertCalled( const char * pcFile, unsigned long ulLine ) {
    (void)pcFile; // unused
    (void)ulLine; // unused

    vTaskSuspendAll(); // Suspend all tasks

    displayUpdate(deviceState, 0, true); // Display ASSERT ERROR

    while (true); // Safe state
}