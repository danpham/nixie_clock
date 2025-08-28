add_executable(native_tests
    test_display.c
    test_clock.c
    test_main.c
    ../components/display/display.c   # pour inclure shift_compute
    ../components/clock/clock.c   # pour inclure shift_compute
)

include_directories(
    ../components/display
    ../components/clock
    C:/Espressif/frameworks/esp-idf-v5.5/components/unity/include
    C:/Espressif/frameworks/esp-idf-v5.5/components/unity/unity/src
)

target_sources(native_tests PRIVATE
    C:/Espressif/frameworks/esp-idf-v5.5/components/unity/unity/src/unity.c
)

target_compile_definitions(native_tests PRIVATE UNITY_TESTING UNITY_VERBOSE)
