idf_component_register(SRCS "gpio_driver.c"
                    INCLUDE_DIRS "."
                    REQUIRES driver esp_timer # <-- use GPIO driver from esp-idf
)
