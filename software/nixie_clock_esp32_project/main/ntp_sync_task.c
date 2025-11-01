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

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/
#define NTP_SYNC_TIMEOUT_MS     60000U

/******************************************************************
 * 3. Typedef definitions (simple typedef, then enum and structs)
******************************************************************/

/******************************************************************
 * 4. Variable definitions (static then global)
******************************************************************/

/******************************************************************
 * 5. Functions prototypes (static only)
******************************************************************/

/******************************************************************
 * 6. Functions definitions
******************************************************************/

void time_sync_task(void *arg)
{
    (void)arg;
    const char *TAG = "time_sync";
    ESP_LOGI(TAG, "Initialisation de SNTP...");
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org");
    esp_sntp_init();

    if (esp_netif_sntp_sync_wait(pdMS_TO_TICKS(NTP_SYNC_TIMEOUT_MS)) != ESP_OK) {
        ESP_LOGW("time_sync", "Failed to update system time");
    } else {
        time_t now;
        struct tm timeinfo;
        time(&now);
        localtime_r(&now, &timeinfo);

        myclock_t clockUpdate = {
            .hours = timeinfo.tm_hour,
            .minutes = timeinfo.tm_min,
            .seconds = timeinfo.tm_sec
        };

        xQueueSend(clockUpdateQueue, &clockUpdate, 0);
    }

    /* Free task */
    vTaskDelete(NULL);
}

void time_sync_task_start(myclock_t *clk)
{
    xTaskCreate(time_sync_task,
                "time_sync_task",
                4096,
                clk,
                2U,
                NULL);
}
