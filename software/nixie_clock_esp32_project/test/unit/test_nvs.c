#include "unity.h"
#include "nvs.h"

void test_nvs_init_should_return_ok(void) {
    TEST_ASSERT_EQUAL(ESP_OK, nvs_init());
}

void test_nvs_save_and_load_int(void) {
    uint8_t value = 123;
    printf("nvs_save_value: input integer = %d\n", value);
    TEST_ASSERT_EQUAL(ESP_OK, nvs_save_value("mode", value));

    uint8_t read_value = 0;
    TEST_ASSERT_EQUAL(ESP_OK, nvs_load_value("mode", &read_value));
    printf("nvs_load_value: output integer = %d\n", read_value);
    TEST_ASSERT_EQUAL(value, read_value);
}

void test_nvs_save_and_load_str(void) {
    const char* ssid = "my_wifi";
    printf("nvs_save_str: input sring = %s\n", ssid);
    TEST_ASSERT_EQUAL(ESP_OK, nvs_save_str("ssid", ssid));

    char buffer[32];
    size_t len = sizeof(buffer);
    TEST_ASSERT_EQUAL(ESP_OK, nvs_load_str("ssid", buffer, &len));
    printf("nvs_load_str: output sring = %s\n", buffer);
    TEST_ASSERT_EQUAL_STRING(ssid, buffer);
}

void test_nvs(void) {
    RUN_TEST(test_nvs_init_should_return_ok);
    RUN_TEST(test_nvs_save_and_load_int);
    RUN_TEST(test_nvs_save_and_load_str);
}
