#include "unity.h"
#include "clock.h"

static clock_t clk;

// Test initialization with clock_init
void test_clock_init_sets_values(void) {
    clock_init(&clk, 12, 34, 56);
    TEST_ASSERT_EQUAL_UINT8(12, clk.hours);
    TEST_ASSERT_EQUAL_UINT8(34, clk.minutes);
    TEST_ASSERT_EQUAL_UINT8(56, clk.seconds);
}

// Simple tick test
void test_clock_tick_increments_seconds(void) {
    clock_set(&clk, 1, 2, 3);
    clock_tick(&clk);
    TEST_ASSERT_EQUAL_UINT8(4, clk.seconds);
    TEST_ASSERT_EQUAL_UINT8(2, clk.minutes);
    TEST_ASSERT_EQUAL_UINT8(1, clk.hours);
}

// Test seconds rollover → minutes
void test_clock_tick_rollover_seconds_to_minutes(void) {
    clock_set(&clk, 1, 2, 59);
    clock_tick(&clk);
    TEST_ASSERT_EQUAL_UINT8(0, clk.seconds);
    TEST_ASSERT_EQUAL_UINT8(3, clk.minutes);
    TEST_ASSERT_EQUAL_UINT8(1, clk.hours);
}

// Test minutes rollover → hours
void test_clock_tick_rollover_minutes_to_hours(void) {
    clock_set(&clk, 1, 59, 59);
    clock_tick(&clk);
    TEST_ASSERT_EQUAL_UINT8(0, clk.seconds);
    TEST_ASSERT_EQUAL_UINT8(0, clk.minutes);
    TEST_ASSERT_EQUAL_UINT8(2, clk.hours);
}

// Test hours rollover → 0
void test_clock_tick_rollover_hours_to_zero(void) {
    clock_set(&clk, 23, 59, 59);
    clock_tick(&clk);
    TEST_ASSERT_EQUAL_UINT8(0, clk.seconds);
    TEST_ASSERT_EQUAL_UINT8(0, clk.minutes);
    TEST_ASSERT_EQUAL_UINT8(0, clk.hours);
}

// Test clock_set with modulo wrapping
void test_clock_set_wraps_values(void) {
    clock_set(&clk, 25, 61, 70);
    TEST_ASSERT_EQUAL_UINT8(1, clk.hours);    // 25 % 24 = 1
    TEST_ASSERT_EQUAL_UINT8(1, clk.minutes);  // 61 % 60 = 1
    TEST_ASSERT_EQUAL_UINT8(10, clk.seconds); // 70 % 60 = 10
}

// Test incrementing hours
void test_clock_increment_hours(void) {
    clock_set(&clk, 23, 0, 0);
    clock_increment_hours(&clk);
    TEST_ASSERT_EQUAL_UINT8(0, clk.hours);
}

// Test incrementing minutes
void test_clock_increment_minutes(void) {
    clock_set(&clk, 0, 59, 0);
    clock_increment_minutes(&clk);
    TEST_ASSERT_EQUAL_UINT8(0, clk.minutes);
}
