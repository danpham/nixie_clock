#ifndef EVENT_BUS_H
#define EVENT_BUS_H

/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/

/******************************************************************
 * 3. Typedef definitions (simple typedef, then enum and structs)
******************************************************************/
typedef enum {
    EVT_NONE = 0,
    EVT_CLOCK_NTP_CONFIG,
    EVT_CLOCK_GPIO_CONFIG,
    EVT_CLOCK_WEB_CONFIG,
    EVT_NTP_CONFIG,
    EVT_WIFI_CONFIG,
    EVT_PWM_CONFIG,
} event_bus_event_t;

/******************************************************************
 * 4. Variable definitions (static then global)
******************************************************************/

/******************************************************************
 * 5. Functions prototypes (static only)
******************************************************************/

/******************************************************************
 * 6. Functions definitions
******************************************************************/
void event_bus_init(void);
void event_bus_publish(event_bus_event_t evt);
bool event_bus_try_publish(event_bus_event_t evt, TickType_t timeout);
event_bus_event_t event_bus_wait(TickType_t timeout);

#endif // EVENT_BUS_H