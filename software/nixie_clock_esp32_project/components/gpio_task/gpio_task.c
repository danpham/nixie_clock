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
#include "esp_timer.h"

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/
#define GPIOTASK_ENCODER_QUEUE_SIZE        (50U)

/******************************************************************
 * 3. Typedef definitions (simple typedef, then enum and structs)
******************************************************************/
typedef struct {
    button_state_t a;
    button_state_t b;
} gpio_event_t;

/******************************************************************
 * 4. Variable definitions (static then global)
******************************************************************/
static my_gpio_btn_t rotaryEncoderChanA = {
    .pin = GPIO_NUM_5,
    .pull = MY_GPIO_PULL_NONE,
    .debounce_ms = 10,
    .intr_type = GPIO_INTR_ANYEDGE,
    .isr_handler = NULL,
 };

 static my_gpio_btn_t rotaryEncoderChanB = {
    .pin = GPIO_NUM_4,
    .pull = MY_GPIO_PULL_NONE,
    .debounce_ms = 10,
    .intr_type = GPIO_INTR_ANYEDGE,
    .isr_handler = NULL,
};

/******************************************************************
 * 5. Functions prototypes (static only)
******************************************************************/
static void gpio_task(void *arg);
static QueueHandle_t gpio_evt_queue = NULL;

/******************************************************************
 * 6. Functions definitions
******************************************************************/

/**
 * @brief ISR for rotary encoder channels A and B.
 *
 * Debounces input, rejects duplicate states, and sends valid events to a queue.
 *
 * @param[in] arg Unused argument.
 */
static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    (void)arg;
    static uint8_t previous_state = 0xFFU;

    gpio_event_t evt;
    evt.a = gpio_get_level(rotaryEncoderChanA.pin);
    evt.b = gpio_get_level(rotaryEncoderChanB.pin);

    uint8_t state = (evt.a << 1) | evt.b;

    if (state != previous_state) {
        previous_state = state;
        if (gpio_evt_queue != NULL) {
            xQueueSendFromISR(gpio_evt_queue, &evt, NULL);
        }
    }
}

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

    my_gpio_btn_t rotaryEncoderSwitch = {
        .pin = GPIO_NUM_3,
        .pull = MY_GPIO_PULL_UP,
        .debounce_ms = 50,
        .intr_type = GPIO_INTR_DISABLE,
        .isr_handler = NULL,
    };

    /* Add task watchdog */
    esp_task_wdt_add(NULL);

    /* Set isr handler after its defined */
    rotaryEncoderChanA.isr_handler = gpio_isr_handler;
    rotaryEncoderChanB.isr_handler = gpio_isr_handler;

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

    /* Create queue */
    gpio_evt_queue = xQueueCreate(GPIOTASK_ENCODER_QUEUE_SIZE, sizeof(gpio_event_t));
    if (gpio_evt_queue == NULL) {
        ESP_LOGE(GPIO_TASK_TAG, "Failed to create GPIO event queue!");
    }

    while(1) {
        /* Reset watchdog */
        esp_task_wdt_reset();

        button_state_t state_rotarySwitch = my_gpio_read_btn(&rotaryEncoderSwitch);

        /* Avoid sending event when no changes */
        if (state_last_rotarySwitch != state_rotarySwitch) {

            event_bus_message_t evt_message;
            evt_message.type = EVT_CLOCK_GPIO_CONFIG;
            evt_message.payload_size = 5U;
            evt_message.payload[0U] = (buttons_type_t)BUTTON_ROTARY_SWITCH_1;
            evt_message.payload[1U] = (button_press_t)rotaryEncoderSwitch.press_type;
            evt_message.payload[2U] = (button_state_t)state_rotarySwitch;
            evt_message.payload[3U] = 0U;
            evt_message.payload[4U] = 0U;
            event_bus_publish(evt_message);

            state_last_rotarySwitch = state_rotarySwitch;
        }

        gpio_event_t evt_encoder;
        static int8_t accumulator = 0;

        /* Drain the queue and accumulate */
        while (xQueueReceive(gpio_evt_queue, &evt_encoder, 0) == pdTRUE)
        {
            rotary_encoder_event_t ev = process_rotary_encoder(
                state_last_rotaryChanA, state_last_rotaryChanB,
                evt_encoder.a, evt_encoder.b);

            if (ev == ROTARY_ENCODER_EVENT_INCREMENT) {
                accumulator++;
            } else if (ev == ROTARY_ENCODER_EVENT_DECREMENT) {
                accumulator--;
            } else {
                /* ROTARY_ENCODER_EVENT_NONE */
            }
            state_last_rotaryChanA = evt_encoder.a;
            state_last_rotaryChanB = evt_encoder.b;
        }

        int8_t steps = accumulator / 4;
        accumulator = accumulator % 4;

        if (steps > 0) {
            ESP_LOGI(GPIO_TASK_TAG, "ROTARY_ENCODER_EVENT_INCREMENT: %d steps", steps);
            event_bus_message_t evt_message;
            evt_message.type = EVT_CLOCK_GPIO_CONFIG;
            evt_message.payload_size = 5U;
            evt_message.payload[0U] = (buttons_type_t)BUTTON_ROTARY_ENCODER;
            evt_message.payload[1U] = 0U;
            evt_message.payload[2U] = (button_state_t)BUTTON_STATE_RELEASE;
            evt_message.payload[3U] = (uint8_t)ROTARY_ENCODER_EVENT_INCREMENT;
            evt_message.payload[4U] = (uint8_t)steps;
            event_bus_publish(evt_message);
        } else if (steps < 0) {
            ESP_LOGI(GPIO_TASK_TAG, "ROTARY_ENCODER_EVENT_DECREMENT: %d steps", -steps);
            event_bus_message_t evt_message;
            evt_message.type = EVT_CLOCK_GPIO_CONFIG;
            evt_message.payload_size = 5U;
            evt_message.payload[0U] = (buttons_type_t)BUTTON_ROTARY_ENCODER;
            evt_message.payload[1U] = 0U;
            evt_message.payload[2U] = (button_state_t)BUTTON_STATE_RELEASE;
            evt_message.payload[3U] = (uint8_t)ROTARY_ENCODER_EVENT_DECREMENT;
            evt_message.payload[4U] = (uint8_t)(-steps);    
            event_bus_publish(evt_message);
        } else {
            /* steps == 0 */
        }

        vTaskDelay(pdMS_TO_TICKS(10));
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
