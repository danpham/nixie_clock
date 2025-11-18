/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#ifdef UNITY_TESTING
#include "nvs_mock.h"
#else
#include "esp_log.h"
#include "nvs_flash.h"
#endif
#include "nvs.h"

#include "../../main/esp_stub.h"

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/
#ifdef UNITY_TESTING
#define NOT_STATIC
#else
#define NOT_STATIC static
#endif

/******************************************************************
 * 3. Typedef definitions (simple typedef, then enum and structs)
******************************************************************/

/******************************************************************
 * 4. Variable definitions (static then global)
******************************************************************/
static const char NVS_NAMESPACE[] = "storage";
static const char NVS_TAG[] = "NVS";

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
NOT_STATIC esp_err_t nvs_save_value(const char *key, uint8_t value)
{
    nvs_handle_t handle = 0U;
    esp_err_t err = ESP_FAIL;
    esp_err_t ret = ESP_FAIL;

    err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err == ESP_OK) {
        ret = nvs_set_u8(handle, key, value);
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
 * @brief Save a string value to NVS under a given key.
 *
 * Opens the NVS namespace in read/write mode, writes the string, commits
 * it, and closes the handle.
 *
 * @param key The key under which the string will be stored.
 * @param value The null-terminated string to save.
 * @return esp_err_t ESP_OK on success, otherwise an error code.
 */
NOT_STATIC esp_err_t nvs_save_str(const char * key, const char * value)
{
    nvs_handle_t handle = 0U;
    esp_err_t err = ESP_FAIL;
    esp_err_t ret = ESP_FAIL;

    /* Open namespace */
    err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err == ESP_OK)
    {
        ret = nvs_set_str(handle, key, value);
        if (ret == ESP_OK) {
            ret = nvs_commit(handle);
        }
        else {
            ESP_LOGE(NVS_TAG, "Write error: %s", esp_err_to_name(ret));
        }

        (void)nvs_close(handle);
    }
    else {
        ret = err;
        ESP_LOGE(NVS_TAG, "Open error: %s", esp_err_to_name(err));
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
NOT_STATIC esp_err_t nvs_load_value(const char *key, uint8_t *value)
{
    nvs_handle_t handle = 0U;
    esp_err_t ret = ESP_FAIL;

    if (NULL != value) {
        esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle);
        if (err == ESP_OK) {
            ret = nvs_get_u8(handle, key, value);
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

/**
 * @brief Load a string value from NVS under a given key.
 *
 * Opens the NVS namespace in read-only mode, reads the string value into
 * the provided buffer, and closes the handle.
 *
 * @param key The key from which the string will be read.
 * @param value Buffer to store the string read from NVS.
 * @param length Pointer to a variable containing the buffer length on input,
 *               updated with the actual string length (including null terminator).
 * @return esp_err_t ESP_OK on success, otherwise an error code.
 */
NOT_STATIC esp_err_t nvs_load_str(const char * key, char * value, size_t * length)
{
    nvs_handle_t handle = 0U;
    esp_err_t err = ESP_FAIL;
    esp_err_t ret = ESP_FAIL;

    /* Open namespace */
    err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle);
    if (err == ESP_OK)
    {
        /* Read string from NVS */
        ret = nvs_get_str(handle, key, value, length);
        if (ret != ESP_OK)
        {
            ESP_LOGE(NVS_TAG, "Read error: %s", esp_err_to_name(ret));
        }

        nvs_close(handle);
    }
    else {
        ret = err;
        ESP_LOGE(NVS_TAG, "Open error: %s", esp_err_to_name(err));
    }

    return ret;
}

esp_err_t nvs_save_ntp(uint8_t enabled)             { return nvs_save_value("ntp", enabled); }
esp_err_t nvs_load_ntp(uint8_t *enabled)            { return nvs_load_value("ntp", enabled); }

esp_err_t nvs_save_hours(uint8_t value)             { return nvs_save_value("hours", value); }
esp_err_t nvs_load_hours(uint8_t *value)            { return nvs_load_value("hours", value); }

esp_err_t nvs_save_minutes(uint8_t value)           { return nvs_save_value("minutes", value); }
esp_err_t nvs_load_minutes(uint8_t *value)          { return nvs_load_value("minutes", value); }

esp_err_t nvs_save_seconds(uint8_t value)           { return nvs_save_value("seconds", value); }
esp_err_t nvs_load_seconds(uint8_t *value)          { return nvs_load_value("seconds", value); }

esp_err_t nvs_save_mode(uint8_t value)              { return nvs_save_value("mode", value); }
esp_err_t nvs_load_mode(uint8_t *value)             { return nvs_load_value("mode", value); }

esp_err_t nvs_save_dutycycle(uint8_t value)         { return nvs_save_value("dutycycle", value); }
esp_err_t nvs_load_dutycycle(uint8_t *value)        { return nvs_load_value("dutycycle", value); }

esp_err_t nvs_save_ssid(const char *value)                         { return nvs_save_str("ssid", value); }
esp_err_t nvs_load_ssid(char *value, size_t *length)               { return nvs_load_str("ssid", value, length); }

esp_err_t nvs_save_wpa_passphrase(const char *value)               { return nvs_save_str("wpa_passphrase", value); }
esp_err_t nvs_load_wpa_passphrase(char *value, size_t *length)     { return nvs_load_str("wpa_passphrase", value, length); }