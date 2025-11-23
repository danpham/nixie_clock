/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#ifdef STATIC_ANALYSIS
#include "../test/common/esp_stub.h"
#endif
#include "esp_sntp.h"
#include "esp_netif.h"
#include "esp_netif_sntp.h"
#include "esp_log.h"
#include "../clock/clock.h"
#include "../clock_task/clock_task.h"
#include "ntp.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "../config/config.h"
#include "../event_bus/event_bus.h"

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/
#define NTP_INTERVAL_MS         (30U * 60U * 1000U)
#define NTP_WAIT_WIFI_MS        (1000U)

/******************************************************************
 * 3. Typedef definitions (simple typedef, then enum and structs)
******************************************************************/

/******************************************************************
 * 4. Variable definitions (static then global)
******************************************************************/
static SemaphoreHandle_t time_sync_done_sem = NULL;
static TaskHandle_t time_sync_task_handle = NULL;
static const char NTP_TAG[] = "NTP";

/******************************************************************
 * 5. Functions prototypes (static only)
******************************************************************/
static void time_sync_task(void *arg);
static void time_sync_notification_cb(struct timeval *tv);
static void timestamp_to_hms(uint32_t timestamp, myclock_t *clock);

/******************************************************************
 * 6. Functions definitions
******************************************************************/

/**
 * @brief Convert UNIX timestamp to hours, minutes, and seconds.
 *
 * Stores the converted time in a `myclock_t` struct.
 *
 * @param timestamp UNIX timestamp in seconds.
 * @param clock Pointer to myclock_t struct to store the result.
 */
static void timestamp_to_hms(uint32_t timestamp, myclock_t *clock)
{
    uint32_t t = timestamp % 86400U;
    clock->hours = (uint8_t)(t / 3600U);
    clock->minutes = (uint8_t)((t % 3600U) / 60U);
    clock->seconds = (uint8_t)(t % 60U);
}

/**
 * @brief Callback invoked on SNTP time update.
 *
 * Converts the SNTP timestamp to hours, minutes, and seconds,
 * then sends it to the `clockUpdateQueue`. Logs warning if queue is full.
 *
 * @param tv Pointer to timeval struct containing the synchronized time.
 */
static void time_sync_notification_cb(struct timeval *tv)
{
    myclock_t clockUpdate;

    if ((tv != NULL) && (tv->tv_sec >= 0) && (tv->tv_sec <= UINT32_MAX))
    {
        uint32_t now32 = (uint32_t)tv->tv_sec;
        timestamp_to_hms(now32, &clockUpdate);

        /* Send clockUpdate to the queue (non-blocking) */
        BaseType_t queue_ret;
        queue_ret = xQueueSend(clockUpdateQueue, &clockUpdate, 0U);
        if (queue_ret != pdTRUE) {
            ESP_LOGW(NTP_TAG, "Failed to send clock update to queue");
        }
        else {
            event_bus_publish(EVT_CLOCK_NTP_CONFIG);
        }
    }
    else {
        ESP_LOGE(NTP_TAG, "Invalid SNTP timestamp or NULL pointer");
    }
}

/**
 * @brief Start the NTP synchronization task.
 *
 * Creates the semaphore and FreeRTOS task to initialize SNTP.
 * Logs error if the task is already running or creation fails.
 */
static void time_sync_task(void *arg)
{
    (void)arg;
    bool wifi_ready = false;
    ESP_LOGI(NTP_TAG, "Waiting for Wi-Fi connection...");

    /* Wait Wi-Fi for first sync */
    while (!wifi_ready) {
        esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
        bool is_if_up = false;
        if (netif != NULL) {
            is_if_up = esp_netif_is_netif_up(netif);
        }

        if (is_if_up) {
            wifi_ready = true;
        }
        else {
            vTaskDelay(pdMS_TO_TICKS(NTP_WAIT_WIFI_MS));
        }
    }

    ESP_LOGI(NTP_TAG, "Initialisation de SNTP...");
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org");
    sntp_set_sync_interval(NTP_INTERVAL_MS);
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
    esp_sntp_init();

    /* Signal task is finished */
    BaseType_t given = xSemaphoreGive(time_sync_done_sem);
    if (given != pdTRUE) {
        ESP_LOGW(NTP_TAG, "Failed to give done semaphore");
    }

    /* Free task */
    vTaskDelete(NULL);
}

/**
 * @brief Start the NTP synchronization task.
 *
 * Creates the semaphore and FreeRTOS task to initialize SNTP.
 * Logs error if the task is already running or creation fails.
 */
static void ntp_sync_task_start(void)
{
    if ((time_sync_done_sem != NULL) || (time_sync_task_handle != NULL)) {
        time_sync_done_sem = xSemaphoreCreateBinary();
        if (time_sync_done_sem == NULL) {
            ESP_LOGE(NTP_TAG, "Failed to create done semaphore");
        }
        else {
            BaseType_t ret = xTaskCreate(time_sync_task,
                                         "time_sync_task",
                                         4096,
                                         NULL,
                                         2U,
                                         &time_sync_task_handle);

            if (ret != pdPASS) {
                ESP_LOGE(NTP_TAG, "Failed to create time_sync_task");
            }
        }
    }
    else {
        ESP_LOGE(NTP_TAG, "time_sync_task already started or semaphore exists; cannot start again");
    }
}

/**
 * @brief Stop the NTP task and clean up resources.
 *
 * Waits for task completion, deletes semaphore, stops SNTP,
 * and disables the time update callback.
 */
static void ntp_stop(void)
{
    /* Wait for the time_sync_task to finish */
    if (time_sync_task_handle != NULL) {
        static const TickType_t NTP_SYNC_TASK_MUTEX_TIMEOUT = portMAX_DELAY;
        ESP_LOGI(NTP_TAG, "Waiting for time_sync_task to finish...");

        BaseType_t taken = xSemaphoreTake(time_sync_done_sem, NTP_SYNC_TASK_MUTEX_TIMEOUT);
        if (taken == pdTRUE) {
            ESP_LOGI(NTP_TAG, "time_sync_task finished");
        }
        else {
            ESP_LOGE(NTP_TAG, "Semaphore wait failed (should never happen)");
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

        ESP_LOGI(NTP_TAG, "NTP stopped");
    }
    else
    {
        ESP_LOGI(NTP_TAG, "time_sync_task is not running");
    }
}

/**
 * @brief NTP config callback.
 *
 * Reads the latest configuration and starts or stops the NTP
 * synchronization task depending on the `ntp` parameter.
 * Ensures NTP is not re-started or re-stopped unnecessarily.
 */
void ntp_callback(void) {
    esp_err_t result = ESP_OK;
    config_t config;
    
    /* Get latest configuration */
    result = config_get_copy(&config);
    if (result == ESP_OK) {
        static bool ntp_initialized = false;

        /* NTP sync */
        if (config.ntp == 1U) {
            if (ntp_initialized == false) {
                ntp_sync_task_start();
                ntp_initialized = true;
            }
        }
        else {
            if (ntp_initialized == true) {
                ntp_stop();
                ntp_initialized = false;
            }
        }
    }
    else {
        ESP_LOGE(NTP_TAG, "Failed to get configuration");
    }
}