#ifndef GPIO_TASK_H
#define GPIO_TASK_H

/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "../gpio_driver/gpio_driver.h"
#include "../rotary_encoder/rotary_encoder.h"

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/

/******************************************************************
 * 3. Typedef definitions (simple typedef, then enum and structs)
******************************************************************/
typedef enum buttons_type_e {
    BUTTON_ROTARY_SWITCH_1 = 0U,
    BUTTON_ROTARY_ENCODER = 1U,
    BUTTON_COUNT = 2U
} buttons_type_t;

/* Event struct shared between tasks */
typedef struct {
    buttons_type_t id;
    button_press_t pressed;
    rotary_encoder_event_t updateValue;
} button_event_t;

/******************************************************************
 * 4. Variable definitions (static then global)
******************************************************************/
extern QueueHandle_t buttonQueue;

/******************************************************************
 * 5. Functions prototypes (static only)
******************************************************************/

/******************************************************************
 * 6. Functions definitions (public API in .c)
******************************************************************/
void gpio_task_start(void);

#endif // GPIO_TASK_H