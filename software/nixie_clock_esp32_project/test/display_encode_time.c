#define UNITY_INCLUDE_SETUP_STUBS
#define UNITY_VERBOSE 1
#include "unity.h"
#include "display.h"
#include <stdint.h>
#include <stdio.h>

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

typedef struct {
    uint8_t h, m, s;
} time_case_t;

void test_encode_time_binary(void) {
    time_case_t cases[4] = {
        {0,0,0},
        {12,34,56},
        {23,59,59},
        {1,2,3}
    };

    for (int i = 0; i < 4; i++) {
        uint64_t val = encode_time(cases[i].h, cases[i].m, cases[i].s, 0, 0, 0, 0);
        printf("\nencode_time(%u,%u,%u,0,0,0,0) = 0x%016llX\n",
               cases[i].h, cases[i].m, cases[i].s, val);
        print_uint64_binary(val);
        TEST_ASSERT_TRUE_MESSAGE(val != 0, "encode_time should not return 0");
    }

    for (int i = 0; i < 4; i++) {
        uint64_t val = encode_time(cases[i].h, cases[i].m, cases[i].s, 1, 1, 0, 0);
        printf("\nencode_time(%u,%u,%u,1,1,0,0) = 0x%016llX\n",
               cases[i].h, cases[i].m, cases[i].s, val);
        print_uint64_binary(val);
        TEST_ASSERT_TRUE_MESSAGE(val != 0, "encode_time should not return 0");
    }

    /* All dots */
    uint64_t val = encode_time(cases[0].h, cases[0].m, cases[0].s, 1, 1, 1, 1);
    printf("\nencode_time(%u,%u,%u,1,1,1,1) = 0x%016llX\n",
               cases[0].h, cases[0].m, cases[0].s, val);
    print_uint64_binary(val);
    TEST_ASSERT_TRUE_MESSAGE(val != 0, "encode_time should not return 0");
}
