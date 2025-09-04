/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include "display.h"
#ifdef UNITY_TESTING
#include "hv5622_mock.h"
#else
#include "hv5622.h"
#endif

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/
#ifdef UNITY_TESTING
#define NOT_STATIC
#else
#define NOT_STATIC static
#endif

/******************************************************************
 * 3. Typedef definitions (simple typedef, then enum and structs)
******************************************************************/


/******************************************************************
 * 4. Variable definitions (static then global)
******************************************************************/


/******************************************************************
 * 5. Functions prototypes (static only)
******************************************************************/
NOT_STATIC uint8_t shift_compute(uint8_t number);
NOT_STATIC uint64_t encode_time(uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t dot1, uint8_t dot2, uint8_t nixie3_dot, uint8_t nixie6_dot);
NOT_STATIC uint64_t encode_time_digits(uint8_t * nixies, uint8_t dot1, uint8_t dot2, uint8_t nixie3_dot, uint8_t nixie6_dot);
NOT_STATIC uint64_t display_pattern_1_get(uint8_t step);

/******************************************************************
 * 6. Functions definitions
******************************************************************/
NOT_STATIC uint8_t shift_compute(uint8_t number) {
    uint8_t shift_number = 0;

    if  ((number > 0) && (number <= 10))
    {
       shift_number = 10 - number;
    }

    return shift_number;
}

NOT_STATIC uint64_t encode_time(uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t dot1, uint8_t dot2, uint8_t nixie3_dot, uint8_t nixie6_dot) {
    uint8_t nixies[6];

    // Split
    nixies[0] = hours / 10;
    nixies[1] = hours % 10;
    nixies[2] = minutes / 10;
    nixies[3] = minutes % 10;
    nixies[4] = seconds / 10;
    nixies[5] = seconds % 10;

    return encode_time_digits(nixies, dot1, dot2, nixie3_dot, nixie6_dot);
}

NOT_STATIC uint64_t encode_time_digits(uint8_t * nixies, uint8_t dot1, uint8_t dot2, uint8_t nixie3_dot, uint8_t nixie6_dot) {
    uint64_t data = 0;
 
    data |= ((uint64_t)dot1 & 0x01) << 11;
    data |= ((uint64_t)dot2 & 0x01) << 53;
    data |= ((uint64_t)nixie3_dot & 0x01);
    data |= ((uint64_t)nixie6_dot & 0x01) << 32;
    data |= (uint64_t)0x01 << shift_compute(nixies[0]) << 22; // hour hi
    data |= (uint64_t)0x01 << shift_compute(nixies[1]) << 12; // hour low   
    data |= (uint64_t)0x01 << shift_compute(nixies[2]) << 1; // minute hi
    data |= (uint64_t)0x01 << shift_compute(nixies[3]) << 54; // minute lo
    data |= (uint64_t)0x01 << shift_compute(nixies[4]) << 43; // second hi
    data |= (uint64_t)0x01 << shift_compute(nixies[5]) << 33; // second low

    return data;
}

NOT_STATIC uint64_t display_pattern_1_get(uint8_t step) {
    uint8_t nixies[6];
    step %= 10;
    
    for (uint8_t i = 0; i < sizeof(nixies); ++i) {
        nixies[i] = step;
    }

    return encode_time_digits(nixies, 1, 1, 1, 1);
}

void display_init(void) {
    hv5622_init();
}

void display_set_time(uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t dot1, uint8_t dot2) {
    hv5622_send64(encode_time(hours, minutes, seconds, dot1, dot2, 0, 0));
}

void display_set_pattern_1(uint8_t step) {
    hv5622_send64(display_pattern_1_get(step));
}