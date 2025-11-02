/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include <string.h>
#include "driver/gpio.h"
#include "gpio_task.h"
#include "clock_task.h"
#include "ntp_sync_task.h"
#include "../components/uart/uart.h"
#include "../components/hv5622/hv5622.h"
#include "../components/display/display.h"
#include "../components/clock/clock.h"
#include "../components/wifi/wifi.h"

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/
#define WIFI_SSID       "YourSSID"
#define WIFI_PASSWORD   "MyPassword"

/******************************************************************
 * 3. Typedef definitions (simple typedef, then enum and structs)
******************************************************************/

/******************************************************************
 * 4. Variable definitions (static then global)
******************************************************************/

/******************************************************************
 * 5. Functions prototypes (static only)
******************************************************************/
static void on_wifi_ready(void);

/******************************************************************
 * 6. Functions definitions
******************************************************************/
void app_main(void);

/* MISRA C:2012 Rule 8.9 deviation:
   app_main() is called externally by FreeRTOS/ESP-IDF runtime */
// cppcheck-suppress unusedFunction
void app_main(void)
{
    const char hello[] = "Nixie clock v1.0: Starting...";
    size_t len = sizeof(hello) - 1U;

    uart_init();
    uart_write(hello, len);

    hv5622_init();
    display_init();

    clock_task_start();
    gpio_task_start();

    /* Time syncrhonisation */
    wifi_init_sta(WIFI_SSID, WIFI_PASSWORD);

    while(1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}