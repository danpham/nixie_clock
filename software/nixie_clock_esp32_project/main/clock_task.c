/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "clock_task.h"
#include "display.h"
#include "clock.h"
#include "gpio_task.h"
#include "../components/gpio_driver/gpio_driver.h"
#include "../components/rotary_encoder/rotary_encoder.h"
#include "esp_stub.h"
#include "esp_log.h"
#include "config.h"

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/
#define CLOCK_MENU_CLOCK                (0U)
#define CLOCK_MENU_CONFIGURE_MINUTES    (1U)
#define CLOCK_MENU_CONFIGURE_HOURS      (2U)
#define CLOCK_PATTERN_MAX_STEP          (9U)
#define CLOCK_QUEUE_SIZE                (10U)

/******************************************************************
 * 3. Typedef definitions (simple typedef, then enum and structs)
******************************************************************/

/******************************************************************
 * 4. Variable definitions (static then global)
******************************************************************/
static const char CLOCK_TASK_TAG[] = "CLOCK_TASK";
QueueHandle_t clockUpdateQueue;

/******************************************************************
 * 5. Functions prototypes (static only)
******************************************************************/
static void clock_menu(myclock_t *clk);
static void clock_task(void *arg);

/******************************************************************
 * 6. Functions definitions
******************************************************************/

/**
 * @brief Main clock task.
 *
 * Runs in a FreeRTOS task. Handles:
 * - Clock ticking every second
 * - Display updates
 * - User input via clock_menu()
 * - Receiving updated time from clockUpdateQueue
 *
 * @param[in] arg Task argument (unused)
 */
static void clock_task(void *arg) { 
    myclock_t clk;
    bool dots = true;
    bool in_pattern_mode = false;
    bool in_test_mode = false;
    uint8_t pattern_step = 0;
    (void)arg; 
    config_t config;

    clock_init(&clk, CLOCK_DEFAULT_HOURS, CLOCK_DEFAULT_MINUTES, CLOCK_DEFAULT_SECONDS);

    TickType_t lastTick = xTaskGetTickCount();
    const TickType_t tickPeriod = pdMS_TO_TICKS(1000);    // 1s
    const TickType_t displayPeriod = pdMS_TO_TICKS(50);   // 50ms

    while (1) {
        TickType_t now = xTaskGetTickCount();

        /* Get latest configuration */
        config_set_config(&config);

        /* Clock configuration menu */
        clock_menu(&clk);

        /* Every second */
        if ((now - lastTick) >= tickPeriod) {

            /* Update with NTP or webserver config */
            if (clockUpdateQueue != NULL) {
                myclock_t upd;
                if (xQueueReceive(clockUpdateQueue, &upd, 0) == pdPASS) {
                    clock_init(&clk, upd.hours, upd.minutes, upd.seconds);
                }
            }
            else {
                ESP_LOGW(CLOCK_TASK_TAG, "clockUpdateQueue not initialized");
            }

            lastTick += tickPeriod;
            clock_tick(&clk);
            dots = !dots;

            if (clk.seconds == 0) {
                in_pattern_mode = true;
                pattern_step = 0;
            }
        }

        /* Mode selection */
        if (config.mode == (uint8_t)CONFIG_MODE_ANTIPOISONING){
            in_pattern_mode = true;
            in_test_mode = false;
        }
        else if (config.mode == (uint8_t)CONFIG_MODE_TEST){
            in_pattern_mode = false;
            in_test_mode = true;
        }
        else { /* Clock mode */
            in_pattern_mode = false;
            in_test_mode = false;
        }

        if (in_test_mode) {
            display_set_time(12, 34, 56, 1, 1);
        }
        else if (in_pattern_mode) {
            display_set_pattern_1(pattern_step);
            pattern_step++;

            if (pattern_step > CLOCK_PATTERN_MAX_STEP) {
                pattern_step = 0;
                in_pattern_mode = false;
            }
        } else {
            display_set_time(clk.hours, clk.minutes, clk.seconds, dots, dots);
        }

        vTaskDelay(displayPeriod);
    }
}

/**
 * @brief Handle clock configuration menu.
 *
 * Reads button and rotary encoder events from buttonQueue and
 * updates the clock structure accordingly.
 *
 * Menu states:
 * - CLOCK_MENU_CLOCK: default view
 * - CLOCK_MENU_CONFIGURE_MINUTES: adjust minutes
 * - CLOCK_MENU_CONFIGURE_HOURS: adjust hours
 *
 * @param[in,out] clk Pointer to the clock structure.
 */
void clock_menu(myclock_t *clk)
{
    static uint8_t state = CLOCK_MENU_CLOCK;
    button_event_t event;

    if (xQueueReceive(buttonQueue, &event, 0) == pdTRUE) {
        switch (state) {
            case CLOCK_MENU_CLOCK:
                if ((event.id == BUTTON_ROTARY_SWITCH_1) && (event.pressed == BUTTON_LONG_PRESS)) {
                    state = CLOCK_MENU_CONFIGURE_MINUTES;
                }
        
                break;
            case CLOCK_MENU_CONFIGURE_MINUTES:
                if ((event.id == BUTTON_ROTARY_SWITCH_1) && (event.pressed == BUTTON_LONG_PRESS)) {
                    state = CLOCK_MENU_CONFIGURE_HOURS;
                }
                else if (event.id == BUTTON_ROTARY_ENCODER) {
                    if (event.updateValue == ROTARY_ENCODER_EVENT_INCREMENT) {
                        clock_increment_minutes(clk);
                    }
                    else if (event.updateValue == ROTARY_ENCODER_EVENT_DECREMENT) {
                        clock_decrement_minutes(clk);
                    }
                }
                break;
            case CLOCK_MENU_CONFIGURE_HOURS:
                if ((event.id == BUTTON_ROTARY_SWITCH_1) && (event.pressed == BUTTON_LONG_PRESS)) {
                    state = CLOCK_MENU_CLOCK;
                }
                else if (event.id == BUTTON_ROTARY_ENCODER) {
                    if (event.updateValue == ROTARY_ENCODER_EVENT_INCREMENT) {
                        clock_increment_hours(clk);
                    }
                    else if (event.updateValue == ROTARY_ENCODER_EVENT_DECREMENT) {
                        clock_decrement_hours(clk);
                    }
                }
                break;
            default:
                state = CLOCK_MENU_CLOCK;
                break;
        }
    }
}

/**
 * @brief Start the clock task and initialize the update queue.
 *
 * Creates the FreeRTOS queue `clockUpdateQueue` for receiving
 * clock updates (from NTP or other tasks), then creates
 * the `clock_task` FreeRTOS task.
 *
 * @note If the queue creation fails, the task is not started.
 */
void clock_task_start(void)
{
    /* Create queue: 10 events max, each of size myclock_t */
    clockUpdateQueue = xQueueCreate(CLOCK_QUEUE_SIZE, sizeof(myclock_t));
    if (clockUpdateQueue == NULL) {
        ESP_LOGE(CLOCK_TASK_TAG, "Failed to create myclock queue!");
    } else {
        /* Create clock task */
        BaseType_t ret = xTaskCreate(clock_task, "clock_task", configMINIMAL_STACK_SIZE, NULL, 2U, NULL);

        if (ret != pdPASS) {
            ESP_LOGE(CLOCK_TASK_TAG, "Failed to create clock_task");
        }
    }
}
