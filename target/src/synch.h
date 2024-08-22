/*
 *  sync.h
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

#ifndef SYNCH_H
#define SYNCH_H

#include "FreeRTOS.h"
#include "semphr.h"
#include "accl_manager.h"
#include "timers.h"

// Declare externally observable handles
extern SemaphoreHandle_t xADCSemaphore;
extern SemaphoreHandle_t xResetSemaphore;
extern SemaphoreHandle_t xPromptSemaphore;
extern SemaphoreHandle_t xDeviceStateMutex;
extern QueueHandle_t accl_q;
extern TimerHandle_t xMoveTimer;

// Create semaphores, mutexes, and queues
void createSemaphores(void);

#endif // SYNCH_H
