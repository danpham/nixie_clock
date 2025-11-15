#ifndef CONFIG_H
#define CONFIG_H

/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include "esp_err.h"
#include "../clock/clock.h"

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/
#define CONFIG_SSID_SIZE 32U
#define CONFIG_SSID_BUF_SZ (CONFIG_SSID_SIZE + 1U)

#define CONFIG_WPA_PASSPHRASE_SIZE 63U
#define CONFIG_WPA_PASSPHRASE_BUF_SZ (CONFIG_WPA_PASSPHRASE_SIZE + 1U)

/******************************************************************
 * 3. Typedef definitions (simple typedef, then enum and structs)
******************************************************************/
typedef struct
{
    char ssid[CONFIG_SSID_BUF_SZ];
    char wpa_passphrase[CONFIG_WPA_PASSPHRASE_BUF_SZ];
    uint8_t mode;
    uint8_t ntp;
    myclock_t time;
} config_t;

/******************************************************************
 * 4. Variable definitions (static then global)
******************************************************************/

/******************************************************************
 * 5. Functions prototypes (static only)
******************************************************************/

/******************************************************************
 * 6. Functions definitions (public API in .c)
******************************************************************/

esp_err_t config_init(void);
esp_err_t config_save(void);
esp_err_t config_get_copy(config_t *copy);
esp_err_t config_set_config(const config_t *config);
esp_err_t config_apply(void);

#endif // CONFIG_H
