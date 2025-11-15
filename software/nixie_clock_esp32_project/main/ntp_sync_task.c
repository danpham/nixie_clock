/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include "esp_sntp.h"
#include "esp_netif.h"
#include "esp_netif_sntp.h"
#include "esp_log.h"
#include "esp_stub.h"
#include "../components/clock/clock.h"
#include "clock_task.h"
#include "ntp_sync_task.h"
#include "esp_interface.h"
#include "freertos/FreeRTOS.h"


/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/
#define NTP_SYNC_INTERVAL_MS    (30U * 60U * 1000U)
#define NTP_WAIT_WIFI_MS        (1000U)

/******************************************************************
 * 3. Typedef definitions (simple typedef, then enum and structs)
******************************************************************/

/******************************************************************
 * 4. Variable definitions (static then global)
******************************************************************/
static SemaphoreHandle_t time_sync_done_sem = NULL;
static TaskHandle_t time_sync_task_handle = NULL;
static const char NTP_SYNC_TASK_TAG[] = "NTP_SYNC_TASK";

/******************************************************************
 * 5. Functions prototypes (static only)
******************************************************************/
static void time_sync_task(void *arg);
static void time_sync_notification_cb(struct timeval *tv);
static void timestamp_to_hms(uint32_t timestamp, myclock_t *clock);

/******************************************************************
 * 6. Functions definitions
******************************************************************/
static void timestamp_to_hms(uint32_t timestamp, myclock_t *clock)
{
    uint32_t t = timestamp % 86400U;
    clock->hours   = (uint8_t)(t / 3600U);
    clock->minutes = (uint8_t)((t % 3600U) / 60U);
    clock->seconds = (uint8_t)(t % 60U);
}

static void time_sync_notification_cb(struct timeval *tv)
{
    myclock_t clockUpdate;
    uint32_t now32;

    if ((tv != NULL) && (tv->tv_sec >= 0) && (tv->tv_sec <= UINT32_MAX))
    {
        now32 = (uint32_t)tv->tv_sec;
        timestamp_to_hms(now32, &clockUpdate);

        /* Send clockUpdate to the queue (non-blocking) */
        BaseType_t queue_ret;
        queue_ret = xQueueSend(clockUpdateQueue, &clockUpdate, 0U);
        if (queue_ret != pdTRUE)
        {
            ESP_LOGW(NTP_SYNC_TASK_TAG, "Failed to send clock update to queue");
        }
    }
    else {
        ESP_LOGE(NTP_SYNC_TASK_TAG, "Invalid SNTP timestamp or NULL pointer");
    }
}

static void time_sync_task(void *arg)
{
    (void)arg;
    const char *TAG = "time_sync";
    bool wifi_ready = false;
    ESP_LOGI(TAG, "Waiting for Wi-Fi connection...");

    /* Wait Wi-Fi for first sync */
    while (!wifi_ready)
    {
        esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
        bool is_if_up = false;
        if (netif != NULL){
            is_if_up = esp_netif_is_netif_up(netif);
        } 

        if (is_if_up) {
            wifi_ready = true;
        } else {
            vTaskDelay(pdMS_TO_TICKS(NTP_WAIT_WIFI_MS));
        }
    }

    ESP_LOGI(TAG, "Initialisation de SNTP...");
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org");
    sntp_set_sync_interval(NTP_SYNC_INTERVAL_MS);
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
    esp_sntp_init();

    /* Signal task is finished */
    if (xSemaphoreGive(time_sync_done_sem) != pdTRUE)
    {
        ESP_LOGW(TAG, "Failed to give done semaphore");
    }

    /* Free task */
    vTaskDelete(NULL);
}

void time_sync_task_start(void)
{
    if ((time_sync_done_sem != NULL) || (time_sync_task_handle != NULL)) {
        time_sync_done_sem = xSemaphoreCreateBinary();
        if (time_sync_done_sem == NULL) {
            ESP_LOGE(NTP_SYNC_TASK_TAG, "Failed to create done semaphore");
        } 
        else {
            BaseType_t ret = xTaskCreate(time_sync_task,
                        "time_sync_task",
                        4096,
                        NULL,
                        2U,
                        &time_sync_task_handle);
            
            
            if (ret != pdPASS) {
                ESP_LOGE(NTP_SYNC_TASK_TAG, "Failed to create time_sync_task");
            }
        }
    } 
    else {
        ESP_LOGE(NTP_SYNC_TASK_TAG, "time_sync_task already started or semaphore exists; cannot start again");
    }
}

void stop_ntp(void)
{
    /* Wait for the time_sync_task to finish */
    if (time_sync_task_handle != NULL) {

        ESP_LOGI(NTP_SYNC_TASK_TAG, "Waiting for time_sync_task to finish...");

        if (xSemaphoreTake(time_sync_done_sem, portMAX_DELAY) == pdTRUE) {
            ESP_LOGI(NTP_SYNC_TASK_TAG, "time_sync_task finished");
        } else {
            ESP_LOGE(NTP_SYNC_TASK_TAG, "Semaphore wait failed (should never happen)");
        }

        /* Cleanup */
        if (time_sync_done_sem != NULL) {
            vSemaphoreDelete(time_sync_done_sem);
            time_sync_done_sem = NULL;
        }

        time_sync_task_handle = NULL;

        /* Stop SNTP */
        esp_sntp_stop();

        /* Disable callback */
        sntp_set_time_sync_notification_cb(NULL);

        ESP_LOGI(NTP_SYNC_TASK_TAG, "NTP stopped");
    }
    else {
        ESP_LOGI(NTP_SYNC_TASK_TAG, "time_sync_task is not running");
    }
}