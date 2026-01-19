/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#ifdef STATIC_ANALYSIS
#include "../test/common/esp_stub.h"
#endif
#include "esp_sntp.h"
#include "esp_netif.h"
#include "esp_netif_sntp.h"
#include "esp_task_wdt.h"
#include "esp_log.h"
#include "../clock/clock.h"
#include "ntp.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "../config/config.h"
#include "../event_bus/event_bus.h"

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/
/* NTP minimum interval in milliseconds is 15000ms */
#define NTP_INTERVAL_MS         (30000U)
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
 * then sends it to the event bus.
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

        /* Send clock data to evt_bus */
        event_bus_message_t evt_message;
        evt_message.payload[0U] = clockUpdate.hours;
        evt_message.payload[1U] = clockUpdate.minutes;
        evt_message.payload[2U] = clockUpdate.seconds;
        evt_message.type = EVT_CLOCK_NTP_CONFIG;
        evt_message.payload_size = 3U;

        event_bus_publish(evt_message);
        ESP_LOGI(NTP_TAG, "NTP SYNC");
    }
    else {
        ESP_LOGE(NTP_TAG, "Invalid SNTP timestamp or NULL pointer");
    }
}

/**
 * @brief Check if the Wi-Fi STA interface is UP.
 *
 * Iterates over network interfaces and sets the boolean pointed by ctx
 * to true if "WIFI_STA_DEF" is active and up.
 *
 * @param ctx Pointer to a boolean that will be set to true if STA is UP.
 *
 * @return ESP_FAIL Returned by convention, used as a callback.
 */
static esp_err_t wifi_sta_check(void *ctx)
{
    bool *sta_up = (bool *)ctx;
    esp_err_t ret = ESP_FAIL;
    esp_netif_t *netif = esp_netif_next_unsafe(NULL);

    while (netif != NULL) {
        const char *key = esp_netif_get_ifkey(netif);
        if ((strcmp(key, "WIFI_STA_DEF") == 0) && (esp_netif_is_netif_up(netif))) {
            *sta_up = true;
            ret = ESP_OK;
            break;
        }
        netif = esp_netif_next_unsafe(netif);
    }

    return ret;
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
    bool sta_up = false;

    esp_task_wdt_add(NULL);
    ESP_LOGI(NTP_TAG, "Monitoring network interfaces...");

    while (!sta_up) {
        /* Check if Wi-Fi STA is up in LWIP context (thread-safe) */
        esp_netif_tcpip_exec(wifi_sta_check, &sta_up);
        vTaskDelay(pdMS_TO_TICKS(NTP_WAIT_WIFI_MS));
        esp_task_wdt_reset();
    }

    ESP_LOGI(NTP_TAG, "Launching SNTP synchronization...");
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org");
    sntp_set_sync_interval(NTP_INTERVAL_MS);
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
    esp_sntp_init();
    ESP_LOGI(NTP_TAG, "SNTP initialized");

    /* Remove watchdog */
    esp_task_wdt_delete(NULL);

    /* Give the semaphore (so ntp_stop() can finish) */
    xSemaphoreGive(time_sync_done_sem);

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
    if ((time_sync_done_sem == NULL) && (time_sync_task_handle == NULL)) {
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

            /* Stop SNTP */
            esp_sntp_stop();

            /* Disable callback */
            sntp_set_time_sync_notification_cb(NULL);
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
        ESP_LOGI(NTP_TAG, "NTP client stopped");
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
void ntp_callback(uint8_t* payload, uint8_t size) {
    (void)payload;
    (void)size;
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