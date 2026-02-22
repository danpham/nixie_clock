#include "unity.h"
#include "rotary_encoder.h"

typedef struct {
    uint8_t lastA;
    uint8_t lastB;
    uint8_t currentA;
    uint8_t currentB;
    rotary_encoder_event_t expected;
} rotary_encoder_test_t;

void test_rotary_encoder(void) {
    /* Test vectors */
    rotary_encoder_test_t tests[] = {
        /* No move */
        {0, 0, 0, 0, ROTARY_ENCODER_EVENT_NONE},
        {0, 1, 0, 1, ROTARY_ENCODER_EVENT_NONE},
        {1, 0, 1, 0, ROTARY_ENCODER_EVENT_NONE},
        {1, 1, 1, 1, ROTARY_ENCODER_EVENT_NONE},
        /* Increments */
        {0, 0, 1, 0, ROTARY_ENCODER_EVENT_DECREMENT},
        {0, 1, 0, 0, ROTARY_ENCODER_EVENT_DECREMENT},
        {1, 0, 1, 1, ROTARY_ENCODER_EVENT_DECREMENT},
        {1, 1, 0, 1, ROTARY_ENCODER_EVENT_DECREMENT},
        /* Decrements */
        {0, 0, 0, 1, ROTARY_ENCODER_EVENT_INCREMENT},
        {0, 1, 1, 1, ROTARY_ENCODER_EVENT_INCREMENT},
        {1, 0, 0, 0, ROTARY_ENCODER_EVENT_INCREMENT},
        {1, 1, 1, 0, ROTARY_ENCODER_EVENT_INCREMENT},
        /* Bad states: no move */
        {1, 1, 0, 0, ROTARY_ENCODER_EVENT_NONE},
        {1, 0, 0, 1, ROTARY_ENCODER_EVENT_NONE},
        {0, 1, 1, 0, ROTARY_ENCODER_EVENT_NONE},
        {0, 0, 1, 1, ROTARY_ENCODER_EVENT_NONE},
    };

    const size_t n = sizeof(tests) / sizeof(tests[0]);

    for (size_t i = 0; i < n; i++) {
        rotary_encoder_test_t *t = &tests[i];
        rotary_encoder_event_t ev = process_rotary_encoder(
            t->lastA, t->lastB, t->currentA, t->currentB
        );

        TEST_ASSERT_EQUAL_UINT8_MESSAGE(
            t->expected, ev, "Rotary encoder test failed"
        );
    }
}