/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#ifdef STATIC_ANALYSIS
#include "../test/common/esp_stub.h"
#endif
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "clock_task.h"
#include "../display/display.h"
#include "../clock/clock.h"
#include "../gpio_task/gpio_task.h"
#include "../gpio_driver/gpio_driver.h"
#include "../rotary_encoder/rotary_encoder.h"
#include "esp_log.h"
#include "../config/config.h"

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/
#define CLOCK_MENU_CLOCK                (0U)
#define CLOCK_MENU_CONFIGURE_MINUTES    (1U)
#define CLOCK_MENU_CONFIGURE_HOURS      (2U)
#define CLOCK_PATTERN_MAX_STEP          (9U)

/******************************************************************
 * 3. Typedef definitions (simple typedef, then enum and structs)
******************************************************************/

/******************************************************************
 * 4. Variable definitions (static then global)
******************************************************************/
static const char CLOCK_TASK_TAG[] = "CLOCK_TASK";
static myclock_t clk;

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
 *
 * @param[in] arg Task argument (unused)
 */
static void clock_task(void *arg) { 

    bool dots = true;
    uint8_t pattern_step = 0U;
    (void)arg; 
    config_t config;
    bool in_pattern_mode = false;
    bool in_test_mode = false;
    esp_err_t ret = ESP_OK;

    clock_init(&clk, CONFIG_CLOCK_DEFAULT_HOURS, CONFIG_CLOCK_DEFAULT_MINUTES, CONFIG_CLOCK_DEFAULT_SECONDS);

    TickType_t lastTick = xTaskGetTickCount();
    const TickType_t tickPeriod = pdMS_TO_TICKS(1000);    // 1s
    const TickType_t displayPeriod = pdMS_TO_TICKS(50);   // 50ms

    while (ret == ESP_OK) {
        TickType_t now = xTaskGetTickCount();
        
        /* Get latest configuration */
        ret = config_get_copy(&config);
        if (ret == ESP_OK) {

            /* Every second */
            if ((now - lastTick) >= tickPeriod) {
                lastTick += tickPeriod;
                clock_tick(&clk);
                dots = !dots;

                if (clk.seconds == 0U) {
                    in_pattern_mode = true;
                    pattern_step = 0U;
                }
            }

            if (in_pattern_mode == false) {
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
            }

            if (in_test_mode == true) {
                display_set_time(12U, 34U, 56U, 1U, 1U);
            }
            else if (in_pattern_mode == true) {
                display_set_pattern_1(pattern_step);
                pattern_step++;

                if (pattern_step > CLOCK_PATTERN_MAX_STEP) {
                    pattern_step = 0U;
                    in_pattern_mode = false;
                }
            } else {
                display_set_time(clk.hours, clk.minutes, clk.seconds, dots, dots);
            }

            vTaskDelay(displayPeriod);
        } else {
            ESP_LOGE(CLOCK_TASK_TAG, "Unable to get configuration");
        }
    }

    /* Free task */
    vTaskDelete(NULL);
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
    button_event_t event;

    if (xQueueReceive(buttonQueue, &event, 0) == pdTRUE) {
        static uint8_t state = CLOCK_MENU_CLOCK;
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
                    else {
                        /* ROTARY_ENCODER_EVENT_NONE */    
                    }
                }
                else {
                    /* BUTTON_ROTARY_SWITCH_1 && BUTTON_SHORT_PRESS */
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
                    else {
                        /* ROTARY_ENCODER_EVENT_NONE */    
                    }
                }
                else {
                    /* BUTTON_ROTARY_SWITCH_1 && BUTTON_SHORT_PRESS */
                }
                break;
            default:
                state = CLOCK_MENU_CLOCK;
                break;
        }
    }
}

/**
 * @brief Start the clock task.
 *
 * Creates the FreeRTOS task `clock_task` to handle clock updates,
 * display refresh, and user input. Logs an error if task creation fails.
 */
void clock_task_start(void)
{
    /* Create clock task */
    BaseType_t ret = xTaskCreate(clock_task, "clock_task", configMINIMAL_STACK_SIZE, NULL, 2U, NULL);

    if (ret != pdPASS) {
        ESP_LOGE(CLOCK_TASK_TAG, "Failed to create clock_task");
    }
}

/**
 * @brief Apply time update from NTP.
 *
 * Reads payload and updates the clock state.
 */
void clock_ntp_config_callback(uint8_t* payload, uint8_t size)
{
    (void)payload;
    (void)size;

    /* Update with NTP */
    const myclock_t *upd = NULL;
    if ((payload != NULL) && (size == sizeof(upd))) {
        upd = (myclock_t *)payload;
        clock_init(&clk, upd->hours, upd->minutes, upd->seconds);
    }
    else {
        ESP_LOGW(CLOCK_TASK_TAG, "Invalid NTP payload or queue not initialized");
    }
}

/**
 * @brief Update clock menu/UI on user input.
 *
 * Reads the latest GPIOs and refreshes the clock menu if
 * NTP is disabled. Intended to be triggered by user input events
 * (rotary encoder, buttons).
 */
void clock_update_with_menu_callback(uint8_t* payload, uint8_t size)
{
    (void)payload;
    (void)size;
    esp_err_t result = ESP_OK;
    config_t config;

    /* Get latest configuration */
    result = config_get_copy(&config);
    if (result == ESP_OK) {
        
        /* If no NTP sync */
        if (config.ntp == 0U) {
            clock_menu(&clk);
        }
    }
    else {
        ESP_LOGE(CLOCK_TASK_TAG, "Failed to get configuration");
    }
}

/**
 * @brief Update clock state from configuration.
 *
 * Reads the latest configuration and updates the clock time if
 * NTP is disabled. Intended to be triggered by configuration change
 * events (boot, webserver update, etc.).
 */
void clock_update_from_config_callback(uint8_t* payload, uint8_t size)
{
    (void)payload;
    (void)size;
    esp_err_t result = ESP_OK;
    config_t config;

    /* Get latest configuration */
    result = config_get_copy(&config);
    if (result == ESP_OK) {
        
        /* If no NTP sync */
        if (config.ntp == 0U) {
            clock_init(&clk, config.time.hours, config.time.minutes, config.time.seconds);
        }
    }
    else {
        ESP_LOGE(CLOCK_TASK_TAG, "Failed to get configuration");
    }
}