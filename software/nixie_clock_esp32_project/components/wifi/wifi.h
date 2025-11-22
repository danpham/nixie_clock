#ifndef WIFI_H
#define WIFI_H

/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include "esp_err.h"

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/
#define WIFI_AP_SSID        "NixieClock-Setup"
#define WIFI_AP_PASSWORD    "12345678"

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
void wifi_init_apsta(const char *sta_ssid, const char *sta_password,
                     const char *ap_ssid, const char *ap_password);
esp_err_t wifi_change_sta(const char* sta_ssid, const char* sta_passphrase);
void wifi_callback(void);

#endif // WIFI_H