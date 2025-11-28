/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#ifdef STATIC_ANALYSIS
#include "../test/common/esp_stub.h"
#endif
#include <string.h>
#include "driver/gpio.h"
#include "../components/ntp/ntp.h"
#include "esp_log.h"
#include "../components/uart/uart.h"
#include "../components/hv5622/hv5622.h"
#include "../components/display/display.h"
#include "../components/clock/clock.h"
#include "../components/webserver/webserver.h"
#include "../components/config/config.h"
#include "../components/pwm/pwm.h"
#include "../components/dispatcher/dispatcher.h"
#include "../components/event_bus/event_bus.h"
#include "../components/wifi/wifi.h"
#include "../components/clock_task/clock_task.h"
#include "../components/gpio_task/gpio_task.h"

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

    event_bus_init();
    dispatcher_subscribe(EVT_NTP_CONFIG, ntp_callback);
    dispatcher_subscribe(EVT_WIFI_CONFIG, wifi_callback);
    dispatcher_subscribe(EVT_PWM_CONFIG, pwm_callback);
    dispatcher_subscribe(EVT_CLOCK_NTP_CONFIG, clock_ntp_config_callback);
    dispatcher_subscribe(EVT_CLOCK_GPIO_CONFIG, clock_update_with_menu_callback);
    dispatcher_subscribe(EVT_CLOCK_WEB_CONFIG, clock_update_from_config_callback);
    dispatcher_task_start();

    uart_init();
    uart_write(hello, len);

    ret = config_init();
    if (ret != ESP_OK) {
        ESP_LOGE(MAIN_TAG, "Config init failed");
    }

    hv5622_init();
    display_init();

    clock_task_start();
    gpio_task_start();

    pwm_init();
    start_webserver();
    
    /* Read and apply config with events */
    event_bus_message_t evt_message;
    evt_message.type = EVT_NTP_CONFIG;
    evt_message.payload_size = 0U;
    event_bus_publish(evt_message);
    evt_message.type = EVT_WIFI_CONFIG;
    event_bus_publish(evt_message);
    evt_message.type = EVT_PWM_CONFIG;
    event_bus_publish(evt_message);

    while(ret == ESP_OK) {
        vTaskDelay(pdMS_TO_TICKS(portMAX_DELAY));
    }

    /* Free task */
    vTaskDelete(NULL);
}