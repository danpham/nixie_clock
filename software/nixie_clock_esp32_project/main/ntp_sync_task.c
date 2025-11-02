/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include <time.h>
#include "esp_sntp.h"
#include "esp_netif.h"
#include "esp_netif_sntp.h"
#include "esp_log.h"
#include "esp_stub.h"
#include "clock.h"
#include "clock_task.h"
#include "ntp_sync_task.h"
#include "esp_interface.h"

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/
#define NTP_SYNC_INTERVAL_MS    (30U * 60U * 1000U)
#define NTP_WAIT_WIFI_MS        1000U

/******************************************************************
 * 3. Typedef definitions (simple typedef, then enum and structs)
******************************************************************/

/******************************************************************
 * 4. Variable definitions (static then global)
******************************************************************/

/******************************************************************
 * 5. Functions prototypes (static only)
******************************************************************/
static void time_sync_task(void *arg);
static void time_sync_notification_cb(struct timeval *tv);

/******************************************************************
 * 6. Functions definitions
******************************************************************/
static void time_sync_notification_cb(struct timeval *tv)
{
    time_t now;
    struct tm timeinfo;
    (void)tv;

    // Use the NTP-synced timestamp
    now = tv->tv_sec;

    // Set France timezone with automatic daylight saving
    setenv("TZ", "CET-1CEST,M3.5.0/2,M10.5.0/3", 1);
    tzset();

    localtime_r(&now, &timeinfo);

    myclock_t clockUpdate = {
        .hours = timeinfo.tm_hour,
        .minutes = timeinfo.tm_min,
        .seconds = timeinfo.tm_sec
    };

    xQueueSend(clockUpdateQueue, &clockUpdate, 0);
}

static void time_sync_task(void *arg)
{
    (void)arg;
    const char *TAG = "time_sync";
    bool wifi_ready = false;
    ESP_LOGI(TAG, "Waiting for Wi-Fi connection...");

    // Wait Wi-Fi for first sync
    while (!wifi_ready)
    {
        int netif_index = esp_netif_get_netif_impl_index(ESP_IF_WIFI_STA);
        esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
        if (netif_index != -1 && netif != NULL && esp_netif_is_netif_up(netif)) {
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

    /* Free task */
    vTaskDelete(NULL);
}

void time_sync_task_start(void)
{
    xTaskCreate(time_sync_task,
                "time_sync_task",
                4096,
                NULL,
                2U,
                NULL);
}
