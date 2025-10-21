#include "unity.h"
#include "rotary_encoder.h"

// Test rotary encoder behaviour
void test_rotary_encoder(void) {
    int lastA = 0;
    int currentA = 0;
    int currentB = 0;
    rotary_encoder_event_t ev = process_rotary_encoder(lastA, currentA, currentB);
    TEST_ASSERT_EQUAL_UINT8(ROTARY_ENCODER_EVENT_NONE, ev);

    lastA = 0;
    currentA = 1;
    currentB = 0;
    ev = process_rotary_encoder(lastA, currentA, currentB);
    TEST_ASSERT_EQUAL_UINT8(ROTARY_ENCODER_EVENT_INCREMENT, ev);

    lastA = 0;
    currentA = 1;
    currentB = 1;
    ev = process_rotary_encoder(lastA, currentA, currentB);
    TEST_ASSERT_EQUAL_UINT8(ROTARY_ENCODER_EVENT_DECREMENT, ev);

    lastA = 1;
    currentA = 0;
    currentB = 1;
    ev = process_rotary_encoder(lastA, currentA, currentB);
    TEST_ASSERT_EQUAL_UINT8(ROTARY_ENCODER_EVENT_INCREMENT, ev);
}