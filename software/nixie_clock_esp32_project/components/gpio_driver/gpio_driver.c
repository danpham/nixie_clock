/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#ifdef STATIC_ANALYSIS
#include "../test/common/esp_stub.h"
#endif
#include "gpio_driver.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/
#define BUTTON_LONG_PRESS_MS    (2000U)

/******************************************************************
 * 3. Typedef definitions (simple typedef, then enum and structs)
******************************************************************/

/******************************************************************
 * 4. Variable definitions (static then global)
******************************************************************/

/******************************************************************
 * 5. Functions prototypes (static only)
******************************************************************/

/******************************************************************
 * 6. Functions definitions
******************************************************************/

/**
 * @brief Initialize a GPIO button
 *
 * Configures the GPIO pin as input with pull-up or pull-down according
 * to the button configuration and initializes its runtime state.
 *
 * @param[in,out] btn Pointer to the button structure to initialize
 * @return
 *      - ESP_OK: Initialization successful
 *      - ESP_ERR_INVALID_ARG: NULL pointer passed
 *      - Other ESP_ERR_XXX codes if GPIO API fails
 */
esp_err_t my_gpio_init(my_gpio_btn_t *btn) {
    esp_err_t err = ESP_ERR_INVALID_ARG;
    static bool isr_service_installed = false;
    
    if (btn != NULL) {
        gpio_config_t io_conf = {0};
        io_conf.pin_bit_mask = UINT64_C(1) << (uint64_t)btn->pin;
        io_conf.mode = GPIO_MODE_INPUT;
        io_conf.pull_up_en = (btn->pull == MY_GPIO_PULL_UP) ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE;
        io_conf.pull_down_en = (btn->pull == MY_GPIO_PULL_DOWN) ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE;
        io_conf.intr_type = btn->intr_type;
        err = gpio_config(&io_conf);
        
        /* Install ISR service only once */
        if ((err == ESP_OK) && (isr_service_installed == false)) {
            esp_err_t ret_isr_install = gpio_install_isr_service(0);
            if ((ret_isr_install != ESP_OK) && (ret_isr_install != ESP_ERR_INVALID_STATE)) {
                err = ret_isr_install;
            } else {
                isr_service_installed = true;
            }
        }
        
        if ((err == ESP_OK) && (btn->intr_type != GPIO_INTR_DISABLE)) {
            gpio_isr_handler_add(btn->pin, btn->isr_handler, (void*) btn->pin);
        }
        
        if (err == ESP_OK) {
            btn->previous_state = gpio_get_level(btn->pin);
            btn->last_change_ms = 0;
        }
    }
    
    return err;
}

/**
 * @brief Read the current state of a button and detect press type
 *
 * Reads the physical GPIO state, applies debounce filtering,
 * and determines whether the press is short or long.
 *
 * @param[in,out] btn Pointer to the button structure
 * @return
 *      - BUTTON_STATE_RELEASE: button released
 *      - BUTTON_STATE_PRESS: button pressed
 *
 * Updates the button structure:
 *      - previous_state: last physical state
 *      - press_type: detected press type (short or long)
 */
button_state_t my_gpio_read_btn(my_gpio_btn_t *btn) {
    button_state_t state = BUTTON_STATE_RELEASE;
    uint32_t now = esp_timer_get_time() / 1000;

    if (btn != NULL) {
        state = gpio_get_level(btn->pin);
        btn->press_type = BUTTON_SHORT_PRESS;

        if (state != btn->previous_state) {
            if ((uint32_t)(now - btn->last_change_ms) > btn->debounce_ms) {
                
                if ((state == (button_state_t)BUTTON_STATE_PRESS) && (btn->previous_state == (button_state_t)BUTTON_STATE_RELEASE)) {
                    btn->press_start_ms = now;
                }

                if ((state == (button_state_t)BUTTON_STATE_RELEASE) && (btn->previous_state == (button_state_t)BUTTON_STATE_PRESS)) {
                    uint32_t duration = 0;
                    duration = now - btn->press_start_ms;
                    btn->press_type = (duration >= (uint32_t)BUTTON_LONG_PRESS_MS) ? (button_press_t)BUTTON_LONG_PRESS : (button_press_t)BUTTON_SHORT_PRESS;
                }

                btn->previous_state = state;
                btn->last_change_ms = now;
            } else {
                state = btn->previous_state;
            }
        }
    }

    return state;
}
