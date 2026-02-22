#ifndef CLOCK_TASK_H
#define CLOCK_TASK_H

/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "../clock/clock.h"

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
void clock_task_start(void);
void clock_ntp_config_callback(uint8_t* payload, uint8_t size);
void clock_update_with_menu_callback(uint8_t* payload, uint8_t size);
void clock_update_from_config_callback(uint8_t* payload, uint8_t size);
bool clock_get_copy(myclock_t *out);

#endif // CLOCK_TASK_H