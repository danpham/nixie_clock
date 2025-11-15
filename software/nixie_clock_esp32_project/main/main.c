/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include <string.h>
#include "driver/gpio.h"
#include "gpio_task.h"
#include "clock_task.h"
#include "ntp.h"
#include "esp_log.h"
#include "esp_stub.h"
#include "../components/uart/uart.h"
#include "../components/hv5622/hv5622.h"
#include "../components/display/display.h"
#include "../components/clock/clock.h"
#include "../components/webserver/webserver.h"
#include "../components/config/config.h"

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
void app_main(void);

/* MISRA C:2012 Rule 8.9 deviation:
   app_main() is called externally by FreeRTOS/ESP-IDF runtime */
// cppcheck-suppress unusedFunction
void app_main(void)
{
    static const char MAIN_TAG[] = "MAIN";
    const char hello[] = "Nixie clock v1.0: Starting...";
    esp_err_t ret = ESP_FAIL;
    size_t len = sizeof(hello) - 1U;

    uart_init();
    uart_write(hello, len);

    ret = config_init();
    if (ret != ESP_OK) {
        ESP_LOGE(MAIN_TAG, "Config init failed: %d", ret);
    }

    hv5622_init();
    display_init();

    clock_task_start();
    gpio_task_start();

    ret = config_apply();
    if (ret != ESP_OK) {
        ESP_LOGE(MAIN_TAG, "Config apply failed: %d", ret);
    }

    start_webserver();

    while(1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}