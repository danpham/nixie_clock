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
#define CLOCK_DEFAULT_HOURS             (12U)
#define CLOCK_DEFAULT_MINUTES           (0U)
#define CLOCK_DEFAULT_SECONDS           (0U)

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

#endif // CLOCK_TASK_H