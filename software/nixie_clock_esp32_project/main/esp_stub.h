#ifdef STATIC_ANALYSIS
#ifndef ESP_IDF_STUB_H
#define ESP_IDF_STUB_H

#include <stdint.h>
#include <stdbool.h>

/* FreeRTOS boolean macros */
#define pdTRUE  1

/* ESP-IDF error codes */
#define ESP_OK   0

typedef int esp_err_t;

BaseType_t xQueueReceive( QueueHandle_t xQueue,
                          void * const pvBuffer,
                          TickType_t xTicksToWait );
#endif /* ESP_IDF_STUB_H */
#endif