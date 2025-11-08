#ifndef CONFIG_H
#define CONFIG_H

/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include <stdint.h>
#include "freertos/semphr.h"
#include "esp_err.h"

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/

/******************************************************************
 * 3. Typedef definitions (simple typedef, then enum and structs)
******************************************************************/
typedef struct
{
    int32_t mode;
    int32_t param1;
    int32_t param2;
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
esp_err_t config_set_params(int32_t mode, int32_t param1, int32_t param2);

#endif // CONFIG_H
