#ifndef SYNCH_H
#define SYNCH_H

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "buttons4.h"
#include "switches.h"

// Declare semaphore handles
extern SemaphoreHandle_t xADCSemaphore;
extern SemaphoreHandle_t xDeviceStateMutex;
extern QueueHandle_t button_q;
extern QueueHandle_t switch_q;

void createSemaphores(void);

#endif // SYNCH_H
