#ifndef CLOCK_TASK_H
#define CLOCK_TASK_H

/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/

/******************************************************************
 * 3. Typedef definitions (simple typedef, then enum and structs)
******************************************************************/

/******************************************************************
 * 4. Variable definitions (static then global)
******************************************************************/
// Declaration of the queue used for communication between tasks
extern QueueHandle_t clockUpdateQueue;

/******************************************************************
 * 5. Functions prototypes (static only)
******************************************************************/

/******************************************************************
 * 6. Functions definitions
******************************************************************/
void clock_task_start(void);
void clock_ntp_config_callback(void);
void clock_update_with_menu_callback(void);
void clock_update_from_config_callback(void);

#endif // CLOCK_TASK_H