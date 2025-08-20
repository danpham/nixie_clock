/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "display.h"
#include "clock.h"


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
    clock_init(&clk, 0, 0, 0);  // Initialize time to 00:00:00

    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(1000); // 1 second

    while (1) {
        clock_tick(&clk);  // Advance clock by one second
        display_set_time(clk.hours, clk.minutes, clk.seconds);

        // Wait exactly 1 second before next loop
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}
