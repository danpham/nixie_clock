/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include <errno.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "webserver.h"
#include "config.h"
#include "../../main/esp_stub.h"
#include "../service_manager/service_manager.h"

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/
#define WEBSERVER_HTML_PAGE_SIZE                 (8192U)
#define WEBSERVER_HTTPD_REQ_RECV_BUFFER_SIZE     (256U)

/******************************************************************
 * 3. Typedef definitions (simple typedef, then enum and structs)
******************************************************************/

/******************************************************************
 * 4. Variable definitions (static then global)
******************************************************************/

/******************************************************************
 * 5. Functions prototypes (static only)
******************************************************************/
static const char* get_html_page(void);

/**
 * @brief Handles the root page ("/") request.
 *
 * This handler generates the HTML page for the web interface,
 * including the current configuration values. The page is dynamically
 * populated from the config structure.
 *
 * @param req Pointer to the HTTP request structure.
 *
 * @return ESP_OK Always returns ESP_OK after sending the page.
 */
static esp_err_t root_handler(httpd_req_t *req)
{
    esp_err_t ret = ESP_FAIL;
    config_t config;
    char html_format[WEBSERVER_HTML_PAGE_SIZE];
    const char *html_page_orig = get_html_page();

    ret = config_get_copy(&config);
    if (ESP_OK == ret) {
        int ret_modify_html = snprintf(html_format, sizeof(html_format), html_page_orig,
        config.time.hours, config.time.minutes, config.time.seconds,
        config.ssid, config.wpa_passphrase,
        (config.ntp == 1) ? "checked" : "",
        (config.mode == 0) ? "checked" : "",
        (config.mode == 1) ? "checked" : "",
        (config.mode == 2) ? "checked" : "",
        config.dutycycle, config.dutycycle);

        if (ret_modify_html < 0) {
            ret = ESP_FAIL;
        }
    }

    httpd_resp_send(req, html_format, HTTPD_RESP_USE_STRLEN);

    return ret;
}

/**
 * @brief Handles the "/update" request to update configuration.
 *
 * This handler updates configuration, then redirects the client
 * back to the root page using an HTTP 303 redirect.
 *
 * @param req Pointer to the HTTP request structure.
 *
 * @return ESP_OK Always returns ESP_OK after handling the request.
 */
static esp_err_t update_handler(httpd_req_t *req)
{
    char req_recv_buf[WEBSERVER_HTTPD_REQ_RECV_BUFFER_SIZE];
    config_t new_config;
    esp_err_t ret = ESP_OK;

    /* Start from a copy of the current configuration */
    config_get_copy(&new_config);

    ssize_t len = httpd_req_recv(req, req_recv_buf, (size_t)(sizeof(req_recv_buf) - 1U));
    if (len <= 0) {
        if (len == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }
        ret = ESP_FAIL;
    }

    if (ret == ESP_OK) {
        esp_err_t query_res;
        char tmp[64];

        req_recv_buf[(size_t)len] = 0; /* Null-terminate received data */

        /* Read "ssid" parameter */
        query_res = httpd_query_key_value(req_recv_buf, "ssid", tmp, sizeof(tmp));
        if (query_res == ESP_OK)
        {
            (void)strncpy(new_config.ssid, tmp, sizeof(new_config.ssid)-1U);
            new_config.ssid[sizeof(new_config.ssid)-1U] = '\0';
        }

        /* Read "wpa-passphrase" parameter */
        query_res = httpd_query_key_value(req_recv_buf, "wpa-passphrase", tmp, sizeof(tmp));
        if (query_res == ESP_OK)
        {
            (void)strncpy(new_config.wpa_passphrase, tmp, sizeof(new_config.wpa_passphrase)-1U);
            new_config.wpa_passphrase[sizeof(new_config.wpa_passphrase)-1U] = '\0';
        }

        /* Read "mode" parameter */
        query_res = httpd_query_key_value(req_recv_buf, "mode", tmp, sizeof(tmp));
        if (query_res == ESP_OK)
        {
            char *local_endptr = NULL;
            errno = 0;  /* Reset errno before calling strtol */
            const long tmp_val = strtol(tmp, &local_endptr, 10);
            /* Check for successful numeric conversion */
            if ((local_endptr != tmp) && (*local_endptr == '\0') && (errno == 0))
            {
                new_config.mode = (int)tmp_val;
            }
        }

        /* Read "ntp" parameter */
        query_res = httpd_query_key_value(req_recv_buf, "ntp", tmp, sizeof(tmp));
        if (query_res == ESP_OK)
        {
            char *local_endptr = NULL;
            errno = 0;  /* Reset errno before calling strtol */
            const long tmp_val = strtol(tmp, &local_endptr, 10);
            /* Check for successful numeric conversion */
            if ((local_endptr != tmp) && (*local_endptr == '\0') && (errno == 0))
            {
                new_config.ntp = (int)tmp_val;
            }
        }

        /* Read "hours" parameter */
        query_res = httpd_query_key_value(req_recv_buf, "hours", tmp, sizeof(tmp));
        if (query_res == ESP_OK)
        {
            char *local_endptr = NULL;
            errno = 0;  /* Reset errno before calling strtol */
            const long tmp_val = strtol(tmp, &local_endptr, 10);
            /* Check for successful numeric conversion */
            if ((local_endptr != tmp) && (*local_endptr == '\0') && (errno == 0))
            {
                new_config.time.hours = (int)tmp_val;
            }
        }

        /* Read "minutes" parameter */
        query_res = httpd_query_key_value(req_recv_buf, "minutes", tmp, sizeof(tmp));
        if (query_res == ESP_OK)
        {
            char *local_endptr = NULL;
            errno = 0;  /* Reset errno before calling strtol */
            const long tmp_val = strtol(tmp, &local_endptr, 10);
            /* Check for successful numeric conversion */
            if ((local_endptr != tmp) && (*local_endptr == '\0') && (errno == 0))
            {
                new_config.time.hours = (int)tmp_val;
            }
        }

        /* Read "seconds" parameter */
        query_res = httpd_query_key_value(req_recv_buf, "seconds", tmp, sizeof(tmp));
        if (query_res == ESP_OK)
        {
            char *local_endptr = NULL;
            errno = 0;  /* Reset errno before calling strtol */
            const long tmp_val = strtol(tmp, &local_endptr, 10);
            /* Check for successful numeric conversion */
            if ((local_endptr != tmp) && (*local_endptr == '\0') && (errno == 0))
            {
                new_config.time.seconds = (int)tmp_val;
            }
        }

        /* Read "dutycycle" parameter */
        query_res = httpd_query_key_value(req_recv_buf, "dutycycle", tmp, sizeof(tmp));
        if (query_res == ESP_OK)
        {
            char *local_endptr = NULL;
            errno = 0;  /* Reset errno before calling strtol */
            const long tmp_val = strtol(tmp, &local_endptr, 10);
            /* Check for successful numeric conversion */
            if ((local_endptr != tmp) && (*local_endptr == '\0') && (errno == 0))
            {
                new_config.dutycycle = (int)tmp_val;
            }
        }

        /* Update global configuration (currently commented out for test safety) */
        ret = config_set_config(&new_config);

        if (ret == ESP_OK) {
            ret = config_save();
        }

        /* Apply changes from RAM config */
        (void)service_manager_update();

        /* Redirect client back to the root page */
        httpd_resp_set_status(req, "303 See Other");
        httpd_resp_set_hdr(req, "Location", "/");
        httpd_resp_send(req, NULL, 0);
    }
    
    return ret;
}

/**
 * @brief Starts the HTTP web server and registers URI handlers.
 *
 * This function initializes the HTTP server using default configuration,
 * and registers the handlers for "/", "/on", and "/off".
 *
 * @return httpd_handle_t Handle to the running HTTP server if successful,
 *                        NULL if the server failed to start.
 */
httpd_handle_t start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    esp_err_t start_result = ESP_FAIL;

    start_result = httpd_start(&server, &config);
    if (start_result == ESP_OK) {

        httpd_uri_t root = {
            .uri       = "/",
            .method    = HTTP_GET,
            .handler   = root_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &root);

        httpd_uri_t update = {
            .uri       = "/update",
            .method    = HTTP_POST,
            .handler   = update_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &update);
    }

    return server;
}

/**
 * @brief Returns the static HTML content of the page.
 *
 * This function provides access to the static `html_page` array containing
 * the full HTML code of the page. The array is stored in read-only memory
 * and cannot be modified by the caller.
 *
 * @return const char* Pointer to the beginning of the HTML content.
 *
 * @note The memory is managed statically; do not attempt to free
 *       the returned pointer.
 */
static const char* get_html_page(void) {
    static const char html_page_data[] =
    "<!DOCTYPE html>\n"
    "<html lang=\"en\">\n"
    "<head>\n"
    "<meta charset=\"UTF-8\">\n"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
    "<title>Nixie clock settings</title>\n"
    "<style>\n"
    "body { font-family: 'Roboto', sans-serif; background: #0d0d0d; color: #e0e0e0; margin: 0; display: flex; justify-content: center; align-items: flex-start; min-height: 100vh; padding: 50px 20px; }\n"
    ".card { background: #1e1e1e; border-radius: 16px; padding: 40px 30px; width: 380px; max-width: 100%; box-shadow: 0 8px 25px rgba(0,0,0,0.7); border: 1px solid #2c2c2c; }\n"
    "h1 { font-size: 2.2em; margin-bottom: 10px; color: #ffffff; text-align: center; letter-spacing: 1px; }\n"
    "h2 { font-size: 1.4em; margin: 25px 0 10px 0; color: #cccccc; border-bottom: 1px solid #333; padding-bottom: 5px; }\n"
    ".input-row { display: flex; justify-content: center; align-items: center; margin-bottom: 15px; }\n"
    ".input-row input[type=number] { width: 60px; padding: 10px; margin: 0 5px; font-size: 18px; text-align: center; border-radius: 6px; border: 1px solid #333; background: #111; color: #fff; outline: none; transition: 0.2s; }\n"
    ".input-row input[type=number]:focus { border-color: #888; box-shadow: 0 0 5px #555; }\n"
    ".input-row input[type=number]:disabled { background: #333; color: #777; }\n"
    ".input-group { display: flex; align-items: center; margin-bottom: 10px; }\n"
    ".input-group label { width: 100px; }\n"
    ".input-group input { padding: 10px; font-size: 16px; border-radius: 6px; border: 1px solid #333; background: #111; color: #fff; outline: none; box-sizing: border-box; }\n"
    ".input-group input[type=text], .input-group input[type=password] { flex: 1; }\n"
    ".input-group input:focus { border-color: #888; box-shadow: 0 0 5px #555; }\n"
    ".input-group input:disabled { background: #333; color: #777; }\n"
    ".checkbox-container { margin: 20px 0; }\n"
    ".checkbox-container label { display: flex; align-items: center; font-size: 16px; margin-bottom: 10px; cursor: pointer; user-select: none; }\n"
    ".checkbox-container input[type=checkbox], .checkbox-container input[type=radio] { width: 20px; height: 20px; margin-right: 12px; }\n"
    "button { width: 100%; padding: 14px; font-size: 18px; background: #555; color: #fff; border: none; border-radius: 8px; cursor: pointer; transition: 0.25s; font-weight: bold; }\n"
    "button:hover { background: #777; }\n"
    "hr { border: 0; border-top: 1px solid #333; margin: 20px 0; }\n"
    ".brightness-container { display: flex; flex-direction: column; margin-bottom: 20px; }\n"
    ".brightness-label-row { width: 100%; display: flex; justify-content: space-between; align-items: center; margin-bottom: 5px; }\n"
    "#brightness { width: 100%; margin-top: 0; }\n"
    "#brightnessValue { font-size: 18px; }\n"
    "</style>\n"
    "</head>\n"
    "<body>\n"
    "<div class=\"card\">\n"
    "<h1>Nixie clock settings</h1>\n"
    "<form action=\"/update\" method=\"POST\">\n"
    "<h2>Time synchronization</h2>\n"
    "<div class=\"checkbox-container\">\n"
    "  <label><input type=\"checkbox\" id=\"ntp\" name=\"ntp\" value=\"1\" %s> Sync with NTP</label>\n"
    "</div>\n"
    "<h2>Set time</h2>\n"
    "<div class=\"input-row\">\n"
    "  <input type=\"number\" id=\"hours\" name=\"hours\" min=\"0\" max=\"23\" placeholder=\"HH\" value=\"%d\"> :\n"
    "  <input type=\"number\" id=\"minutes\" name=\"minutes\" min=\"0\" max=\"59\" placeholder=\"MM\" value=\"%d\"> :\n"
    "  <input type=\"number\" id=\"seconds\" name=\"seconds\" min=\"0\" max=\"59\" placeholder=\"SS\" value=\"%d\">\n"
    "</div>\n"
    "<h2>Wi-Fi</h2>\n"
    "<div class=\"input-group\">\n"
    "  <label for=\"ssid\">SSID:</label>\n"
    "  <input type=\"text\" id=\"ssid\" name=\"ssid\" max=\"32\" value=\"%s\">\n"
    "</div>\n"
    "<div class=\"input-group\">\n"
    "  <label for=\"wpa-passphrase\">WPA passphrase:</label>\n"
    "  <input type=\"password\" id=\"wpa-passphrase\" name=\"wpa-passphrase\" max=\"63\" value=\"%s\">\n"
    "</div>\n"
    "<h2>Mode</h2>\n"
    "<div class=\"checkbox-container\">\n"
    "  <label><input type=\"radio\" name=\"mode\" value=\"0\" %s> Hour mode</label>\n"
    "  <label><input type=\"radio\" name=\"mode\" value=\"1\" %s> Cathode antipoisoning mode</label>\n"
    "  <label><input type=\"radio\" name=\"mode\" value=\"2\" %s> Test mode</label>\n"
    "</div>\n"
    "<h2>Brightness control</h2>\n"
    "<div class=\"brightness-container\">\n"
    "  <div class=\"brightness-label-row\">\n"
    "      <label for=\"brightness\">Brightness:</label>\n"
    "      <span id=\"brightnessValue\">%d</span>\n"
    "  </div>\n"
    "  <input type=\"range\" id=\"brightness\" name=\"brightness\" min=\"0\" max=\"255\" value=\"%d\">\n"
    "</div>\n"
    "<hr>\n"
    "<button type=\"submit\">Apply</button>\n"
    "</form>\n"
    "</div>\n"
    "<script>\n"
    "const ntpCheckbox = document.getElementById('ntp');\n"
    "const hourInputs = [document.getElementById('hours'), document.getElementById('minutes'), document.getElementById('seconds')];\n"
    "function updateTimeInputs() {\n"
    "  const disabled = ntpCheckbox.checked;\n"
    "  hourInputs.forEach(input => { input.disabled = disabled; });\n"
    "}\n"
    "updateTimeInputs();\n"
    "ntpCheckbox.addEventListener('change', updateTimeInputs);\n"
    "const brightnessSlider = document.getElementById('brightness');\n"
    "const brightnessValue = document.getElementById('brightnessValue');\n"
    "brightnessSlider.addEventListener('input', () => { brightnessValue.textContent = brightnessSlider.value; });\n"
    "</script>\n"
    "</body>\n"
    "</html>\n";
    return html_page_data;
}
