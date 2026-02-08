/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#ifdef STATIC_ANALYSIS
#include "../test/common/esp_stub.h"
#endif
#include <string.h>
#include "esp_task_wdt.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "../components/ntp/ntp.h"
#include "../components/display/display.h"
#include "../components/clock/clock.h"
#include "../components/webserver/webserver.h"
#include "../components/config/config.h"
#include "../components/pwm/pwm.h"
#include "../components/dispatcher_task/dispatcher_task.h"
#include "../components/event_bus/event_bus.h"
#include "../components/wifi/wifi.h"
#include "../components/clock_task/clock_task.h"
#include "../components/gpio_task/gpio_task.h"

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/
#define MAIN_TASK_WDT_TIMEOUT_MS     5000U

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
    //const char hello[] = "Nixie clock v1.0: Starting...";
    //size_t len = sizeof(hello) - 1U;
    esp_err_t ret = ESP_OK;
    esp_task_wdt_config_t wdt_config = {
        .timeout_ms = MAIN_TASK_WDT_TIMEOUT_MS,
        .idle_core_mask = 0x01,   /* Only Core 0 */
        .trigger_panic  = true
    };

    /* Setup task watchdog */
    esp_task_wdt_init(&wdt_config);
    esp_task_wdt_add(NULL);

    event_bus_init();
    dispatcher_subscribe(EVT_NTP_CONFIG, ntp_callback);
    dispatcher_subscribe(EVT_WIFI_CONFIG, wifi_callback);
    dispatcher_subscribe(EVT_PWM_CONFIG, pwm_callback);
    dispatcher_subscribe(EVT_CLOCK_NTP_CONFIG, clock_ntp_config_callback);
    dispatcher_subscribe(EVT_CLOCK_GPIO_CONFIG, clock_update_with_menu_callback);
    dispatcher_subscribe(EVT_CLOCK_WEB_CONFIG, clock_update_from_config_callback);

    pwm_init();
    dispatcher_task_start();

    ret = config_init();
    if (ret != ESP_OK) {
        ESP_LOGE(MAIN_TAG, "Config init failed");
    }

    start_webserver();
    display_init();
    clock_task_start();
    gpio_task_start();

    while (ret == ESP_OK) {
        esp_task_wdt_reset();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    /* Free task */
    vTaskDelete(NULL);
}