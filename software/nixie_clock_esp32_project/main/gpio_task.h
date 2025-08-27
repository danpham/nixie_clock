#ifndef GPIO_TASK_H
#define GPIO_TASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

// Event struct shared between tasks
typedef struct {
    uint8_t id;       // Button number (1, 2, etc.)
    uint8_t pressed;  // 1 = pressed, 0 = released
} button_event_t;

// Declaration of the queue used for communication between tasks
extern QueueHandle_t buttonQueue;

// Prototype to start the GPIO task
void gpio_task_start(void);

#endif // GPIO_TASK_H