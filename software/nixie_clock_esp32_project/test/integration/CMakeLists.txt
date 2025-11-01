# Exécutable de tests d’intégration
add_executable(native_integration_tests
    test_integration_main.c
    ../common/hv5622_mock.c
    ../../components/display/display.c
    ../../components/clock/clock.c
)

# Inclure les headers nécessaires
include_directories(
    ../../components/display
    ../../components/clock
    ../../components/hv5622
    C:/Espressif/frameworks/esp-idf-v5.5/components/unity/include
    C:/Espressif/frameworks/esp-idf-v5.5/components/unity/unity/src
)

# Ajouter Unity source
target_sources(native_integration_tests PRIVATE
    C:/Espressif/frameworks/esp-idf-v5.5/components/unity/unity/src/unity.c
)

# Définir macro pour les tests
target_compile_definitions(native_integration_tests PRIVATE UNITY_TESTING UNITY_VERBOSE)