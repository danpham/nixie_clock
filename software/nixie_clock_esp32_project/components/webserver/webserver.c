/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "webserver.h"
#include "config.h"
#include "html_page.h"

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
    esp_err_t err = ESP_FAIL;
    config_t config;
    char buffer_page[WEBSERVER_HTML_PAGE_SIZE];
    char html_format[WEBSERVER_HTML_PAGE_SIZE];
    const char *html_page_orig = get_html_page();

    /* Copy original HTML page to local buffer (safe because size is bounded) */
    (void)strncpy(html_format, html_page_orig, sizeof(html_format) - 1U);
    html_format[sizeof(html_format) - 1U] = '\0';

    err = config_get_copy(&config);
    if (ESP_OK == err) {
        snprintf(buffer_page, sizeof(buffer_page), html_format,
        12, 0, 0,
        "","",
        "",
        (config.mode == 0) ? "checked" : "",
        (config.param1 == 1) ? "checked" : "",
        (config.param2 == 2) ? "checked" : "");
    }

    httpd_resp_send(req, buffer_page, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/**
 * @brief Handles the "/on" request to turn the LED on.
 *
 * This handler sets the LED GPIO high, then redirects the client
 * back to the root page using an HTTP 303 redirect.
 *
 * @param req Pointer to the HTTP request structure.
 *
 * @return ESP_OK Always returns ESP_OK after handling the request.
 */
static esp_err_t update_handler(httpd_req_t *req)
{
    char buf[200];
    char tmp[32];
    config_t new_config;

    config_get_copy(&new_config); // on part d'une copie existante

    int len = httpd_req_recv(req, buf, sizeof(buf) - 1);
    if (len <= 0) {
        if (len == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    buf[len] = 0; // <-- important, on termine la chaîne

    // Extraction des champs
    if (httpd_query_key_value(buf, "mode", tmp, sizeof(tmp)) == ESP_OK)
        new_config.mode = atoi(tmp);

    if (httpd_query_key_value(buf, "param1", tmp, sizeof(tmp)) == ESP_OK)
        new_config.param1 = atoi(tmp);

    if (httpd_query_key_value(buf, "param2", tmp, sizeof(tmp)) == ESP_OK)
        new_config.param2 = atoi(tmp);

    // Mise à jour de la config globale
    //config_set(&new_config);

    // Redirection vers /
    httpd_resp_set_status(req, "303 See Other");
    httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
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