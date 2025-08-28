#define UNITY_INCLUDE_SETUP_STUBS
#define UNITY_VERBOSE 1
#include "unity.h"
#include "display.h"

void setUp(void) {}
void tearDown(void) {}

void test_shift_compute_zero(void) {
    uint8_t val = shift_compute(0);
    printf("shift_compute(0) = %u\n", val);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, val, "shift_compute(0) should return 0");
}

void test_shift_compute_positive(void) {
    uint8_t val;

    val = shift_compute(1);
    printf("shift_compute(1) = %u\n", val);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(9, val, "shift_compute(1) should return 9");

    val = shift_compute(5);
    printf("shift_compute(5) = %u\n", val);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(5, val, "shift_compute(5) should return 5");

    val = shift_compute(9);
    printf("shift_compute(9) = %u\n", val);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, val, "shift_compute(9) should return 1");
}

void test_shift_compute_max(void) {
    uint8_t val;

    val = shift_compute(10);
    printf("shift_compute(10) = %u\n", val);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, val, "shift_compute(10) should return 0");

    val = shift_compute(20);
    printf("shift_compute(20) = %u\n", val);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, val, "shift_compute(20) should return 0");
}
