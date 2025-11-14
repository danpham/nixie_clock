idf_component_register(
    SRCS "config.c" "../../main/ntp_sync_task.c"
    INCLUDE_DIRS "."
    REQUIRES nvs wifi 
)
