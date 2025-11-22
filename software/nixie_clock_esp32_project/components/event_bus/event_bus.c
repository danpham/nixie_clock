/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include "event_bus.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/

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
 * It allocates a FreeRTOS queue capable of storing multiple event_bus_event_t
 * messages. Subsequent calls have no effect (idempotent).
 *
 * @note If this function is not called before event_bus_publish(), events
 *       will simply be ignored because the queue does not exist.
 */
void event_bus_init(void)
{
    if (s_event_queue == NULL) {
        s_event_queue = xQueueCreate(32, sizeof(event_bus_event_t));
    }
}

/**
 * @brief Publish an event to the Event Bus (blocking).
 *
 * Adds an event to the queue and blocks until space becomes available if the
 * queue is full. This is the simplest and safest way to publish events when
 * timing is not critical.
 *
 * @param evt The event to publish.
 */
void event_bus_publish(event_bus_event_t evt)
{
    if (s_event_queue) {
        xQueueSend(s_event_queue, &evt, portMAX_DELAY);
    }
}

/**
 * @brief Publish an event to the Event Bus with a timeout.
 *
 * This function allows publishing an event without blocking indefinitely.
 * It is useful for ISR-safe contexts (when using the ISR variant), or when
 * preventing long stalls is important.
 *
 * @param evt The event to publish.
 * @param timeout Maximum time to wait in FreeRTOS ticks (0 for non-blocking).
 *
 * @return true if the event was successfully added to the queue, false otherwise
 *         (queue full or uninitialized).
 */
bool event_bus_try_publish(event_bus_event_t evt, TickType_t timeout)
{
    if (s_event_queue == NULL) return false;
    return xQueueSend(s_event_queue, &evt, timeout) == pdTRUE;
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
event_bus_event_t event_bus_wait(TickType_t timeout)
{
    event_bus_event_t evt = EVT_NONE;
    if (s_event_queue) {
        xQueueReceive(s_event_queue, &evt, timeout);
    }
    return evt;
}
