/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include "esp_err.h"
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
static const char *NVS_NAMESPACE = "storage";

/******************************************************************
 * 5. Functions prototypes (static only)
******************************************************************/

/**
 * @brief Initialize the Non-Volatile Storage (NVS) system.
 *
 * This function initializes the NVS flash. If there are no free pages
 * or a new NVS version is detected, it erases the NVS and reinitializes it.
 *
 * @return esp_err_t ESP_OK on success, otherwise an error code.
 */
esp_err_t nvs_init(void)
{
    esp_err_t ret = ESP_FAIL;
    esp_err_t err = ESP_FAIL;

    err = nvs_flash_init();
    if ((err == ESP_ERR_NVS_NO_FREE_PAGES) || (err == ESP_ERR_NVS_NEW_VERSION_FOUND)) {
        ret = nvs_flash_erase();
        if (ret == ESP_OK) {
            ret = nvs_flash_init();
        }        
    }
    else {
        ret = err;
    }

    return ret;
}

/**
 * @brief Save an integer value to NVS under a given key.
 *
 * Opens the NVS namespace in read/write mode, writes the value, commits
 * it, and closes the handle.
 *
 * @param key The key under which the value will be stored.
 * @param value The int32_t value to save.
 * @return esp_err_t ESP_OK on success, otherwise an error code.
 */
static esp_err_t nvs_save_value(const char *key, int32_t value)
{
    nvs_handle_t handle = 0U;
    esp_err_t err = ESP_FAIL;
    esp_err_t ret = ESP_FAIL;

    err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err == ESP_OK) {
        ret = nvs_set_i32(handle, key, value);
        if (ret == ESP_OK) {
            ret = nvs_commit(handle);
        }
        nvs_close(handle);
    }
    else {
        ret = err;
    }

    return ret;
}

/**
 * @brief Load an integer value from NVS using a given key.
 *
 * Opens the NVS namespace in read-only mode, retrieves the value, and closes the handle.
 *
 * @param key The key from which the value will be retrieved.
 * @param value Pointer to an int32_t variable where the result will be stored.
 * @return esp_err_t ESP_OK on success, otherwise an error code.
 */
static esp_err_t nvs_load_value(const char *key, int32_t *value)
{
    nvs_handle_t handle = 0U;
    esp_err_t err;
    esp_err_t ret = ESP_FAIL;

    if (NULL != value) {
        err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle);
        if (err == ESP_OK) {
            ret = nvs_get_i32(handle, key, value);
            nvs_close(handle);
        }
        else {
            ret = err;
        }
    } else {
        ret = ESP_ERR_INVALID_ARG;
    }

    return ret;
}


esp_err_t nvs_save_ntp(int32_t enabled)         { return nvs_save_value("ntp", enabled); }
esp_err_t nvs_load_ntp(int32_t *enabled)        { return nvs_load_value("ntp", enabled); }

esp_err_t nvs_save_cathode(int32_t enabled)     { return nvs_save_value("cathode", enabled); }
esp_err_t nvs_load_cathode(int32_t *enabled)    { return nvs_load_value("cathode", enabled); }

esp_err_t nvs_save_counter(int32_t value)       { return nvs_save_value("counter", value); }
esp_err_t nvs_load_counter(int32_t *value)      { return nvs_load_value("counter", value); }
