#ifndef GPIO_TASK_H
#define GPIO_TASK_H

/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/

/******************************************************************
 * 3. Typedef definitions (simple typedef, then enum and structs)
******************************************************************/
typedef enum buttons_type_e {
    BUTTON_ROTARY_SWITCH_1 = 0,
    BUTTON_ROTARY_ENCODER = 1,
    BUTTON_COUNT = 2
} buttons_type_t;

// Event struct shared between tasks
typedef struct {
    buttons_type_t id;    // Button number (1, 2, etc.)
    uint8_t pressed;      // 1 = pressed, 0 = released
    uint8_t updateValue;  // 1 = increment, 0 = decrement
} button_event_t;

/******************************************************************
 * 4. Variable definitions (static then global)
******************************************************************/
// Declaration of the queue used for communication between tasks
extern QueueHandle_t buttonQueue;

/******************************************************************
 * 5. Functions prototypes (static only)
******************************************************************/

/******************************************************************
 * 6. Functions definitions (public API in .c)
******************************************************************/
void gpio_task_start(void);

#endif // GPIO_TASK_H