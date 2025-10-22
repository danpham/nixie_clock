#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gpio_task.h"
#include "gpio_driver.h"
#include "rotary_encoder.h"
#include "esp_log.h"

QueueHandle_t buttonQueue;

// FreeRTOS task to handle GPIO reading
static void gpio_task(void *pvParameter)
{
    const char *TAG = "GPIO_TASK";
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
    int8_t state_last_rotarySwitch = 0;
 
    if (my_gpio_init(&rotaryEncoderSwitch) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize rotaryEncoderSwitch!");
    }

    if (my_gpio_init(&rotaryEncoderChanA) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize rotaryEncoderChanA!");
    }

    if (my_gpio_init(&rotaryEncoderChanB) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize rotaryEncoderChanB!");
    }

    /* Initialize state_last_ variables */
    state_last_rotaryChanA = (int8_t)my_gpio_read_btn(&rotaryEncoderChanA);
    state_last_rotaryChanB = (int8_t)my_gpio_read_btn(&rotaryEncoderChanB);
    state_last_rotarySwitch = (int8_t)my_gpio_read_btn(&rotaryEncoderSwitch);

    while(1) {
        int8_t state_rotarySwitch = 0;
        int8_t state_rotaryChanA = 0;
   
        state_rotarySwitch = (int8_t)my_gpio_read_btn(&rotaryEncoderSwitch);

        /* Avoid sending event when no changes */
        if (state_last_rotarySwitch != state_rotarySwitch){
            if (state_rotarySwitch == (button_state_t)BUTTON_STATE_PRESS) {
                event.id = BUTTON_ROTARY_SWITCH_1;
                event.pressed = rotaryEncoderSwitch.press_type;
                xQueueSend(buttonQueue, &event, 0);
                ESP_LOGI(TAG, "Rotary switch pressed!");
            } else {
                event.id = BUTTON_ROTARY_SWITCH_1;
                event.pressed = rotaryEncoderSwitch.press_type;
                xQueueSend(buttonQueue, &event, 0);
                ESP_LOGI(TAG, "Rotary switch released");
            }
            state_last_rotarySwitch = state_rotarySwitch;
        }

        state_rotaryChanA = (int8_t)my_gpio_read_btn(&rotaryEncoderChanA);
        int8_t state_rotaryChanB = (int8_t)my_gpio_read_btn(&rotaryEncoderChanB);
        rotary_encoder_event_t ev = process_rotary_encoder(state_last_rotaryChanA, state_last_rotaryChanB, state_rotaryChanA, state_rotaryChanB);
        if (ev != ROTARY_ENCODER_EVENT_NONE) {
            event.id = BUTTON_ROTARY_ENCODER;
            event.updateValue = (uint8_t)ev;
            xQueueSend(buttonQueue, &event, 0);
            ESP_LOGI(TAG, "Rotary encoder %s", (ev == ROTARY_ENCODER_EVENT_INCREMENT) ? "increment" : "decrement");
        }

        state_last_rotaryChanA = state_rotaryChanA;
        state_last_rotaryChanB = state_rotaryChanB;

        vTaskDelay(pdMS_TO_TICKS(5)); // Loop every 5 ms
    }
}

void gpio_task_start(void)
{
    // Create queue: 10 events max, each of size button_event_t
    buttonQueue = xQueueCreate(10, sizeof(button_event_t));
    if (buttonQueue == NULL) {
        ESP_LOGE("GPIO_TASK", "Failed to create button queue!");
    } else {
        xTaskCreate(
            gpio_task,     // Task function
            "gpio_task",   // Task name (for debugging)
            2048,          // Stack size in bytes
            NULL,          // Parameter passed to the task
            5,             // Task priority
            NULL           // Task handle (optional)
        );
    }
}
