#ifndef GPIO_TASK_H
#define GPIO_TASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

typedef enum buttons_type_e {
    BUTTON_ROTARY_SWITCH_1 = 0,
    BUTTON_ROTARY_ENCODER = 1,
    BUTTON_COUNT
} buttons_type_t;

typedef enum rotary_encoder_event_e {
    ROTARY_ENCODER_EVENT_INCREMENT = 0,
    ROTARY_ENCODER_EVENT_DECREMENT = 1,
    ROTARY_ENCODER_EVENT_COUNT
} rotary_encoder_event_type_t;

// Event struct shared between tasks
typedef struct {
    uint8_t id;       // Button number (1, 2, etc.)
    uint8_t pressed;  // 1 = pressed, 0 = released
    uint8_t updateValue;  // 1 = increment, 0 = decrement
} button_event_t;

// Declaration of the queue used for communication between tasks
extern QueueHandle_t buttonQueue;

// Prototype to start the GPIO task
void gpio_task_start(void);

#endif // GPIO_TASK_H