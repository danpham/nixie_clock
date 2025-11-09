#ifdef STATIC_ANALYSIS
#ifndef ESP_IDF_STUB_H
#define ESP_IDF_STUB_H

#include <stdint.h>
#include <stdbool.h>

/* FreeRTOS boolean macros */
#define pdTRUE  1
#define pdPASS  1

/* ESP-IDF error codes */
#define ESP_OK   0
#define ESP_ERR_INVALID_STATE 0
#define ESP_ERR_NVS_NO_FREE_PAGES 0
#define ESP_ERR_NVS_NEW_VERSION_FOUND 0

/* Wifi */
#define WIFI_EVENT 1
#define WIFI_EVENT_STA_START 1
#define WIFI_EVENT_STA_DISCONNECTED 1
#define WIFI_EVENT_AP_STACONNECTED 1
#define WIFI_EVENT_AP_STADISCONNECTED 1
#define IP_EVENT_STA_GOT_IP 1
#define IP_EVENT 1

typedef int esp_err_t;

BaseType_t xQueueReceive( QueueHandle_t xQueue,
                          void * const pvBuffer,
                          TickType_t xTicksToWait );
#endif /* ESP_IDF_STUB_H */
#endif