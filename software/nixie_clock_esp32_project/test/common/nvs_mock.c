#include "nvs_mock.h"
#include <stdio.h>


static int32_t i32_stored_value = 0;
static char str_stored_value[32] = "";

esp_err_t nvs_flash_init(void) { return ESP_OK; }
esp_err_t nvs_flash_erase(void) { return ESP_OK; }
esp_err_t nvs_open(const char* namespace_name, int open_mode, nvs_handle_t *out_handle) { *out_handle = 1; return ESP_OK; }
esp_err_t nvs_set_i32(nvs_handle_t handle, const char* key, int32_t value) { i32_stored_value = value; return ESP_OK; }
esp_err_t nvs_get_i32(nvs_handle_t handle, const char* key, int32_t* value) { if(value) *value=i32_stored_value; return ESP_OK; }
esp_err_t nvs_set_str(nvs_handle_t handle, const char* key, const char* value) {
    if(value) {
        strcpy_s(str_stored_value, sizeof(str_stored_value), value);
    }
    return ESP_OK; 
}
esp_err_t nvs_get_str(nvs_handle_t handle, const char* key, char* value, size_t* length) {
    size_t len = strlen(str_stored_value) + 1;

    if (value && length && *length >= len) {
        strcpy_s(value, *length, str_stored_value);
    }

    if (length) *length = len;
    return ESP_OK;
}
esp_err_t nvs_commit(nvs_handle_t handle) { return ESP_OK; }
void nvs_close(nvs_handle_t handle) { }
const char* esp_err_to_name(esp_err_t err) { return "ESP_OK"; }
