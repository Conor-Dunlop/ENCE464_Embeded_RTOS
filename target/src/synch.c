#include "synch.h"

// Define semaphore handles
SemaphoreHandle_t xADCSemaphore; // ADC signalling semaphore
SemaphoreHandle_t xDeviceStateMutex; // deviceState shared resource protection
QueueHandle_t accl_q;

void createSemaphores(void) {
    // Create semaphores, mutexes, and queues
    xADCSemaphore = xSemaphoreCreateBinary();
    xDeviceStateMutex = xSemaphoreCreateMutex();
    accl_q = xQueueCreate(1, BUF_SIZE);
}
