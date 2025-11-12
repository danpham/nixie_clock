idf_component_register(SRCS "webserver.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_wifi esp_event esp_netif esp_http_server driver config)