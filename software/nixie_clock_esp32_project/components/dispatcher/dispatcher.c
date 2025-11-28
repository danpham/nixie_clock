/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
 ******************************************************************/
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "event_bus.h"
#include "dispatcher.h"

/******************************************************************
 * 2. Define declarations (macros then function macros)
 ******************************************************************/
#define MAX_SUBSCRIBERS 16

/******************************************************************
 * 3. Typedef definitions (simple typedef, then enum and structs)
 ******************************************************************/
typedef struct
{
  event_bus_event_t evt_type;
  event_callback_t cb;
} subscriber_t;

/******************************************************************
 * 4. Variable definitions (static then global)
 ******************************************************************/
static subscriber_t subscribers[MAX_SUBSCRIBERS];
static int subscriber_count = 0;

/******************************************************************
 * 5. Functions prototypes (static only)
 ******************************************************************/

/******************************************************************
 * 6. Functions definitions
 ******************************************************************/

/**
 * @brief Subscribe a callback to a specific event
 */
void dispatcher_subscribe(event_bus_event_t evt_type, event_callback_t cb)
{
  if (subscriber_count < MAX_SUBSCRIBERS) {
    subscribers[subscriber_count].evt_type = evt_type;
    subscribers[subscriber_count].cb = cb;
    subscriber_count++;
  }
}

/**
 * @brief The dispatcher task that reads the EventBus queue
 *        and calls subscribed callbacks
 */
static void dispatcher_task(void *arg)
{
  (void)arg;
  while(1)
  {
      /* Wait for the next event*/
      event_bus_message_t evt_message = event_bus_wait(portMAX_DELAY);

      /* Call all callbacks subscribed to this event */
      for(int i = 0; i < subscriber_count; i++)
      {
          if (subscribers[i].evt_type == evt_message.type)
          {
              if(subscribers[i].cb != NULL) {
                  subscribers[i].cb(evt_message.payload, evt_message.payload_size);
              }
          }
      }
  }
}

/**
 * @brief Start the dispatcher task
 */
void dispatcher_task_start(void)
{
  xTaskCreate(dispatcher_task, "dispatcher_task", 4096, NULL, 5, NULL);
}