idf_component_register(SRCS "hv5622.c"
                    INCLUDE_DIRS "."
                    REQUIRES driver  # <-- use SPI driver from esp-idf
)
