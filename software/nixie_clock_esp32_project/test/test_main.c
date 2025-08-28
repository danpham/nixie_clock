#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

extern void test_shift_compute_zero(void);
extern void test_shift_compute_positive(void);
extern void test_shift_compute_max(void);
extern void test_encode_time_binary(void);

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_shift_compute_zero);
    RUN_TEST(test_shift_compute_positive);
    RUN_TEST(test_shift_compute_max);
    RUN_TEST(test_encode_time_binary);
    return UNITY_END();
}