
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
static const char *WIFI_TAG = "wifi";

/******************************************************************
 * 5. Functions prototypes (static only)
******************************************************************/

/******************************************************************
 * 6. Functions definitions
******************************************************************/

/**
 * @brief Wi-Fi and IP event handler
 *
 * Handles the following events:
 * - WIFI_EVENT_STA_START: starts STA connection
 * - WIFI_EVENT_STA_DISCONNECTED: auto-reconnect
 * - IP_EVENT_STA_GOT_IP: prints the obtained IP address
 * - WIFI_EVENT_AP_STACONNECTED: logs a connected AP client
 * - WIFI_EVENT_AP_STADISCONNECTED: logs a disconnected AP client
 *
 * @param arg User argument (unused)
 * @param event_base Event base (WIFI_EVENT or IP_EVENT)
 * @param event_id Event ID
 * @param event_data Pointer to event-specific data
 */
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    (void)arg;


    if ((event_base == WIFI_EVENT) && (event_id == WIFI_EVENT_STA_START)) {
        ESP_LOGI(WIFI_TAG, "Wi‑Fi STA started, connecting...");
        esp_wifi_connect();
    } else if ((event_base == WIFI_EVENT) && (event_id == WIFI_EVENT_STA_DISCONNECTED)) {
        ESP_LOGI(WIFI_TAG, "Wi‑Fi STA disconnected, reconnecting…");
        esp_wifi_connect();
    } else if ((event_base == IP_EVENT) && (event_id == IP_EVENT_STA_GOT_IP)) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(WIFI_TAG, "IP: " IPSTR, IP2STR(&event->ip_info.ip));
    }
    else if ((event_base == WIFI_EVENT) && (event_id == WIFI_EVENT_AP_STACONNECTED)) {
        wifi_event_ap_staconnected_t* e = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(WIFI_TAG, "AP client connected: MAC=%02x:%02x:%02x:%02x:%02x:%02x, AID=%d",
                    e->mac[0], e->mac[1], e->mac[2],
                    e->mac[3], e->mac[4], e->mac[5],
                    e->aid);
    }
    else if ((event_base == WIFI_EVENT) && (event_id == WIFI_EVENT_AP_STADISCONNECTED)) {
        wifi_event_ap_stadisconnected_t* e = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(WIFI_TAG, "AP client disconnected: MAC=%02x:%02x:%02x:%02x:%02x:%02x, AID=%d",
                    e->mac[0], e->mac[1], e->mac[2],
                    e->mac[3], e->mac[4], e->mac[5],
                    e->aid);
    }
    else {
        /* Do nothing */ 
    }
}

/**
 * @brief Registers Wi-Fi and IP event handlers
 *
 * @return ESP_OK on success, otherwise returns ESP-IDF error code
 */
static esp_err_t wifi_register_event_handlers(void)
{
    esp_err_t ret = ESP_OK;

    ret = esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                              &wifi_event_handler, NULL, NULL);
    if (ret != ESP_OK) {
        ESP_LOGE(WIFI_TAG, "WIFI_EVENT handler register failed: %s", esp_err_to_name(ret));
    }

    if (ret == ESP_OK) {
        ret = esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                                  &wifi_event_handler, NULL, NULL);
        if (ret != ESP_OK) {
            ESP_LOGE(WIFI_TAG, "IP_EVENT handler register failed: %s", esp_err_to_name(ret));
        }
    }

    return ret;
}

/**
 * @brief Initializes Wi-Fi in AP+STA mode
 *
 * Configures and starts Wi-Fi with:
 * - Station (STA) mode to connect to an existing network
 * - Access Point (AP) mode for direct connections
 *
 * @param sta_ssid SSID of the station Wi-Fi network
 * @param sta_password Password of the station Wi-Fi network
 * @param ap_ssid SSID of the local access point
 * @param ap_password Password of the local access point
 */
void wifi_init_apsta(const char *sta_ssid, const char *sta_password,
                     const char *ap_ssid, const char *ap_password)
{
    esp_err_t ret = ESP_OK;
    esp_netif_t *netif_sta = NULL;
    esp_netif_t *netif_ap = NULL;

    ret = esp_netif_init();
    if (ret != ESP_OK) {
        ESP_LOGE(WIFI_TAG, "esp_netif_init failed: %s", esp_err_to_name(ret));
    }

    if (ret == ESP_OK) {
        ret = esp_event_loop_create_default();
        if ((ret != ESP_OK) && (ret != ESP_ERR_INVALID_STATE)) {
            ESP_LOGE(WIFI_TAG, "esp_event_loop_create_default failed: %s", esp_err_to_name(ret));
        } else {
            ret = ESP_OK;
        }
    }

    if (ret == ESP_OK) {
        netif_sta = esp_netif_create_default_wifi_sta();
        if (netif_sta == NULL) {
            ESP_LOGE(WIFI_TAG, "esp_netif_create_default_wifi_sta failed");
            ret = ESP_FAIL;
        }
    }

    if (ret == ESP_OK) {
        netif_ap = esp_netif_create_default_wifi_ap();
        if (netif_ap == NULL) {
            ESP_LOGE(WIFI_TAG, "esp_netif_create_default_wifi_ap failed");
            ret = ESP_FAIL;
        }
    }

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    if (ret == ESP_OK) {
        ret = esp_wifi_init(&cfg);
        if (ret != ESP_OK) {
            ESP_LOGE(WIFI_TAG, "esp_wifi_init failed: %s", esp_err_to_name(ret));
        }
    }

    if (ret == ESP_OK) {
        ret = wifi_register_event_handlers();
    }
   
    if (ret == ESP_OK) {
        /* STA config */
        wifi_config_t wifi_config = { 0 };
        (void)strlcpy((char *)wifi_config.sta.ssid, sta_ssid, sizeof(wifi_config.sta.ssid));
        (void)strlcpy((char *)wifi_config.sta.password, sta_password, sizeof(wifi_config.sta.password));
        wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

        /* AP config */
        wifi_config_t wifi_ap_config = { 0 };
        strlcpy((char *)wifi_ap_config.ap.ssid, ap_ssid, sizeof(wifi_ap_config.ap.ssid));
        wifi_ap_config.ap.ssid_len = strlen(ap_ssid);
        wifi_ap_config.ap.max_connection = 4;
        wifi_ap_config.ap.authmode = WIFI_AUTH_WPA2_PSK;
        strlcpy((char *)wifi_ap_config.ap.password, ap_password, sizeof(wifi_ap_config.ap.password));

        ret = esp_wifi_set_mode(WIFI_MODE_APSTA);
        if (ret == ESP_OK) {
            ret = esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
        }
        if (ret == ESP_OK) {
            ret = esp_wifi_set_config(WIFI_IF_AP, &wifi_ap_config);
        }
        if (ret == ESP_OK) {
            ret = esp_wifi_start();
        }

        if (ret != ESP_OK) {
            ESP_LOGE(WIFI_TAG, "WiFi start/config failed: %s", esp_err_to_name(ret));
        }
    }
}
