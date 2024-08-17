#ifndef SYNCH_H
#define SYNCH_H

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "accl_manager.h"
#include "timers.h"
#include "step_counter_main.h"

// Declare semaphore handles
extern SemaphoreHandle_t xADCSemaphore;
extern SemaphoreHandle_t xPromptSemaphore;
extern SemaphoreHandle_t xDeviceStateMutex;
extern QueueHandle_t accl_q;
extern TimerHandle_t xMoveTimer;

void createSemaphores(void);

#endif // SYNCH_H
