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

typedef uint8_t event_bus_event_t;
#define EVT_NONE              ((event_bus_event_t)0U)
#define EVT_CLOCK_NTP_CONFIG  ((event_bus_event_t)1U)
#define EVT_CLOCK_GPIO_CONFIG ((event_bus_event_t)2U)
#define EVT_CLOCK_WEB_CONFIG  ((event_bus_event_t)3U)
#define EVT_NTP_CONFIG        ((event_bus_event_t)4U)
#define EVT_WIFI_CONFIG       ((event_bus_event_t)5U)
#define EVT_PWM_CONFIG        ((event_bus_event_t)6U)

/******************************************************************
 * 3. Typedef definitions (simple typedef, then enum and structs)
******************************************************************/
typedef struct {
    event_bus_event_t type;
    uint8_t payload[EVENT_BUS_MAX_PAYLOAD_SIZE];
    uint8_t payload_size;
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
event_bus_message_t event_bus_wait(TickType_t timeout);

#endif // EVENT_BUS_H