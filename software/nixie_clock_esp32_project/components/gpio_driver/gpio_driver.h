#pragma once
#include "driver/gpio.h"
#include "esp_err.h"

#define BUTTON_LONG_PRESS_MS 2000

typedef enum {
    BUTTON_STATE_RELEASE  = 0,
    BUTTON_STATE_PRESS = 1
} button_state_t;

typedef enum {
    BUTTON_SHORT_PRESS = 0,
    BUTTON_LONG_PRESS  = 1
} button_press_t;

// Pull-up / Pull-down enum
typedef enum {
    MY_GPIO_PULL_NONE,
    MY_GPIO_PULL_UP,
    MY_GPIO_PULL_DOWN
} my_gpio_pull_t;

// Button structure with debounce
typedef struct {
    gpio_num_t pin;
    my_gpio_pull_t pull;
    int last_state;
    button_press_t press_type;
    uint32_t last_change_ms;
    uint32_t press_start_ms;
    uint32_t debounce_ms;
} my_gpio_btn_t;

// GPIO functions
esp_err_t my_gpio_init(my_gpio_btn_t *btn);
int my_gpio_read_btn(my_gpio_btn_t *btn);

// Start the GPIO task
void gpio_task_start(void);
