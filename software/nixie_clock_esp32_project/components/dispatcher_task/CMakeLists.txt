idf_component_register(SRCS "dispatcher_task.c"
                    INCLUDE_DIRS "."
                    REQUIRES freertos event_bus
)
