idf_component_register(SRCS "main.c"
                       INCLUDE_DIRS "."
                       REQUIRES hv5622 display clock gpio_driver rotary_encoder wifi webserver nvs)