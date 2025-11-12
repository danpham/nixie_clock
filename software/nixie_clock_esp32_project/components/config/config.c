/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "config.h"
#include "nvs.h"
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
static config_t cfg;
static config_t cfg_last;
static SemaphoreHandle_t config_mutex = NULL;
static const TickType_t CONFIG_MUTEX_TIMEOUT = portMAX_DELAY;

/******************************************************************
 * 5. Functions prototypes (static only)
******************************************************************/

/**
 * @brief Initialize the configuration module.
 *
 * Creates the mutex if not already created and loads configuration
 * from NVS. If a value is not found in NVS, it is set to default (0).
 *
 * @return ESP_OK if initialization succeeded, ESP_FAIL otherwise.
 */
esp_err_t config_init(void)
{
    esp_err_t ret = ESP_OK;


    if (config_mutex == NULL)
    {
        config_mutex = xSemaphoreCreateMutex();
        if (config_mutex == NULL)
        {
            const char *CONFIG_TAG = "config";
            ESP_LOGE(CONFIG_TAG, "Failed to create config mutex");
            ret = ESP_FAIL;
        }
    }

    if (ret == ESP_OK) {

        ret = nvs_init();
        if (ret == ESP_OK)
        {
            esp_err_t ret_load = nvs_load_counter(&cfg.mode);
            if (ret_load != ESP_OK)
            {
                cfg.mode = 0;
            }

            ret_load = nvs_load_ntp(&cfg.param1);
            if (ret_load != ESP_OK)
            {
                cfg.param1 = 0;
            }

            ret_load = nvs_load_cathode(&cfg.param2);
            if (ret_load != ESP_OK)
            {
                cfg.param2 = 0;
            }

            cfg_last = cfg;
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
esp_err_t config_save(void)
{
    esp_err_t changed = ESP_FAIL;

    BaseType_t taken = xSemaphoreTake(config_mutex, CONFIG_MUTEX_TIMEOUT);
    if (taken == pdTRUE)
    {
        esp_err_t ret_save;

        if (cfg.mode != cfg_last.mode)
        {
            ret_save = nvs_save_counter(cfg.mode);
            if (ret_save == ESP_OK) {
                changed = ESP_OK;
            }
        }
        if (cfg.param1 != cfg_last.param1)
        {
            ret_save = nvs_save_counter(cfg.param1);
            if (ret_save == ESP_OK) {
                changed = ESP_OK;
            }
        }
        if (cfg.param2 != cfg_last.param2)
        {
            ret_save = nvs_save_counter(cfg.param2);
            if (ret_save == ESP_OK) {
                changed = ESP_OK;
            }
        }

        cfg_last = cfg;
        (void)xSemaphoreGive(config_mutex);
    }

    return changed;
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

    if (copy == NULL)
    {
        ret = ESP_ERR_INVALID_ARG;
    }

    if (ret == ESP_OK) {
        BaseType_t taken = xSemaphoreTake(config_mutex, CONFIG_MUTEX_TIMEOUT);
        if (taken == pdTRUE)
        {
            *copy = cfg;
            (void)xSemaphoreGive(config_mutex);
            ret = ESP_OK;
        }
        else {
            ret = ESP_FAIL;
        }
    }

    return ret;
}

/**
 * @brief Set new configuration parameters.
 *
 * Updates the configuration in RAM if any value differs from the current one.
 * Access is protected by mutex to avoid race conditions.
 *
 * @param[in] mode   New mode value.
 * @param[in] param1 New param1 value.
 * @param[in] param2 New param2 value.
 *
 * @return ESP_OK if configuration was updated, ESP_FAIL if nothing changed
 *         or mutex could not be acquired.
 */
esp_err_t config_set_params(int32_t mode, int32_t param1, int32_t param2)
{
    esp_err_t result = ESP_FAIL;

    BaseType_t taken = xSemaphoreTake(config_mutex, CONFIG_MUTEX_TIMEOUT);
    if (taken == pdTRUE)
    {
        if ((cfg.mode != mode) || (cfg.param1 != param1) || (cfg.param2 != param2))
        {
            cfg.mode = mode;
            cfg.param1 = param1;
            cfg.param2 = param2;
            result = ESP_OK;
        }

        (void)xSemaphoreGive(config_mutex);
    }

    return result;
}
