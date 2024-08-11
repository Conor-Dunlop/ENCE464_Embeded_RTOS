#include "synch.h"

// Define semaphore handles
SemaphoreHandle_t xADCSemaphore; // ADC signalling semaphore
SemaphoreHandle_t xDeviceStateMutex; // deviceState shared resource protection
QueueHandle_t button_q;
QueueHandle_t switch_q;

void createSemaphores(void) {
    // Create semaphores, mutexes, and queues
    xADCSemaphore = xSemaphoreCreateBinary();
    xDeviceStateMutex = xSemaphoreCreateMutex();
    button_q = xQueueCreate(10, sizeof(enum butNames));
    switch_q = xQueueCreate(10, sizeof(enum SWStates));
}
