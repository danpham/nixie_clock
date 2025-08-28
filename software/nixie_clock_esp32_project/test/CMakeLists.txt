add_executable(native_tests
    test_display.c
    test_clock.c
    test_main.c
    ../components/hv5622/hv5622_mock.c
    ../components/display/display.c
    ../components/clock/clock.c
)

include_directories(
    ../components/hv5622
    ../components/display
    ../components/clock
    C:/Espressif/frameworks/esp-idf-v5.5/components/unity/include
    C:/Espressif/frameworks/esp-idf-v5.5/components/unity/unity/src
)

target_sources(native_tests PRIVATE
    C:/Espressif/frameworks/esp-idf-v5.5/components/unity/unity/src/unity.c
)

target_compile_definitions(native_tests PRIVATE UNITY_TESTING UNITY_VERBOSE)
