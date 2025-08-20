idf_component_register(SRCS "main.c" "clock_task.c"
                       INCLUDE_DIRS "."
                       REQUIRES uart hv5622 display clock)