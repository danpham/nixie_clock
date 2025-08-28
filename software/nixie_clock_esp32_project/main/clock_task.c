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
    uint8_t dots_state;

    clock_init(&clk, 0, 0, 0);  // Initialize time to 00:00:00

    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(1000); // 1 second

    while (1) {
        // Show time
        dots_state = dots ? 1 : 0;
        display_set_time(clk.hours, clk.minutes, clk.seconds, dots_state, dots_state);

        // Read buttons
        if (xQueueReceive(buttonQueue, &event, 0) == pdTRUE) {
            if (event.id == 1 && event.pressed) {
                clock_increment_hours(&clk);
            }
            else if (event.id == 2 && event.pressed) {
                clock_increment_minutes(&clk);
            }
        }

        clock_tick(&clk);  // Advance clock by one second

        // Toggle dots
        dots = !dots;

        // Wait exactly 1 second before next loop
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

// Function to start the clock task
void clock_task_start()
{
    // Create clock task
    xTaskCreate(clock_task, "clock_task", 2048, NULL, 5, NULL);
}
