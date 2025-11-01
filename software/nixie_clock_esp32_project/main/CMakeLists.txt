idf_component_register(SRCS "gpio_task.c" "main.c" "clock_task.c" "ntp_sync_task.c"
                       INCLUDE_DIRS "."
                       REQUIRES uart hv5622 display clock gpio_driver rotary_encoder wifi)