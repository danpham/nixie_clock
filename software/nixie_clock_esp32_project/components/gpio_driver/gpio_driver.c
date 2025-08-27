#include "gpio_driver.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"

esp_err_t my_gpio_init(my_gpio_btn_t *btn) {
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << btn->pin,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = (btn->pull == MY_GPIO_PULL_UP) ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE,
        .pull_down_en = (btn->pull == MY_GPIO_PULL_DOWN) ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    btn->last_state = gpio_get_level(btn->pin);
    btn->last_change_ms = 0;
    return gpio_config(&io_conf);
}

int my_gpio_read_btn(my_gpio_btn_t *btn) {
    int state = gpio_get_level(btn->pin);
    uint32_t now = esp_timer_get_time() / 1000; // in ms
    
    if (state != btn->last_state) {
        if (now - btn->last_change_ms > btn->debounce_ms) {
            btn->last_state = state;
            btn->last_change_ms = now;
        } else {
            state = btn->last_state; // ignore
        }
    }
    return state;
}

