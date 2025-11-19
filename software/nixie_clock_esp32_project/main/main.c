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
#include "../components/pwm/pwm.h"
#include "../components/service_manager/service_manager.h"

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
    size_t len = sizeof(hello) - 1U;
    esp_err_t ret = ESP_OK;

    uart_init();
    uart_write(hello, len);

    esp_err_t cfg_ret = config_init();
    if (cfg_ret != ESP_OK) {
        ESP_LOGE(MAIN_TAG, "Config init failed");
    }

    hv5622_init();
    display_init();

    clock_task_start();
    gpio_task_start();

    pwm_init();

    esp_err_t svc_ret = service_manager_update();
    if (svc_ret != ESP_OK) {
        ESP_LOGE(MAIN_TAG, "Service manager unable to apply config");
    }

    start_webserver();

    /* Combine both results */
    if (cfg_ret != ESP_OK || svc_ret != ESP_OK) {
        ret = ESP_FAIL;
    } else {
        ret = ESP_OK;
    }

    while(ret == ESP_OK) {
        vTaskDelay(pdMS_TO_TICKS(portMAX_DELAY));
    }

    /* Free task */
    vTaskDelete(NULL);
}