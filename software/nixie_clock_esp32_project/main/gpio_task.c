#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gpio_task.h"
#include "gpio_driver.h"
#include "esp_log.h"

QueueHandle_t buttonQueue;

static const char *TAG = "GPIO_TASK";

static my_gpio_btn_t btn1 = {
    .pin = GPIO_NUM_3,
    .pull = MY_GPIO_PULL_DOWN,
    .debounce_ms = 50
};

static my_gpio_btn_t btn2 = {
    .pin = GPIO_NUM_4,
    .pull = MY_GPIO_PULL_DOWN,
    .debounce_ms = 50
};


// FreeRTOS task to handle GPIO reading
static void gpio_task(void *pvParameter)
{
    button_event_t event;
    my_gpio_init(&btn1); // Initialize the GPIO 4
    my_gpio_init(&btn2); // Initialize the GPIO 3

    while(1) {
        int state_btn1 = my_gpio_read_btn(&btn1);
        int state_btn2 = my_gpio_read_btn(&btn2);
        if (state_btn1 == 1) {
            event.id = 1;       // button 1
            event.pressed = 1;  // pressed
            xQueueSend(buttonQueue, &event, 0);
            ESP_LOGI(TAG, "Button 1 pressed!");
        } else {
            event.id = 1;       // button 1
            event.pressed = 0;  // released
            xQueueSend(buttonQueue, &event, 0);
            ESP_LOGI(TAG, "Button 1 released");
        }

        if (state_btn2 == 1) {
            event.id = 2;       // button 2
            event.pressed = 1;  // released
            xQueueSend(buttonQueue, &event, 0);
            ESP_LOGI(TAG, "Button 2 pressed!");
        } else {
            event.id = 2;       // button 2
            event.pressed = 0;  // released
            xQueueSend(buttonQueue, &event, 0);
            ESP_LOGI(TAG, "Button 2 released");
        }
        
        vTaskDelay(pdMS_TO_TICKS(50)); // Loop every 50 ms
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
