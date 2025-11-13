#include "unity.h"
#include "clock.h"

static myclock_t system_clock_ticks;

// Test initialization with clock_init
void test_clock_init_sets_values(void) {
    clock_init(&system_clock_ticks, 12, 34, 56);
    TEST_ASSERT_EQUAL_UINT8(12, system_clock_ticks.hours);
    TEST_ASSERT_EQUAL_UINT8(34, system_clock_ticks.minutes);
    TEST_ASSERT_EQUAL_UINT8(56, system_clock_ticks.seconds);
}

// Simple tick test
void test_clock_tick_increments_seconds(void) {
    clock_init(&system_clock_ticks, 1, 2, 3);
    clock_tick(&system_clock_ticks);
    TEST_ASSERT_EQUAL_UINT8(4, system_clock_ticks.seconds);
    TEST_ASSERT_EQUAL_UINT8(2, system_clock_ticks.minutes);
    TEST_ASSERT_EQUAL_UINT8(1, system_clock_ticks.hours);
}

// Test seconds rollover → minutes
void test_clock_tick_rollover_seconds_to_minutes(void) {
    clock_init(&system_clock_ticks, 1, 2, 59);
    clock_tick(&system_clock_ticks);
    TEST_ASSERT_EQUAL_UINT8(0, system_clock_ticks.seconds);
    TEST_ASSERT_EQUAL_UINT8(3, system_clock_ticks.minutes);
    TEST_ASSERT_EQUAL_UINT8(1, system_clock_ticks.hours);
}

// Test minutes rollover → hours
void test_clock_tick_rollover_minutes_to_hours(void) {
    clock_init(&system_clock_ticks, 1, 59, 59);
    clock_tick(&system_clock_ticks);
    TEST_ASSERT_EQUAL_UINT8(0, system_clock_ticks.seconds);
    TEST_ASSERT_EQUAL_UINT8(0, system_clock_ticks.minutes);
    TEST_ASSERT_EQUAL_UINT8(2, system_clock_ticks.hours);
}

// Test hours rollover → 0
void test_clock_tick_rollover_hours_to_zero(void) {
    clock_init(&system_clock_ticks, 23, 59, 59);
    clock_tick(&system_clock_ticks);
    TEST_ASSERT_EQUAL_UINT8(0, system_clock_ticks.seconds);
    TEST_ASSERT_EQUAL_UINT8(0, system_clock_ticks.minutes);
    TEST_ASSERT_EQUAL_UINT8(0, system_clock_ticks.hours);
}

// Test clock_set with modulo wrapping
void test_clock_set_wraps_values(void) {
    clock_init(&system_clock_ticks, 25, 61, 70);
    TEST_ASSERT_EQUAL_UINT8(1, system_clock_ticks.hours);    // 25 % 24 = 1
    TEST_ASSERT_EQUAL_UINT8(1, system_clock_ticks.minutes);  // 61 % 60 = 1
    TEST_ASSERT_EQUAL_UINT8(10, system_clock_ticks.seconds); // 70 % 60 = 10
}

// Test incrementing hours
void test_clock_increment_hours(void) {
    clock_init(&system_clock_ticks, 23, 0, 0);
    clock_increment_hours(&system_clock_ticks);
    TEST_ASSERT_EQUAL_UINT8(0, system_clock_ticks.hours);
}

// Test incrementing minutes
void test_clock_increment_minutes(void) {
    clock_init(&system_clock_ticks, 0, 59, 0);
    clock_increment_minutes(&system_clock_ticks);
    TEST_ASSERT_EQUAL_UINT8(0, system_clock_ticks.minutes);
}
