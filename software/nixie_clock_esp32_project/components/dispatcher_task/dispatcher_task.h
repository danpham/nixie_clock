#ifndef SERVICE_MANAGER_H
#define SERVICE_MANAGER_H

/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include "../event_bus/event_bus.h"

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/

/******************************************************************
 * 3. Typedef definitions (simple typedef, then enum and structs)
******************************************************************/
typedef void (*event_callback_t)(uint8_t* payload, uint8_t size);

/******************************************************************
 * 4. Variable definitions (static then global)
******************************************************************/

/******************************************************************
 * 5. Functions prototypes (static only)
******************************************************************/

/******************************************************************
 * 6. Functions definitions
******************************************************************/
void dispatcher_subscribe(event_bus_event_t evt_type, event_callback_t cb);
void dispatcher_task_start(void);

#endif // SERVICE_MANAGER_H