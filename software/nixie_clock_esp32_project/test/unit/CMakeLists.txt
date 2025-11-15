add_executable(native_tests
    test_display.c
    test_clock.c
    test_rotary_encoder.c
    test_nvs.c
    test_unit_main.c
    ../common/hv5622_mock.c
    ../common/nvs_mock.c
    ../../components/rotary_encoder/rotary_encoder.c
    ../../components/display/display.c
    ../../components/clock/clock.c
    ../../components/nvs/nvs.c
)

include_directories(
    ../../components/hv5622
    ../../components/rotary_encoder
    ../../components/display
    ../../components/clock
    ../../components/nvs
    ../common/
    C:/Espressif/frameworks/esp-idf-v5.5/components/unity/include
    C:/Espressif/frameworks/esp-idf-v5.5/components/unity/unity/src
)

target_sources(native_tests PRIVATE
    C:/Espressif/frameworks/esp-idf-v5.5/components/unity/unity/src/unity.c
)

target_compile_definitions(native_tests PRIVATE UNITY_TESTING UNITY_VERBOSE)
