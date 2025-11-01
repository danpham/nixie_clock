#ifndef GPIO_DRIVER_H
#define GPIO_DRIVER_H

/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include "driver/gpio.h"
#include "esp_err.h"

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/
#define BUTTON_LONG_PRESS_MS 2000U

/******************************************************************
 * 3. Typedef definitions (simple typedef, then enum and structs)
******************************************************************/
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
    button_state_t last_state;
    button_press_t press_type;
    uint32_t last_change_ms;
    uint32_t press_start_ms;
    uint32_t debounce_ms;
} my_gpio_btn_t;

/******************************************************************
 * 4. Variable definitions (static then global)
******************************************************************/

/******************************************************************
 * 5. Functions prototypes (static only)
******************************************************************/

/******************************************************************
 * 6. Functions definitions
******************************************************************/
esp_err_t my_gpio_init(my_gpio_btn_t *btn);
int my_gpio_read_btn(my_gpio_btn_t *btn);

#endif // GPIO_DRIVER_H