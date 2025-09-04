/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "display.h"
#include "clock.h"
#include "gpio_task.h"

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

/******************************************************************
 * 6. Functions definitions
******************************************************************/
void clock_task(void *arg) {
    clock_t clk;
    button_event_t event;
    bool dots = true;
    bool in_pattern = false;
    uint8_t pattern_step = 0;
    uint8_t pattern_loops = 0;

    clock_init(&clk, 0, 0, 0);

    TickType_t lastTick = xTaskGetTickCount();
    const TickType_t tickPeriod    = pdMS_TO_TICKS(1000); // 1s
    const TickType_t displayPeriod = pdMS_TO_TICKS(50);   // 50ms

    while (1) {
        TickType_t now = xTaskGetTickCount();

        // Read pushbuttons
        if (xQueueReceive(buttonQueue, &event, 0) == pdTRUE) {
            if (event.id == 1 && event.pressed)
            {
                clock_increment_hours(&clk);
            }
            else if (event.id == 2 && event.pressed)
            {
                clock_increment_minutes(&clk);
            }
        }

        // Every second
        if ((now - lastTick) >= tickPeriod) {
            lastTick += tickPeriod;
            clock_tick(&clk);
            dots = !dots;

            if (clk.seconds == 0) {
                in_pattern   = true;
                pattern_step = 0;
                pattern_loops = 0;
            }
        }

        // Every 50 ms ---
        if (in_pattern) {
            display_set_pattern_1(pattern_step);
            pattern_step++;

            if (pattern_step > 9) {
                pattern_step = 0;
                pattern_loops++;
            }

            // Stop after one pattern loop
            if (pattern_loops >= 1) {
                in_pattern = false;
            }
        } else {
            display_set_time(clk.hours, clk.minutes, clk.seconds, dots, dots);
        }

        vTaskDelay(displayPeriod);
    }
}

// Function to start the clock task
void clock_task_start()
{
    // Create clock task
    xTaskCreate(clock_task, "clock_task", 2048, NULL, 5, NULL);
}
