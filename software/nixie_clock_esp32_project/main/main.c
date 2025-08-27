/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "uart.h"
#include "hv5622.h"
#include "display.h"
#include "clock.h"
#include "gpio_driver.h"


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
extern void clock_task(void *arg);


/******************************************************************
 * 6. Functions definitions
******************************************************************/
void app_main(void)
{
    hv5622_init();
    display_init();

    clock_task_start();
    gpio_task_start();

    while(1) {
        vTaskDelay(pdMS_TO_TICKS(1000)); // Main loop remains free
    }
}
