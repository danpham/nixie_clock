#ifndef NVS_H
#define NVS_H

/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include "esp_err.h"
#include <stdint.h>

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

// Initialize NVS
esp_err_t nvs_init(void);

esp_err_t nvs_save_ntp(int32_t enabled);
esp_err_t nvs_load_ntp(int32_t *enabled);

esp_err_t nvs_save_cathode(int32_t enabled);
esp_err_t nvs_load_cathode(int32_t *enabled);

esp_err_t nvs_save_counter(int32_t value);
esp_err_t nvs_load_counter(int32_t *value);

esp_err_t nvs_save_ssid(const char *value);
esp_err_t nvs_load_ssid(char *value, size_t *length);

esp_err_t nvs_save_wpa_passphrase(const char *value);
esp_err_t nvs_load_wpa_passphrase(char *value, size_t *length);

#endif // NVS_H