/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#ifdef STATIC_ANALYSIS
#include "../test/common/esp_stub.h"
#endif
#include "esp_err.h"
#include "esp_log.h"
#include "config.h"
#include "nvs.h"
#include "../event_bus/event_bus.h"
#include "nvs_flash.h"
/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/

/******************************************************************
 * 3. Typedef definitions (simple typedef, then enum and structs)
******************************************************************/

/******************************************************************
 * 4. Variable definitions (static then global)
******************************************************************/
static config_t cfg;
static config_t cfg_last;
static const char CONFIG_TAG[] = "CONFIG";
SemaphoreHandle_t config_mutex = NULL;
const TickType_t CONFIG_MUTEX_TIMEOUT = portMAX_DELAY;

/******************************************************************
 * 5. Functions prototypes (static only)
******************************************************************/
static esp_err_t _config_save_nolock(void);

/**
 * @brief Initialize the configuration module.
 *
 * Creates the mutex if not already created and loads configuration
 * from NVS. If a value is not found in NVS, it is set to default (0).
 *
 * @return ESP_OK if initialization succeeded, ESP_FAIL otherwise.
 */
esp_err_t config_init(void) {
    uint8_t init_flag = false;
    esp_err_t ret = ESP_OK;
    config_t default_cfg = {
        .ssid = "",
        .wpa_passphrase = "",
        .mode = CONFIG_MODE_DEFAULT,
        .ntp = CONFIG_NTP_DEFAULT,
        .time = {
            .hours = CONFIG_CLOCK_DEFAULT_HOURS,
            .minutes = CONFIG_CLOCK_DEFAULT_MINUTES,
            .seconds = CONFIG_CLOCK_DEFAULT_SECONDS
        },
        .dutycycle = CONFIG_PWM_DEFAULT_DUTYCYCLE
    };

    if (config_mutex == NULL) {
        config_mutex = xSemaphoreCreateMutex();
        if (config_mutex == NULL) {
            ESP_LOGE(CONFIG_TAG, "Failed to create config mutex");
            ret = ESP_FAIL;
        }
    }

    if (ret == ESP_OK) {
        BaseType_t taken = xSemaphoreTake(config_mutex, CONFIG_MUTEX_TIMEOUT);
        if (taken == pdTRUE)
        {
            ret = nvs_init();
            if (ret == ESP_OK) {
                esp_err_t ret_load = nvs_load_init_flag(&init_flag);
                if (ret_load != ESP_OK) {
                    /* First time initialization */
                    cfg = default_cfg;
                    ret = _config_save_nolock();

                    /* Save the initialization flag */
                    if (ret == ESP_OK) {
                        ret = nvs_save_init_flag(1U);
                        if (ret != ESP_OK) {
                            ESP_LOGE(CONFIG_TAG, "Critical: Failed to save init flag to NVS");
                        }
                    } else {
                        ESP_LOGE(CONFIG_TAG, "Critical: Failed to save config to NVS");
                    }
                }
                else {
                    /* Incorrect init flag */
                    if (init_flag != 1U) {
                        ESP_LOGE(CONFIG_TAG, "Critical: Failed to load init flag from NVS");

                        /* First time initialization */
                        cfg = default_cfg;
                        (void)_config_save_nolock();

                        /* Save the initialization flag */
                        (void)nvs_save_init_flag(1U);
                    }
                    else {
                        ESP_LOGI(CONFIG_TAG, "Loading config from NVS");
                        if (config_read() != ESP_OK) {
                            ESP_LOGE(CONFIG_TAG, "Error loading configuration");
                        }
                    }

                    cfg_last = cfg;  
                }
            }

            BaseType_t give_ret = xSemaphoreGive(config_mutex);
            if (give_ret != pdTRUE) {
                ESP_LOGE(CONFIG_TAG, "Failed to give config mutex in init");
                ret = ESP_FAIL;
            }
            else {
                ret = ESP_OK;

                /* Push events on bus */
                event_bus_message_t evt_message;
                evt_message.type = EVT_WIFI_CONFIG;
                evt_message.payload_size = 0U;
                event_bus_publish(evt_message);
                evt_message.type = EVT_PWM_CONFIG;
                event_bus_publish(evt_message);
                evt_message.type = EVT_NTP_CONFIG;
                event_bus_publish(evt_message);
                evt_message.type = EVT_CLOCK_WEB_CONFIG;
                event_bus_publish(evt_message);
            }
        }
    }

    return ret;
}

/**
 * @brief Save the configuration to NVS if it has changed.
 *
 * Compares current configuration with last saved one and writes
 * only the modified fields to NVS. Access is protected by mutex.
 *
 * @return ESP_OK if any value was saved, ESP_FAIL if nothing changed
 *         or mutex could not be acquired.
 */
esp_err_t config_save(void){
    esp_err_t ret = ESP_FAIL;

    BaseType_t taken = xSemaphoreTake(config_mutex, CONFIG_MUTEX_TIMEOUT);
    if (taken == pdTRUE) {
        ret = _config_save_nolock();

        BaseType_t give_ret = xSemaphoreGive(config_mutex);
        if (give_ret != pdTRUE) {
            ESP_LOGE(CONFIG_TAG, "Failed to give config mutex in init");
            ret = ESP_FAIL;
        }
        else {
            ret = ESP_OK;
        }
    }

    return ret;
}


/**
 * @brief Save the configuration to NVS if it has changed.
 *
 * Compares current configuration with last saved one and writes
 * only the modified fields to NVS. Access is protected by mutex.
 *
 * @return ESP_OK if any value was saved, ESP_FAIL if nothing changed
 *         or mutex could not be acquired.
 */
esp_err_t config_read(void)
{
    /* Load existing configuration from NVS */
    size_t len = CONFIG_SSID_BUF_SZ;
    esp_err_t ret_load = nvs_load_ssid(cfg.ssid, &len);
    if (ret_load != ESP_OK)
    {
        cfg.ssid[0] = '\0';
    }

    len = CONFIG_WPA_PASSPHRASE_BUF_SZ;
    ret_load = nvs_load_wpa_passphrase(cfg.wpa_passphrase, &len);
    if (ret_load != ESP_OK)
    {
        cfg.wpa_passphrase[0] = '\0';
    }

    ret_load = nvs_load_mode(&cfg.mode);
    if (ret_load != ESP_OK)
    {
        cfg.mode = 0;
    }

    ret_load = nvs_load_ntp(&cfg.ntp);
    if (ret_load != ESP_OK)
    {
        cfg.ntp = 0;
    }

    cfg.time.hours = CONFIG_CLOCK_DEFAULT_HOURS;
    cfg.time.minutes = CONFIG_CLOCK_DEFAULT_MINUTES;
    cfg.time.seconds = CONFIG_CLOCK_DEFAULT_SECONDS;

    ret_load = nvs_load_dutycycle(&cfg.dutycycle);
    if (ret_load != ESP_OK)
    {
        cfg.dutycycle = CONFIG_PWM_DEFAULT_DUTYCYCLE;
    }

    return ret_load;
}

/**
 * @brief Save configuration fields that have changed without taking a mutex.
 *
 * This function compares the current configuration `cfg` with the previous
 * configuration `cfg_last` and saves only the fields that have changed to NVS.
 * 
 * **Important:** This function does not take any mutex. The caller must ensure
 * thread safety if called from multiple tasks.
 *
 * @return
 * - ESP_OK if at least one field was successfully saved.
 * - ESP_FAIL if no field was saved or if all save operations failed.
 */
static esp_err_t _config_save_nolock(void)
{
    esp_err_t ret = ESP_FAIL;
    esp_err_t ret_save;
    bool wifi_changed = false;

    if (strcmp(cfg.ssid, cfg_last.ssid) != 0)
    {
        ret_save = nvs_save_ssid(cfg.ssid);
        if (ret_save == ESP_OK)
        {
            wifi_changed = true;
            ret = ESP_OK;
        }
    }

    if (strcmp(cfg.wpa_passphrase, cfg_last.wpa_passphrase) != 0)
    {
        ret_save = nvs_save_wpa_passphrase(cfg.wpa_passphrase);
        if (ret_save == ESP_OK)
        {
            wifi_changed = true;
            ret = ESP_OK;
        }
    }

    if (cfg.mode != cfg_last.mode)
    {
        ret_save = nvs_save_mode(cfg.mode);
        if (ret_save == ESP_OK)
        {
            ret = ESP_OK;
        }
    }

    if (cfg.ntp != cfg_last.ntp)
    {
        ret_save = nvs_save_ntp(cfg.ntp);
        if (ret_save == ESP_OK)
        {
            ret = ESP_OK;
        }
    }

    if (cfg.dutycycle != cfg_last.dutycycle)
    {
        ret_save = nvs_save_dutycycle(cfg.dutycycle);
        if (ret_save == ESP_OK)
        {
            ret = ESP_OK;
        }
    }

    if (wifi_changed == true)
    {
        /* Push event on bus */
        event_bus_message_t evt_message;
        evt_message.type = EVT_WIFI_CONFIG;
        evt_message.payload_size = 0U;
        event_bus_publish(evt_message);
    }

    /* Update previous config */
    cfg_last = cfg;

    return ret;
}

/**
 * @brief Get a copy of the current configuration.
 *
 * Acquires the config mutex, copies the current config to the
 * provided structure, and releases the mutex.
 *
 * @param[out] copy Pointer to a config_t structure where data will be copied.
 *
 * @return ESP_OK if copy succeeded, ESP_ERR_INVALID_ARG if copy is NULL,
 *         ESP_FAIL if mutex could not be acquired.
 */
esp_err_t config_get_copy(config_t *copy)
{
    esp_err_t ret = ESP_OK;

    if (copy == NULL) {
        ret = ESP_ERR_INVALID_ARG;
    }

    if (ret == ESP_OK) {
        BaseType_t taken = xSemaphoreTake(config_mutex, CONFIG_MUTEX_TIMEOUT);
        if (taken == pdTRUE) {
            *copy = cfg;
            BaseType_t give_ret = xSemaphoreGive(config_mutex);
            if (give_ret != pdTRUE) {
                ESP_LOGE(CONFIG_TAG, "Failed to give config mutex");
                ret = ESP_FAIL; 
            }
            else {
                ret = ESP_OK;
            }
        }
        else {
            ESP_LOGD(CONFIG_TAG, "Timeout: Failed to take config mutex");
            ret = ESP_FAIL;
        }
    }

    return ret;
}

/**
 * @brief Set a new configuration.
 *
 * Updates the configuration in RAM.
 * Access is protected by mutex to avoid race conditions.
 *
 * @param[in] config Pointer to a config_t structure from which data will be copied.
 *
 * @return ESP_OK if configuration was updated, ESP_FAIL if nothing changed
 *         or mutex could not be acquired.
 */
esp_err_t config_set_config(const config_t *config)
{
    esp_err_t result = ESP_FAIL;

    BaseType_t taken = xSemaphoreTake(config_mutex, CONFIG_MUTEX_TIMEOUT);
    if (taken == pdTRUE) {
        cfg = *config;
        BaseType_t give_ret = xSemaphoreGive(config_mutex);
        if (give_ret != pdTRUE) {
            ESP_LOGE(CONFIG_TAG, "Failed to give config mutex in init");
            result = ESP_FAIL;
        }
        else {
            result = ESP_OK;
        }
    }

    return result;
}
