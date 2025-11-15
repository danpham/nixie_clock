#ifndef NVS_MOCK_H
#define NVS_MOCK_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

typedef int32_t esp_err_t;
typedef int nvs_handle_t;

#define ESP_OK 0
#define ESP_FAIL -1
#define ESP_ERR_NVS_NO_FREE_PAGES -100
#define ESP_ERR_NVS_NEW_VERSION_FOUND -101
#define ESP_ERR_INVALID_ARG -102

#define ESP_LOGE(tag, fmt, ...) \
    do { \
        printf("[ERROR] %s: " fmt "\n", tag, ##__VA_ARGS__); \
    } while(0)

#define ESP_LOGI(tag, fmt, ...) \
    do { \
        printf("[INFO] %s: " fmt "\n", tag, ##__VA_ARGS__); \
    } while(0)

#define ESP_LOGW(tag, fmt, ...) \
    do { \
        printf("[WARN] %s: " fmt "\n", tag, ##__VA_ARGS__); \
    } while(0)

// Flags NVS
#define NVS_READWRITE 0
#define NVS_READONLY  1

esp_err_t nvs_flash_init(void);
esp_err_t nvs_flash_erase(void);
esp_err_t nvs_open(const char* namespace_name, int open_mode, nvs_handle_t *out_handle);
esp_err_t nvs_set_i32(nvs_handle_t handle, const char* key, int32_t value);
esp_err_t nvs_get_i32(nvs_handle_t handle, const char* key, int32_t* value);
esp_err_t nvs_set_str(nvs_handle_t handle, const char* key, const char* value);
esp_err_t nvs_get_str(nvs_handle_t handle, const char* key, char* value, size_t* length);
esp_err_t nvs_commit(nvs_handle_t handle);
void nvs_close(nvs_handle_t handle);
const char* esp_err_to_name(esp_err_t err);

#endif // NVS_MOCK_H