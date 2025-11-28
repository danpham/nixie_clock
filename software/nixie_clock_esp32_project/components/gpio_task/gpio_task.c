/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#ifdef STATIC_ANALYSIS
#include "../test/common/esp_stub.h"
#endif
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gpio_task.h"
#include "../gpio_driver/gpio_driver.h"
#include "../rotary_encoder/rotary_encoder.h"
#include "esp_log.h"
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
QueueHandle_t buttonQueue;
static const char GPIO_TASK_TAG[] = "GPIO_TASK";

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
 * - Detects state changes and sends events to buttonQueue
 * - Logs actions for debugging
 *
 * @param[in] arg Task argument (unused)
 */
static void gpio_task(void *arg)
{
    (void)arg;

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
    button_event_t event;
    int8_t state_last_rotaryChanA = 0;
    int8_t state_last_rotaryChanB = 0;
    button_state_t state_last_rotarySwitch = 0;
    bool gpio_update = false;

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
    state_last_rotaryChanA = (int8_t)my_gpio_read_btn(&rotaryEncoderChanA);
    state_last_rotaryChanB = (int8_t)my_gpio_read_btn(&rotaryEncoderChanB);
    state_last_rotarySwitch = (int8_t)my_gpio_read_btn(&rotaryEncoderSwitch);

    while(1) {
        button_state_t state_rotarySwitch = 0;
        int8_t state_rotaryChanA = 0;
   
        state_rotarySwitch = (int8_t)my_gpio_read_btn(&rotaryEncoderSwitch);

        /* Avoid sending event when no changes */
        if (state_last_rotarySwitch != state_rotarySwitch){
            event.id = (buttons_type_t)BUTTON_ROTARY_SWITCH_1;
            event.pressed = rotaryEncoderSwitch.press_type;
            if (buttonQueue != NULL) {
                xQueueSend(buttonQueue, &event, 0);
                gpio_update = true;
                if (state_rotarySwitch == (button_state_t)BUTTON_STATE_PRESS) {
                    ESP_LOGI(GPIO_TASK_TAG, "Rotary switch pressed!");
                } else {
                    ESP_LOGI(GPIO_TASK_TAG, "Rotary switch released");
                }
            }
            else {
                ESP_LOGW(GPIO_TASK_TAG, "buttonQueue not initialized");
            }
            state_last_rotarySwitch = state_rotarySwitch;
        }

        state_rotaryChanA = (int8_t)my_gpio_read_btn(&rotaryEncoderChanA);
        int8_t state_rotaryChanB = (int8_t)my_gpio_read_btn(&rotaryEncoderChanB);
        rotary_encoder_event_t ev = process_rotary_encoder(state_last_rotaryChanA, state_last_rotaryChanB, state_rotaryChanA, state_rotaryChanB);
        if (ev != ROTARY_ENCODER_EVENT_NONE) {
            event.id = (buttons_type_t)BUTTON_ROTARY_ENCODER;
            event.updateValue = (uint8_t)ev;
            if (buttonQueue != NULL) {
                xQueueSend(buttonQueue, &event, 0);
                gpio_update = true;
            }
            else {
               ESP_LOGW(GPIO_TASK_TAG, "buttonQueue not initialized");
            }

            ESP_LOGI(GPIO_TASK_TAG, "Rotary encoder %s", (ev == ROTARY_ENCODER_EVENT_INCREMENT) ? "increment" : "decrement");
        }
        state_last_rotaryChanA = state_rotaryChanA;
        state_last_rotaryChanB = state_rotaryChanB;

        /* At least one io has been modified, send an event */
        if (gpio_update == true) {
            event_bus_message_t evt_message;
            evt_message.type = EVT_CLOCK_GPIO_CONFIG;
            evt_message.payload_size = 0U;
            event_bus_publish(evt_message);
            gpio_update = false;
        }

        vTaskDelay(pdMS_TO_TICKS(5)); // Loop every 5 ms
    }
}

/**
 * @brief Start the GPIO task and create the button queue.
 *
 * Creates a FreeRTOS queue to send button and rotary encoder events,
 * then starts the `gpio_task`.
 *
 * @note If queue creation fails, the task is not started.
 */
void gpio_task_start(void)
{
    /* Create queue: 10 events max, each of size button_event_t */
    buttonQueue = xQueueCreate(10, sizeof(button_event_t));
    if (buttonQueue == NULL) {
        ESP_LOGE(GPIO_TASK_TAG, "Failed to create button queue!");
    } else {
        xTaskCreate(
            gpio_task,     /* Task function */
            "gpio_task",   /* Task name (for debugging) */
            configMINIMAL_STACK_SIZE, /* Stack size in bytes */
            NULL,          /* Parameter passed to the task */
            3U,            /* Task priority */
            NULL           /* Task handle (optional) */
        );
    }
}
