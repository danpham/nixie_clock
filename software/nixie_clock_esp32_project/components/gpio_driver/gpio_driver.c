#include "gpio_driver.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"

esp_err_t my_gpio_init(my_gpio_btn_t *btn)
{
    esp_err_t err = ESP_OK;
    gpio_config_t io_conf = {0};

    if (btn != NULL) {
        io_conf.pin_bit_mask = UINT64_C(1) << (uint64_t)btn->pin;
        io_conf.mode = GPIO_MODE_INPUT;
        io_conf.pull_up_en = (btn->pull == MY_GPIO_PULL_UP) ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE;
        io_conf.pull_down_en = (btn->pull == MY_GPIO_PULL_DOWN) ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE;
        io_conf.intr_type = GPIO_INTR_DISABLE;

        err = gpio_config(&io_conf);

        if (err == ESP_OK) {
            btn->last_state = gpio_get_level(btn->pin);
            btn->last_change_ms = 0;
        }
    } else {
        err = ESP_ERR_INVALID_ARG;
    }
    
    return err;
}

int my_gpio_read_btn(my_gpio_btn_t *btn) {
    int state = BUTTON_STATE_RELEASE;
    uint32_t now = esp_timer_get_time() / 1000;
    uint32_t duration = 0;
    uint32_t  long_press_ms = BUTTON_LONG_PRESS_MS;

    if (btn != NULL) {
        state = gpio_get_level(btn->pin);
        btn->press_type = BUTTON_SHORT_PRESS;

        if (state != btn->last_state) {
            if (now - btn->last_change_ms > btn->debounce_ms) {
                
                if (state == BUTTON_STATE_PRESS && btn->last_state == BUTTON_STATE_RELEASE) {
                    btn->press_start_ms = now;
                }

                if (state == BUTTON_STATE_RELEASE && btn->last_state == BUTTON_STATE_PRESS) {
                    duration = now - btn->press_start_ms;
                    btn->press_type = (duration >= long_press_ms) ? BUTTON_LONG_PRESS : BUTTON_SHORT_PRESS;
                }

                btn->last_state = state;
                btn->last_change_ms = now;
            } else {
                state = btn->last_state;
            }
        }
    }

    return state;
}
