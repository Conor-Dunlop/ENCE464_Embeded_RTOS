#ifndef SYNCH_H
#define SYNCH_H

#include "FreeRTOS.h"
#include "semphr.h"
#include "accl_manager.h"
#include "timers.h"

// Declare semaphore handles
extern SemaphoreHandle_t xADCSemaphore;
extern SemaphoreHandle_t xPromptSemaphore;
extern SemaphoreHandle_t xDeviceStateMutex;
extern QueueHandle_t accl_q;
extern TimerHandle_t xMoveTimer;

void createSemaphores(void);

#endif // SYNCH_H
