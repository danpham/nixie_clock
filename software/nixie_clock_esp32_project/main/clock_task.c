/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "display.h"
#include "clock.h"
#include "gpio_task.h"
#include "gpio_driver.h"

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/
#define MENU_CLOCK              0
#define MENU_CONFIGURE_MINUTES  1
#define MENU_CONFIGURE_HOURS    2
#define PATTERN_MAX_STEP        (9U)

/******************************************************************
 * 3. Typedef definitions (simple typedef, then enum and structs)
******************************************************************/

/******************************************************************
 * 4. Variable definitions (static then global)
******************************************************************/

/******************************************************************
 * 5. Functions prototypes (static only)
******************************************************************/
static void clock_menu(clock_t *clk, button_event_t *event);

/******************************************************************
 * 6. Functions definitions
******************************************************************/
void clock_task(void *arg) {
    clock_t clk;
    button_event_t event;
    bool dots = true;
    bool in_pattern = false;
    uint8_t pattern_step = 0;
    
    clock_init(&clk, 0, 0, 0);

    TickType_t lastTick = xTaskGetTickCount();
    const TickType_t tickPeriod    = pdMS_TO_TICKS(1000); // 1s
    const TickType_t displayPeriod = pdMS_TO_TICKS(50);   // 50ms

    while (1) {
        TickType_t now = xTaskGetTickCount();

        // Clock configuration menu
        clock_menu(&clk, &event);

        // Every second
        if ((now - lastTick) >= tickPeriod) {
            lastTick += tickPeriod;
            clock_tick(&clk);
            dots = !dots;

            if (clk.seconds == 0) {
                in_pattern = true;
                pattern_step = 0;
                pattern_loops = 0;
            }
        }

        // Triggered every minute
        if (in_pattern) {
            display_set_pattern_1(pattern_step);
            pattern_step++;

            if (pattern_step > PATTERN_MAX_STEP) {
                pattern_step = 0;
                in_pattern = false;
            }
        } else {
            display_set_time(clk.hours, clk.minutes, clk.seconds, dots, dots);
        }

        vTaskDelay(displayPeriod);
    }
}

// Menu
void clock_menu(clock_t *clk, button_event_t *event)
{
    static int state = MENU_CLOCK;

    switch (state)
    {
        case MENU_CLOCK:
            if (xQueueReceive(buttonQueue, event, 0) == pdTRUE) {
                if (event->id == BUTTON_ROTARY_SWITCH_1 && event->pressed == BUTTON_LONG_PRESS)
                {
                    state = MENU_CONFIGURE_MINUTES;
                }
            }
            return;
        case MENU_CONFIGURE_MINUTES:
            if (xQueueReceive(buttonQueue, event, 0) == pdTRUE) {
                if (event->id == BUTTON_ROTARY_SWITCH_1 && event->pressed == BUTTON_LONG_PRESS)
                {
                    state = MENU_CONFIGURE_HOURS;
                }
                else if (event->id == BUTTON_ROTARY_ENCODER && event->updateValue == (uint8_t)ROTARY_ENCODER_EVENT_INCREMENT)
                {
                    clock_increment_minutes(clk);
                    state = MENU_CONFIGURE_MINUTES;
                }
                else if (event->id == BUTTON_ROTARY_ENCODER && event->updateValue == (uint8_t)ROTARY_ENCODER_EVENT_DECREMENT)
                {
                    clock_decrement_minutes(clk);
                    state = MENU_CONFIGURE_MINUTES;
                }
            }
            return;
        case MENU_CONFIGURE_HOURS:
            if (xQueueReceive(buttonQueue, event, 0) == pdTRUE) {
                if (event->id == BUTTON_ROTARY_SWITCH_1 && event->pressed == BUTTON_LONG_PRESS)
                {
                    state = MENU_CLOCK;
                }
                else if (event->id == BUTTON_ROTARY_ENCODER && event->updateValue == ROTARY_ENCODER_EVENT_INCREMENT)
                {
                    clock_increment_hours(clk);
                    state = MENU_CONFIGURE_HOURS;
                }
                else if (event->id == BUTTON_ROTARY_ENCODER && event->updateValue == ROTARY_ENCODER_EVENT_DECREMENT)
                {
                    clock_decrement_hours(clk);
                    state = MENU_CONFIGURE_HOURS;
                }
            }
            return;
        default:
            state = MENU_CLOCK;
    }
}

// Function to start the clock task
void clock_task_start()
{
    // Create clock task
    xTaskCreate(clock_task, "clock_task", 2048, NULL, 5, NULL);
}
