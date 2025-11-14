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
#include "../wifi/wifi.h"
#include "../../main/esp_stub.h"

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/
#define WEBSERVER_HTML_PAGE_SIZE     3500

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
    char buffer_page[WEBSERVER_HTML_PAGE_SIZE];
    char html_format[WEBSERVER_HTML_PAGE_SIZE];
    const char *html_page_orig = get_html_page();

    /* Copy original HTML page to local buffer (safe because size is bounded) */
    (void)strncpy(html_format, html_page_orig, sizeof(html_format) - 1U);
    html_format[sizeof(html_format) - 1U] = '\0';

    ret = config_get_copy(&config);
    if (ESP_OK == ret) {
        int ret_modify_html = snprintf(buffer_page, sizeof(buffer_page), html_format,
        12, 0, 0,
        config.ssid, config.wpa_passphrase,
        "",
        (config.mode == 0) ? "checked" : "",
        (config.param1 == 1) ? "checked" : "",
        (config.param2 == 2) ? "checked" : "");

        if (ret_modify_html < 0) {
            ret = ESP_FAIL;
        }
    }

    httpd_resp_send(req, buffer_page, HTTPD_RESP_USE_STRLEN);

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
    char buf[200];
    config_t new_config;
    esp_err_t ret = ESP_OK;
    static const char WEBSERVER_TAG[] = "WEBSERVER";

    /* Start from a copy of the current configuration */
    config_get_copy(&new_config);

    ssize_t len = httpd_req_recv(req, buf, (size_t)(sizeof(buf) - 1U));
    if (len <= 0) {
        if (len == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }
        ret = ESP_FAIL;
    }

    if (ret == ESP_OK) {
        esp_err_t query_res;
        char tmp[64];
        bool wifi_needs_update = false;

        buf[(size_t)len] = 0; /* Null-terminate received data */

        /* Read "ssid" parameter */
        query_res = httpd_query_key_value(buf, "ssid", tmp, sizeof(tmp));
        if (query_res == ESP_OK)
        {
            if (strncmp(new_config.ssid, tmp, sizeof(new_config.ssid)) != 0) {
                strncpy(new_config.ssid, tmp, sizeof(new_config.ssid)-1U);
                new_config.ssid[sizeof(new_config.ssid)-1U] = '\0';
                wifi_needs_update = true;
            }
        }

        /* Read "wpa-passphrase" parameter */
        query_res = httpd_query_key_value(buf, "wpa-passphrase", tmp, sizeof(tmp));
        if (query_res == ESP_OK)
        {
            if (strncmp(new_config.wpa_passphrase, tmp, sizeof(new_config.wpa_passphrase)) != 0) {
                strncpy(new_config.wpa_passphrase, tmp, sizeof(new_config.wpa_passphrase)-1U);
                new_config.wpa_passphrase[sizeof(new_config.wpa_passphrase)-1U] = '\0';
                wifi_needs_update = true;
            }
        }

        /* Read "mode" parameter */
        query_res = httpd_query_key_value(buf, "mode", tmp, sizeof(tmp));
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

        /* Read "param1" parameter */
        query_res = httpd_query_key_value(buf, "param1", tmp, sizeof(tmp));
        if (query_res == ESP_OK)
        {
            char *local_endptr = NULL;
            errno = 0;  /* Reset errno before calling strtol */
            const long tmp_val = strtol(tmp, &local_endptr, 10);
            /* Check for successful numeric conversion */
            if ((local_endptr != tmp) && (*local_endptr == '\0') && (errno == 0))
            {
                new_config.param1 = (int)tmp_val;
            }
        }

        /* Read "param2" parameter */
        query_res = httpd_query_key_value(buf, "param2", tmp, sizeof(tmp));
        if (query_res == ESP_OK)
        {
            char *local_endptr = NULL;
            errno = 0;  /* Reset errno before calling strtol */
            const long tmp_val = strtol(tmp, &local_endptr, 10);
            /* Check for successful numeric conversion */
            if ((local_endptr != tmp) && (*local_endptr == '\0') && (errno == 0))
            {
                new_config.param2 = (int)tmp_val;
            }
        }

        /* Update global configuration (currently commented out for test safety) */
        ret = config_set_config(&new_config);
        if (ret == ESP_OK) {
            ret = config_save();
        }

        if (wifi_needs_update) {
            esp_err_t wifi_ret = wifi_change_sta(new_config.ssid, new_config.wpa_passphrase);
            if (wifi_ret != ESP_OK) {
                ESP_LOGE(WEBSERVER_TAG, "Failed to update STA Wi-Fi");
                ret = wifi_ret;
            }
        }

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
    "<title>Nixie Clock Control</title>\n"
    "<style>\n"
    "body {\n"
    "  font-family: 'Roboto', sans-serif;\n"
    "  background: #0d0d0d;\n"
    "  color: #e0e0e0;\n"
    "  margin: 0;\n"
    "  display: flex;\n"
    "  justify-content: center;\n"
    "  align-items: flex-start;\n"
    "  min-height: 100vh;\n"
    "  padding: 50px 20px;\n"
    "}\n"
    ".card {\n"
    "  background: #1e1e1e;\n"
    "  border-radius: 16px;\n"
    "  padding: 40px 30px;\n"
    "  width: 380px;\n"
    "  max-width: 100%%;\n"
    "  box-shadow: 0 8px 25px rgba(0,0,0,0.7);\n"
    "  border: 1px solid #2c2c2c;\n"
    "}\n"
    "h1 { font-size: 2.2em; margin-bottom: 10px; color: #ffffff; text-align: center; letter-spacing: 1px; }\n"
    "h2 { font-size: 1.4em; margin: 25px 0 10px 0; color: #cccccc; border-bottom: 1px solid #333; padding-bottom: 5px; }\n"
    ".input-row {\n"
    "  display: flex;\n"
    "  justify-content: center;\n"
    "  align-items: center;\n"
    "  margin-bottom: 15px;\n"
    "}\n"
    ".input-row input[type=number] {\n"
    "  width: 60px;\n"
    "  padding: 10px;\n"
    "  margin: 0 5px;\n"
    "  font-size: 18px;\n"
    "  text-align: center;\n"
    "  border-radius: 6px;\n"
    "  border: 1px solid #333;\n"
    "  background: #111;\n"
    "  color: #fff;\n"
    "  outline: none;\n"
    "  transition: 0.2s;\n"
    "}\n"
    ".input-row input[type=number]:focus { border-color: #888; box-shadow: 0 0 5px #555; }\n"
    ".input-group { display: flex; align-items: center; margin-bottom: 10px; }\n"
    ".input-group label { width: 100px; }\n"
    ".input-group input[type=text] {\n"
    "  flex: 1;\n"
    "  padding: 10px;\n"
    "  font-size: 16px;\n"
    "  border-radius: 6px;\n"
    "  border: 1px solid #333;\n"
    "  background: #111;\n"
    "  color: #fff;\n"
    "  outline: none;\n"
    "  box-sizing: border-box;\n"
    "}\n"
    ".input-group input[type=text]:focus { border-color: #888; box-shadow: 0 0 5px #555; }\n"
    ".checkbox-container { margin: 20px 0; }\n"
    ".checkbox-container label {\n"
    "  display: flex;\n"
    "  align-items: center;\n"
    "  font-size: 16px;\n"
    "  margin-bottom: 10px;\n"
    "  cursor: pointer;\n"
    "  user-select: none;\n"
    "}\n"
    ".checkbox-container input[type=checkbox], .checkbox-container input[type=radio] {\n"
    "  width: 20px;\n"
    "  height: 20px;\n"
    "  margin-right: 12px;\n"
    "}\n"
    "button {\n"
    "  width: 100%%;\n"
    "  padding: 14px;\n"
    "  font-size: 18px;\n"
    "  background: #555;\n"
    "  color: #fff;\n"
    "  border: none;\n"
    "  border-radius: 8px;\n"
    "  cursor: pointer;\n"
    "  transition: 0.25s;\n"
    "  font-weight: bold;\n"
    "}\n"
    "button:hover { background: #777; }\n"
    "hr { border: 0; border-top: 1px solid #333; margin: 20px 0; }\n"
    "</style>\n"
    "</head>\n"
    "<body>\n"
    "<div class=\"card\">\n"
    "<h1>Nixie Clock Control</h1>\n"
    "<form action=\"/set_time\" method=\"GET\">\n"
    "<h2>Set time</h2>\n"
    "<div class=\"input-row\">\n"
    "  <input type=\"number\" name=\"h\" min=\"0\" max=\"23\" placeholder=\"HH\" value=\"%d\"> :\n"
    "  <input type=\"number\" name=\"m\" min=\"0\" max=\"59\" placeholder=\"MM\" value=\"%d\"> :\n"
    "  <input type=\"number\" name=\"s\" min=\"0\" max=\"59\" placeholder=\"SS\" value=\"%d\">\n"
    "</div>\n"
    "<h2>Wi-Fi</h2>\n"
    "<div class=\"input-group\">\n"
    "  <label for=\"ssid\">SSID:</label>\n"
    "  <input type=\"text\" id=\"ssid\" name=\"ssid\" value=\"%s\">\n"
    "</div>\n"
    "<div class=\"input-group\">\n"
    "  <label for=\"wpa-passphrase\">WPA passphrase:</label>\n"
    "  <input type=\"text\" id=\"wpa-passphrase\" name=\"wpa-passphrase\" value=\"%s\">\n"
    "</div>\n"
    "<h2>Time synchronization</h2>\n"
    "<div class=\"checkbox-container\">\n"
    "  <label><input type=\"checkbox\" name=\"ntp\" value=\"1\" %s> Sync with NTP</label>\n"
    "</div>\n"
    "<h2>Mode</h2>\n"
    "<div class=\"checkbox-container\">\n"
    "  <label><input type=\"radio\" name=\"mode\" value=\"0\" %s> Hour mode</label>\n"
    "  <label><input type=\"radio\" name=\"mode\" value=\"1\" %s> Cathode antipoisoning mode</label>\n"
    "  <label><input type=\"radio\" name=\"mode\" value=\"2\" %s> Counter mode</label>\n"
    "</div>\n"
    "<button type=\"submit\">Apply</button>\n"
    "</form>\n"
    "</div>\n"
    "</body>\n"
    "</html>\n";

    return html_page_data;
}
