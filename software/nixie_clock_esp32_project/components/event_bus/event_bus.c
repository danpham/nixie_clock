/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include "event_bus.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/
#define EVENT_BUS_QUEUE_SIZE    (32U)
#define EVENT_BUS_PUBLISH_TIMEOUT_MS (100U)
#define EVENT_BUS_TAG "event_bus"

/******************************************************************
 * 3. Typedef definitions (simple typedef, then enum and structs)
******************************************************************/

/******************************************************************
 * 4. Variable definitions (static then global)
******************************************************************/
static QueueHandle_t s_event_queue = NULL;

/******************************************************************
 * 5. Functions prototypes (static only)
******************************************************************/

/******************************************************************
 * 6. Functions definitions
******************************************************************/

/**
 * @brief Initialize the Event Bus queue.
 *
 * This function must be called before any event publication or consumption.
 * It allocates a FreeRTOS queue capable of storing multiple event_bus_message_t
 * messages. Subsequent calls have no effect (idempotent).
 *
 * @note If this function is not called before event_bus_publish(), events
 *       will simply be ignored because the queue does not exist.
 */
void event_bus_init(void)
{
    if (s_event_queue == NULL) {
        s_event_queue = xQueueCreate(EVENT_BUS_QUEUE_SIZE, sizeof(event_bus_message_t));
    }
}

/**
 * @brief Publish an event to the Event Bus.
 *
 * Adds an event to the queue. If the queue is full, blocks up to
 * EVENT_BUS_PUBLISH_TIMEOUT_MS then drops the event with a warning log.
 *
 * @param evt_message The event to publish.
 */
void event_bus_publish(event_bus_message_t evt_message)
{
    if (s_event_queue) {
        if (xQueueSend(s_event_queue, &evt_message, pdMS_TO_TICKS(EVENT_BUS_PUBLISH_TIMEOUT_MS)) != pdTRUE) {
            ESP_LOGW(EVENT_BUS_TAG, "Queue full, event %u dropped", (unsigned)evt_message.type);
        }
    }
}

/**
 * @brief Wait for the next event from the Event Bus.
 *
 * Blocks until an event is available in the queue, unless a timeout is specified.
 * This function is typically called from a dedicated dispatcher task.
 *
 * @param timeout Maximum time to wait (FreeRTOS ticks).
 *        - portMAX_DELAY: Wait forever  
 *        - 0: Non-blocking  
 *
 * @return The received event, or EVT_NONE if a timeout occurs or if the queue
 *         is not initialized.
 */
event_bus_message_t event_bus_wait(TickType_t timeout)
{
    event_bus_message_t evt_message;
    evt_message.type = EVT_NONE;
    evt_message.payload_size = 0U;
    if (s_event_queue) {
        xQueueReceive(s_event_queue, &evt_message, timeout);
    }
    return evt_message;
}
