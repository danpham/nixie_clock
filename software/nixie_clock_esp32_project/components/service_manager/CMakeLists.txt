idf_component_register(SRCS "service_manager.c"
                    INCLUDE_DIRS "."
                    REQUIRES nvs wifi ntp
)
