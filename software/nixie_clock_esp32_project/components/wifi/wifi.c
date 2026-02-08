
/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#ifdef STATIC_ANALYSIS
#include "../test/common/esp_stub.h"
#endif
#include "wifi.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "../config/config.h"

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/
#define WIFI_MAX_RETRY        (5U)

/******************************************************************
 * 3. Typedef definitions (simple typedef, then enum and structs)
******************************************************************/

/******************************************************************
 * 4. Variable definitions (static then global)
******************************************************************/
static const char *WIFI_TAG = "wifi";
static uint8_t wifi_sta_retry_count = 0U;
static bool wifi_sta_cfg_update_pending = false;

/******************************************************************
 * 5. Functions prototypes (static only)
******************************************************************/

/******************************************************************
 * 6. Functions definitions
******************************************************************/
static esp_err_t wifi_change_sta(const char* sta_ssid, const char* sta_passphrase);

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
        if (wifi_sta_cfg_update_pending == true) {
            config_t config;
            esp_err_t cfg_ret;

            /* Get latest configuration */
            cfg_ret = config_get_copy(&config);
            if (cfg_ret == ESP_OK) {
                (void)wifi_change_sta(config.ssid, config.wpa_passphrase);
                esp_wifi_connect();
                ESP_LOGI(WIFI_TAG, "Wi-Fi STA config updated, reconnecting...");
            }
            else{
                ESP_LOGE(WIFI_TAG, "Failed to get configuration for Wi-Fi update");
            }
            wifi_sta_cfg_update_pending = false;
        } else {

            /* Regular disconnect handling */
            if (wifi_sta_retry_count < WIFI_MAX_RETRY) {
                esp_wifi_connect();
                wifi_sta_retry_count++;
                ESP_LOGI(WIFI_TAG, "Wi-Fi STA disconnected, retrying connection (%d/%d)",
                        wifi_sta_retry_count, WIFI_MAX_RETRY);
            } else {
                ESP_LOGW(WIFI_TAG, "Wi-Fi STA disconnected, max retries reached");
            }
        }
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
static void wifi_init_apsta(const char *sta_ssid, const char *sta_password,
                     const char *ap_ssid, const char *ap_password)
{
    esp_err_t ret = ESP_OK;
    const esp_netif_t *netif_sta = NULL;
    const esp_netif_t *netif_ap = NULL;

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

/**
 * @brief Change the STA Wi-Fi credentials and reconnect.
 *
 * Disconnects the current STA, applies a new SSID/password, and reconnects
 * using the updated configuration. Does not affect AP mode.
 *
 * @param sta_ssid        New STA SSID (null-terminated)
 * @param sta_passphrase  New STA password (null-terminated)
 *
 * @return ESP_OK on success, or an ESP-IDF error code.
 */
static esp_err_t wifi_change_sta(const char* sta_ssid, const char* sta_passphrase)
{
    esp_err_t ret = ESP_OK;

    /* Stop STA */
    ret = esp_wifi_disconnect();
    if (ret != ESP_OK) {
        ESP_LOGE(WIFI_TAG, "esp_wifi_disconnect failed: %s", esp_err_to_name(ret));
    }

    /* Configure the new STA */
    if (ret == ESP_OK) {
        wifi_config_t wifi_config = { 0 };
        strlcpy((char *)wifi_config.sta.ssid, sta_ssid, sizeof(wifi_config.sta.ssid));
        strlcpy((char *)wifi_config.sta.password, sta_passphrase, sizeof(wifi_config.sta.password));
        wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

        ret = esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
        if (ret != ESP_OK) {
            ESP_LOGE(WIFI_TAG, "esp_wifi_set_config failed: %s", esp_err_to_name(ret));
        }
        else {
            ESP_LOGI(WIFI_TAG, "STA updated: SSID=%s", sta_ssid);
        }
    }

    return ret;
}

/**
 * @brief Handles Wi-Fi initialization and configuration updates.
 *
 * This callback retrieves the latest Wi-Fi configuration and either initializes
 * the Wi-Fi (AP+STA mode) or updates the STA connection if already initialized.
 * Ensures thread-safe access to the configuration using a mutex.
 *
 * @note Logs an error if updating the STA Wi-Fi fails.
 */
void wifi_callback(uint8_t* payload, uint8_t size) {
    (void)payload;
    (void)size;
    config_t config;
    esp_err_t cfg_ret;

    /* Get latest configuration */
    cfg_ret = config_get_copy(&config);
    if (cfg_ret == ESP_OK) {
        static bool wifi_initialized = false;
        if (wifi_initialized == false) {
            wifi_init_apsta(config.ssid, config.wpa_passphrase, WIFI_AP_SSID, WIFI_AP_PASSWORD);
            wifi_initialized = true;
        } else {
            wifi_sta_retry_count = 0U;
            wifi_ap_record_t info;
            esp_err_t ret = esp_wifi_sta_get_ap_info(&info);
            if (ret == ESP_OK) {
                ESP_LOGI(WIFI_TAG, "STA connected, disconnecting...");
                wifi_sta_cfg_update_pending = true;
                esp_wifi_disconnect();
            } else {
                /* wifi:Haven't to connect to a suitable AP now!: is a regular warning coming from
                esp_wifi_sta_get_ap_info when state is disconnected or connecting */
                ESP_LOGI(WIFI_TAG, "STA not connected, updating config and reconnecting...");
                (void)wifi_change_sta(config.ssid, config.wpa_passphrase);
                esp_wifi_connect();
            }
        }
    } else {
        ESP_LOGE(WIFI_TAG, "Failed to get configuration");
    }
}