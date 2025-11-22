idf_component_register(SRCS "dispatcher.c"
                    INCLUDE_DIRS "."
                    REQUIRES freertos event_bus
)
