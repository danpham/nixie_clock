/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
 ******************************************************************/
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "nvs.h"
#include "../wifi/wifi.h"
#include "../../main/esp_stub.h"
#include "../../main/clock_task.h"
#include "ntp.h"
#include "../clock/clock.h"
#include "../pwm/pwm.h"
#include "../config/config.h"

/******************************************************************
 * 2. Define declarations (macros then function macros)
 ******************************************************************/

/******************************************************************
 * 3. Typedef definitions (simple typedef, then enum and structs)
 ******************************************************************/

/******************************************************************
 * 4. Variable definitions (static then global)
 ******************************************************************/
extern SemaphoreHandle_t config_mutex;

/******************************************************************
 * 5. Functions prototypes (static only)
 ******************************************************************/

/**
 * @brief Apply config changes to services.
 *
 * Updates dependent services (e.g., Wi-Fi) when related config fields change.
 * Mutex-protected to avoid race conditions.
 *
 * @return ESP_OK on success, or an error code on failure.
 */
esp_err_t service_manager_update(void)
{
  esp_err_t result = ESP_OK;
  config_t config;
  static const char SERVICE_MANAGER_TAG[] = "SERVICE_MANAGER";

  /* Get latest configuration */
  result = config_get_copy(&config);
  if (result != ESP_OK) {

    BaseType_t taken = xSemaphoreTake(config_mutex, CONFIG_MUTEX_TIMEOUT);
    if (taken == pdTRUE)
    {
      static bool wifi_initialized = false;
      static bool ntp_initialized = false;

      if (wifi_initialized == false)
      {
        wifi_init_apsta(config.ssid, config.wpa_passphrase, WIFI_AP_SSID, WIFI_AP_PASSWORD);
        wifi_initialized = true;
      }
      else
      {
        esp_err_t wifi_ret = wifi_change_sta(config.ssid, config.wpa_passphrase);
        if (wifi_ret != ESP_OK)
        {
          ESP_LOGE(SERVICE_MANAGER_TAG, "Failed to update STA Wi-Fi");
          result = ESP_FAIL;
        }
      }

      /* Send clockUpdate to the queue (non-blocking) */
      if (clockUpdateQueue != NULL)
      {
        myclock_t clockUpdate = config.time;
        BaseType_t queue_ret = xQueueSend(clockUpdateQueue, &clockUpdate, 0U);
        if (queue_ret != pdTRUE)
        {
          ESP_LOGW(SERVICE_MANAGER_TAG, "Failed to send clock update to queue");
        }
      }
      else
      {
        ESP_LOGE(SERVICE_MANAGER_TAG, "Clock update queue is not initialized");
      }

      /* NTP sync */
      if (config.ntp == 1U)
      {
        if (ntp_initialized == false)
        {
          ntp_sync_task_start();
          ntp_initialized = true;
        }
      }
      else
      {
        if (ntp_initialized == true)
        {
          ntp_stop();
          ntp_initialized = false;
        }
      }

      /* Update PWM duty cycle */
      pwm_set(config.dutycycle);

      BaseType_t give_ret = xSemaphoreGive(config_mutex);
      if (give_ret != pdTRUE)
      {
        ESP_LOGE(SERVICE_MANAGER_TAG, "Failed to give config mutex in init");
        if (result == ESP_OK)
        {
          result = ESP_FAIL;
        }
      }
    }
  }
  else {
    ESP_LOGE(SERVICE_MANAGER_TAG, "Failed to get configuration");
  }

  return result;
}
