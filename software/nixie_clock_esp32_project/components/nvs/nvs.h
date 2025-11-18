#ifndef NVS_H
#define NVS_H

/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#ifndef UNITY_TESTING
#include "esp_err.h"
#else
#include "nvs_mock.h"
#endif

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
esp_err_t nvs_init(void);

esp_err_t nvs_save_ntp(uint8_t enabled);
esp_err_t nvs_load_ntp(uint8_t *enabled);

esp_err_t nvs_save_hours(uint8_t enabled);
esp_err_t nvs_load_hours(uint8_t *enabled);

esp_err_t nvs_save_minutes(uint8_t enabled);
esp_err_t nvs_load_minutes(uint8_t *enabled);

esp_err_t nvs_save_seconds(uint8_t enabled);
esp_err_t nvs_load_seconds(uint8_t *enabled);

esp_err_t nvs_save_mode(uint8_t value);
esp_err_t nvs_load_mode(uint8_t *value);

esp_err_t nvs_save_dutycycle(uint8_t value);
esp_err_t nvs_load_dutycycle(uint8_t *value);

esp_err_t nvs_save_ssid(const char *value);
esp_err_t nvs_load_ssid(char *value, size_t *length);

esp_err_t nvs_save_wpa_passphrase(const char *value);
esp_err_t nvs_load_wpa_passphrase(char *value, size_t *length);

#ifdef UNITY_TESTING
esp_err_t nvs_save_str(const char * key, const char * value);
esp_err_t nvs_load_str(const char * key, char * value, size_t * length);
esp_err_t nvs_save_value(const char *key, uint8_t value);
esp_err_t nvs_load_value(const char *key, uint8_t *value);
#endif

#endif // NVS_H