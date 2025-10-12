#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gpio_task.h"
#include "gpio_driver.h"
#include "esp_log.h"

QueueHandle_t buttonQueue;

static const char *TAG = "GPIO_TASK";

static my_gpio_btn_t rotaryEncoderSwitch = {
    .pin = GPIO_NUM_3,
    .pull = MY_GPIO_PULL_NONE,
    .debounce_ms = 50
};

static my_gpio_btn_t rotaryEncoderChanB = {
    .pin = GPIO_NUM_4,
    .pull = MY_GPIO_PULL_NONE,
    .debounce_ms = 50
};

static my_gpio_btn_t rotaryEncoderChanA = {
    .pin = GPIO_NUM_5,
    .pull = MY_GPIO_PULL_NONE,
    .debounce_ms = 50
};

// FreeRTOS task to handle GPIO reading
static void gpio_task(void *pvParameter)
{
    button_event_t event;
    button_event_t lastState;

    int state_rotarySwitch = 0;
    int state_rotaryChanA = 0;
    int state_last_rotaryChanA = 0;
    int state_last_rotarySwitch = 0;
    int state_rotaryChanB = 0;

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
    state_last_rotaryChanA = my_gpio_read_btn(&rotaryEncoderChanA);
    state_last_rotarySwitch = my_gpio_read_btn(&rotaryEncoderSwitch);

    while(1) {
        state_rotarySwitch = my_gpio_read_btn(&rotaryEncoderSwitch);

        /* Avoid sending event when no changes */
        if (state_last_rotarySwitch != state_rotarySwitch){
            if (state_rotarySwitch == BUTTON_STATE_PRESS) {
                event.id = BUTTON_ROTARY_SWITCH_1;
                event.pressed = rotaryEncoderSwitch.press_type;
                xQueueSend(buttonQueue, &event, 0);
                ESP_LOGI(TAG, "Rotary swtich pressed!");
            } else {
                event.id = BUTTON_ROTARY_SWITCH_1;
                event.pressed = rotaryEncoderSwitch.press_type;
                xQueueSend(buttonQueue, &event, 0);
                ESP_LOGI(TAG, "Rotary swtich released");
            }
            state_last_rotarySwitch = state_rotarySwitch;
        }

        state_rotaryChanA = my_gpio_read_btn(&rotaryEncoderChanA);
        if (state_rotaryChanA != state_last_rotaryChanA)
        {
            state_rotaryChanB = my_gpio_read_btn(&rotaryEncoderChanB);
            if (state_rotaryChanA != state_rotaryChanB)
            {
                event.id = BUTTON_ROTARY_ENCODER;
                event.updateValue = ROTARY_ENCODER_EVENT_INCREMENT;
                xQueueSend(buttonQueue, &event, 0);
                ESP_LOGI(TAG, "Rotary encoder increment");
            } else {
                event.id = BUTTON_ROTARY_ENCODER;
                event.updateValue = ROTARY_ENCODER_EVENT_DECREMENT;
                xQueueSend(buttonQueue, &event, 0);
                ESP_LOGI(TAG, "Rotary encoder decrement");
            }
        }
        state_last_rotaryChanA = state_rotaryChanA;
        
        vTaskDelay(pdMS_TO_TICKS(5)); // Loop every 5 ms
    }
}

void gpio_task_start()
{
    // Create queue: 10 events max, each of size button_event_t
    buttonQueue = xQueueCreate(10, sizeof(button_event_t));
    if (buttonQueue == NULL) {
        ESP_LOGE("GPIO_TASK", "Failed to create button queue!");
        return;
    }

    xTaskCreate(
        gpio_task,     // Task function
        "gpio_task",   // Task name (for debugging)
        2048,          // Stack size in bytes
        NULL,          // Parameter passed to the task
        5,             // Task priority
        NULL           // Task handle (optional)
    );
}
