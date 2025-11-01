
/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include "wifi.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "../../main/esp_stub.h"

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/

/******************************************************************
 * 3. Typedef definitions (simple typedef, then enum and structs)
******************************************************************/

/******************************************************************
 * 4. Variable definitions (static then global)
******************************************************************/
static void (*wifi_on_got_ip_cb)(void) = NULL;

/******************************************************************
 * 5. Functions prototypes (static only)
******************************************************************/

/******************************************************************
 * 6. Functions definitions
******************************************************************/

void wifi_register_on_got_ip_callback(void (*cb)(void))
{
    wifi_on_got_ip_cb = cb;
}

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    const char *TAG = "wifi";
    (void)arg;

    if ((event_base == WIFI_EVENT) && (event_id == WIFI_EVENT_STA_START)) {
        esp_wifi_connect();
    } else if ((event_base == WIFI_EVENT) && (event_id == WIFI_EVENT_STA_DISCONNECTED)) {
        ESP_LOGI(TAG, "Wi-Fi déconnecté, reconnexion...");
        esp_wifi_connect();
    } else if ((event_base == IP_EVENT) && (event_id == IP_EVENT_STA_GOT_IP)) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "IP: " IPSTR, IP2STR(&event->ip_info.ip));

        if (NULL != wifi_on_got_ip_cb) {
            wifi_on_got_ip_cb();
        }
    }
    else {
        /* Do nothing */ 
    }
}

void wifi_init_sta(const char *ssid, const char *password)
{
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;

    esp_event_handler_instance_register(WIFI_EVENT,
                                        ESP_EVENT_ANY_ID,
                                        &wifi_event_handler,
                                        NULL,
                                        &instance_any_id);

    esp_event_handler_instance_register(IP_EVENT,
                                        IP_EVENT_STA_GOT_IP,
                                        &wifi_event_handler,
                                        NULL,
                                        &instance_got_ip);

    wifi_config_t wifi_config = { 0 };
    (void)strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
    (void)strncpy((char *)wifi_config.sta.password, password, sizeof(wifi_config.sta.password));

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();
}
