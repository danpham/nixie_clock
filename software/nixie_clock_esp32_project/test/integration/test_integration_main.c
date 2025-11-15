#include <stdint.h>
#include "unity.h"
#include "clock.h"
#include "display.h"
#include "../common/hv5622_mock.h"

// ------------------- Mock hardware -------------------
// Variable to store what would have been sent to the HV5622
/* MISRA C:2012 Rule 8.5 deviation:
   last_sent_data is accessed from multiple files for integration testing */
extern uint64_t last_sent_data;

// ------------------- Test variables -------------------
static myclock_t clk;

// ------------------- SetUp / TearDown -------------------
void setUp(void) {
    clock_init(&clk, 0, 0, 0);
    last_sent_data = 0ULL;
}

void tearDown(void) {
}

// ------------------- Tests -------------------
// Basic test: the current time is correctly encoded and sent
void test_clock_to_display_basic(void) {
    clock_init(&clk, 12, 34, 56);
    display_set_time(clk.hours, clk.minutes, clk.seconds, 1, 1);

    uint64_t expected = encode_time(12, 34, 56, 1, 1, 0, 0);
    TEST_ASSERT_EQUAL_UINT64(expected, last_sent_data);
}

// Test progression of one second and with dot1 and dot2 off
void test_clock_tick_one_second(void) {
    clock_init(&clk, 1, 2, 3);
    clock_tick(&clk); // 1:02:04

    display_set_time(clk.hours, clk.minutes, clk.seconds, 0, 0);

    uint64_t expected = encode_time(1, 2, 4, 0, 0, 0, 0);
    TEST_ASSERT_EQUAL_UINT64(expected, last_sent_data);
}

// Test rollover seconds → minutes
void test_clock_tick_rollover_seconds(void) {
    clock_init(&clk, 1, 2, 59);
    clock_tick(&clk); // 1:03:00

    display_set_time(clk.hours, clk.minutes, clk.seconds, 1, 1);

    uint64_t expected = encode_time(1, 3, 0, 1, 1, 0, 0);
    TEST_ASSERT_EQUAL_UINT64(expected, last_sent_data);
}

// Test rollover minutes → hours
void test_clock_tick_rollover_minutes(void) {
    clock_init(&clk, 1, 59, 59);
    clock_tick(&clk); // 2:00:00

    display_set_time(clk.hours, clk.minutes, clk.seconds, 1, 1);

    uint64_t expected = encode_time(2, 0, 0, 1, 1, 0, 0);
    TEST_ASSERT_EQUAL_UINT64(expected, last_sent_data);
}

// Test rollover hours → 0
void test_clock_tick_rollover_hours(void) {
    clock_init(&clk, 23, 59, 59);
    clock_tick(&clk); // 0:00:00

    display_set_time(clk.hours, clk.minutes, clk.seconds, 1, 1);

    uint64_t expected = encode_time(0, 0, 0, 1, 1, 0, 0);
    TEST_ASSERT_EQUAL_UINT64(expected, last_sent_data);
}

// ------------------- Main -------------------

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_clock_to_display_basic);
    RUN_TEST(test_clock_tick_one_second);
    RUN_TEST(test_clock_tick_rollover_seconds);
    RUN_TEST(test_clock_tick_rollover_minutes);
    RUN_TEST(test_clock_tick_rollover_hours);

    return UNITY_END();
}
