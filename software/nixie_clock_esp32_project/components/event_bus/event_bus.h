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
#define EVENT_BUS_MAX_PAYLOAD_SIZE   (32U)

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

typedef struct {
    event_bus_event_t type;
    uint8_t payload[EVENT_BUS_MAX_PAYLOAD_SIZE];
    size_t payload_size;
} event_bus_message_t;

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
void event_bus_publish(event_bus_message_t evt_message);
bool event_bus_try_publish(event_bus_message_t evt_message, TickType_t timeout);
event_bus_message_t event_bus_wait(TickType_t timeout);

#endif // EVENT_BUS_H