/*
 *  sync.c
 * 
 *  Last Modified: 22/08/2024
 *      Authors: Flynn Underwood
 *
 *  Generate mutexes, semaphores, queues and timers using
 *  the FreeRTOS API.
 * 
 *  Used to synchronise the execution of tasks & functions
 *
 */

#include "synch.h"

// Define handles
SemaphoreHandle_t xADCSemaphore; // ADC signalling semaphore
SemaphoreHandle_t xPromptSemaphore; // Move Prompt signalling semaphore
SemaphoreHandle_t xDeviceStateMutex; // deviceState shared resource protection
QueueHandle_t accl_q; // Accelerometer data queue
TimerHandle_t xMoveTimer; // 'Move prompt' timer

// Create semaphores, mutexes, and queues
void createSemaphores(void) {
    xADCSemaphore = xSemaphoreCreateBinary();
    xPromptSemaphore = xSemaphoreCreateBinary();
    xDeviceStateMutex = xSemaphoreCreateMutex();
    accl_q = xQueueCreate(1, BUF_SIZE);
}
