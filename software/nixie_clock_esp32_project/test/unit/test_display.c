#include "unity.h"
#include "display.h"

typedef struct {
    uint8_t h, m, s;
} time_case_t;

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

    val = shift_compute(255);
    printf("shift_compute(255) = %u\n", val);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, val, "shift_compute(255) should return 0");
}

void print_uint64_binary(uint64_t val) {
    for (int byte = 7; byte >= 0; byte--) {
        uint8_t b = (val >> (byte * 8)) & 0xFF;
        for (int bit = 7; bit >= 0; bit--) {
            printf("%u", (b >> bit) & 1);
        }
        printf(" ");
    }
    printf("\n");
}

void test_encode_time_binary(void) {
    time_case_t cases[4] = {
        {0,0,0},
        {12,34,56},
        {23,59,59},
        {1,2,3}
    };

    /* Display leading zero, no dots */
    for (int i = 0; i < 4; i++) {
        uint64_t val = encode_time(cases[i].h, cases[i].m, cases[i].s, 0, 0, 0, 0, 1);
        printf("\nencode_time(%u,%u,%u,0,0,0,0,1) = 0x%016llX\n",
               cases[i].h, cases[i].m, cases[i].s, val);
        print_uint64_binary(val);
        TEST_ASSERT_TRUE_MESSAGE(val != 0, "encode_time should not return 0");
    }

    /* Display leading zero, with dots */
    for (int i = 0; i < 4; i++) {
        uint64_t val = encode_time(cases[i].h, cases[i].m, cases[i].s, 1, 1, 0, 0, 1);
        printf("\nencode_time(%u,%u,%u,1,1,0,0,1) = 0x%016llX\n",
               cases[i].h, cases[i].m, cases[i].s, val);
        print_uint64_binary(val);
        TEST_ASSERT_TRUE_MESSAGE(val != 0, "encode_time should not return 0");
    }

    /* No leading zero, with dots */
    for (int i = 0; i < 4; i++) {
        uint64_t val = encode_time(cases[i].h, cases[i].m, cases[i].s, 1, 1, 0, 0, 0);
        printf("\nencode_time(%u,%u,%u,1,1,0,0,0) = 0x%016llX\n",
               cases[i].h, cases[i].m, cases[i].s, val);
        print_uint64_binary(val);
        TEST_ASSERT_TRUE_MESSAGE(val != 0, "encode_time should not return 0");
    }

    /* All dots, with leading zero */
    uint64_t val = encode_time(cases[0].h, cases[0].m, cases[0].s, 1, 1, 1, 1, 1);
    printf("\nencode_time(%u,%u,%u,1,1,1,1,1) = 0x%016llX\n",
               cases[0].h, cases[0].m, cases[0].s, val);
    print_uint64_binary(val);
    TEST_ASSERT_TRUE_MESSAGE(val != 0, "encode_time should not return 0");
}

void test_display_pattern_1(void) {
    for (uint8_t i = 0; i < 10; i++) {
        uint64_t val = display_pattern_1_get(i);
         printf("Test %i: ", i);
        print_uint64_binary(val);
        char msg[50];
        snprintf(msg, sizeof(msg), "encode_time should not return 0 for step %d", i);
        TEST_ASSERT_TRUE_MESSAGE(val != 0, msg);
    }
}