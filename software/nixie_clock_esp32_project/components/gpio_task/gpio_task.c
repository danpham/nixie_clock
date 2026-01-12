/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#ifdef STATIC_ANALYSIS
#include "../test/common/esp_stub.h"
#endif
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
#include "esp_log.h"
#include "gpio_task.h"
#include "../gpio_driver/gpio_driver.h"
#include "../rotary_encoder/rotary_encoder.h"
#include "../event_bus/event_bus.h"

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/

/******************************************************************
 * 3. Typedef definitions (simple typedef, then enum and structs)
******************************************************************/

/******************************************************************
 * 4. Variable definitions (static then global)
******************************************************************/

/******************************************************************
 * 5. Functions prototypes (static only)
******************************************************************/
static void gpio_task(void *arg);

/******************************************************************
 * 6. Functions definitions
******************************************************************/

/**
 * @brief Main GPIO task.
 *
 * Initializes buttons and rotary encoder GPIOs, then loops:
 * - Reads rotary switch and encoder pins
 * - Detects state changes and sends events to event bus
 * - Logs actions for debugging
 *
 * @param[in] arg Task argument (unused)
 */
static void gpio_task(void *arg)
{
    static const char GPIO_TASK_TAG[] = "GPIO_TASK";
    (void)arg;

    /* Add task watchdog */
    esp_task_wdt_add(NULL);

    my_gpio_btn_t rotaryEncoderSwitch = {
        .pin = GPIO_NUM_3,
        .pull = MY_GPIO_PULL_NONE,
        .debounce_ms = 50
    };

    my_gpio_btn_t rotaryEncoderChanB = {
        .pin = GPIO_NUM_4,
        .pull = MY_GPIO_PULL_NONE,
        .debounce_ms = 50
    };

    my_gpio_btn_t rotaryEncoderChanA = {
        .pin = GPIO_NUM_5,
        .pull = MY_GPIO_PULL_NONE,
        .debounce_ms = 50
    };

    if (my_gpio_init(&rotaryEncoderSwitch) != ESP_OK) {
        ESP_LOGE(GPIO_TASK_TAG, "Failed to initialize rotaryEncoderSwitch!");
    }

    if (my_gpio_init(&rotaryEncoderChanA) != ESP_OK) {
        ESP_LOGE(GPIO_TASK_TAG, "Failed to initialize rotaryEncoderChanA!");
    }

    if (my_gpio_init(&rotaryEncoderChanB) != ESP_OK) {
        ESP_LOGE(GPIO_TASK_TAG, "Failed to initialize rotaryEncoderChanB!");
    }

    /* Initialize state_last_ variables */
    button_state_t state_last_rotaryChanA = my_gpio_read_btn(&rotaryEncoderChanA);
    button_state_t state_last_rotaryChanB = my_gpio_read_btn(&rotaryEncoderChanB);
    button_state_t state_last_rotarySwitch = my_gpio_read_btn(&rotaryEncoderSwitch);

    while(1) {
        /* Reset watchdog */
        esp_task_wdt_reset();

        button_state_t state_rotarySwitch = my_gpio_read_btn(&rotaryEncoderSwitch);

        /* Avoid sending event when no changes */
        if (state_last_rotarySwitch != state_rotarySwitch) {

            event_bus_message_t evt_message;
            evt_message.type = EVT_CLOCK_GPIO_CONFIG;
            evt_message.payload_size = 3U;
            evt_message.payload[0U] = (buttons_type_t)BUTTON_ROTARY_SWITCH_1;
            evt_message.payload[1U] = rotaryEncoderSwitch.press_type;
            evt_message.payload[2U] = 0U;
            event_bus_publish(evt_message);

            /* if (state_rotarySwitch == (button_state_t)BUTTON_STATE_PRESS) {
                ESP_LOGI(GPIO_TASK_TAG, "Rotary switch pressed!");
            } else {
                ESP_LOGI(GPIO_TASK_TAG, "Rotary switch released");
            } */

            state_last_rotarySwitch = state_rotarySwitch;
        }

        button_state_t state_rotaryChanA = my_gpio_read_btn(&rotaryEncoderChanA);
        button_state_t state_rotaryChanB = my_gpio_read_btn(&rotaryEncoderChanB);
        rotary_encoder_event_t ev = process_rotary_encoder(state_last_rotaryChanA, state_last_rotaryChanB, state_rotaryChanA, state_rotaryChanB);
        if (ev != ROTARY_ENCODER_EVENT_NONE) {
            event_bus_message_t evt_message;
            evt_message.type = EVT_CLOCK_GPIO_CONFIG;
            evt_message.payload_size = 3U;
            evt_message.payload[0U] = (buttons_type_t)BUTTON_ROTARY_ENCODER;
            evt_message.payload[1U] = 0U;
            evt_message.payload[2U] = (uint8_t)ev;
            event_bus_publish(evt_message);

            //ESP_LOGI(GPIO_TASK_TAG, "Rotary encoder %s", (ev == ROTARY_ENCODER_EVENT_INCREMENT) ? "increment" : "decrement");
        }
        state_last_rotaryChanA = state_rotaryChanA;
        state_last_rotaryChanB = state_rotaryChanB;

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/**
 * @brief Start the GPIO task.
 *
 * Starts the `gpio_task`.
 */
void gpio_task_start(void)
{
    xTaskCreate(
        gpio_task,     /* Task function */
        "gpio_task",   /* Task name (for debugging) */
        4096,          /* Stack size in bytes */
        NULL,          /* Parameter passed to the task */
        3U,            /* Task priority */
        NULL           /* Task handle (optional) */
    );
}
