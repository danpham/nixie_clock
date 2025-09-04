#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

extern void test_shift_compute_zero(void);
extern void test_shift_compute_positive(void);
extern void test_shift_compute_max(void);
extern void test_encode_time_binary(void);
extern void test_clock_init_sets_values(void);
extern void test_clock_tick_increments_seconds(void);
extern void test_clock_tick_rollover_seconds_to_minutes(void);
extern void test_clock_tick_rollover_minutes_to_hours(void);
extern void test_clock_tick_rollover_hours_to_zero(void);
extern void test_clock_set_wraps_values(void);
extern void test_clock_increment_hours(void);
extern void test_clock_increment_minutes(void);
extern void test_display_pattern_1(void);

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_shift_compute_zero);
    RUN_TEST(test_shift_compute_positive);
    RUN_TEST(test_shift_compute_max);
    RUN_TEST(test_encode_time_binary);
    RUN_TEST(test_clock_init_sets_values);
    RUN_TEST(test_clock_tick_increments_seconds);
    RUN_TEST(test_clock_tick_rollover_seconds_to_minutes);
    RUN_TEST(test_clock_tick_rollover_minutes_to_hours);
    RUN_TEST(test_clock_tick_rollover_hours_to_zero);
    RUN_TEST(test_clock_set_wraps_values);
    RUN_TEST(test_clock_increment_hours);
    RUN_TEST(test_clock_increment_minutes);
    RUN_TEST(test_display_pattern_1);

    return UNITY_END();
}