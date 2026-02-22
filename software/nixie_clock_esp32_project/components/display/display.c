/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include "display.h"
#ifdef UNITY_TESTING
#include "../../test/common/hv5622_mock.h"
#else
#include "../hv5622/hv5622.h"
#endif

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/
#ifdef UNITY_TESTING
#define NOT_STATIC
#else
#define NOT_STATIC static
#endif
#define DISPLAY_NIXIE_COUNT      (6U)
#define DISPLAY_NIXIE_OFF        (0xFFU)

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
NOT_STATIC uint64_t encode_time(uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t dot1, uint8_t dot2, uint8_t nixie3_dot, uint8_t nixie6_dot, uint8_t display_leading_zero);
NOT_STATIC uint64_t encode_time_digits(const uint8_t * nixies, uint8_t dot1, uint8_t dot2, uint8_t nixie3_dot, uint8_t nixie6_dot);
NOT_STATIC uint64_t display_pattern_1_get(uint8_t step);

/******************************************************************
 * 6. Functions definitions
******************************************************************/

/**
 * @brief Compute the shift for a given number (0–9).
 *
 * @param number Number to compute the shift for.
 * @return Calculated shift value.
 */
NOT_STATIC uint8_t shift_compute(uint8_t number) {
    uint8_t shift_number = 0U;

    if  ((number > 0U) && (number <= (uint8_t)9U)) {
       shift_number = (uint8_t)10U - number;
    }

    return shift_number;
}

/**
 * @brief Encode hours, minutes, seconds and dots into a 64-bit value for HV5622.
 *
 * @param hours Hours (0–23)
 * @param minutes Minutes (0–59)
 * @param seconds Seconds (0–59)
 * @param dot1 First dot
 * @param dot2 Second dot
 * @param nixie3_dot Dot for nixie 3
 * @param nixie6_dot Dot for nixie 6
 * @return Encoded 64-bit data for HV5622.
 */
NOT_STATIC uint64_t encode_time(uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t dot1, uint8_t dot2, uint8_t nixie3_dot, uint8_t nixie6_dot, uint8_t display_leading_zero) {
    uint8_t nixies[DISPLAY_NIXIE_COUNT];

    /* Split hours, minutes and seconds into individual digits */
    nixies[0U] = hours / (uint8_t)10U;
    nixies[1U] = hours % (uint8_t)10U;
    nixies[2U] = minutes / (uint8_t)10U;
    nixies[3U] = minutes % (uint8_t)10U;
    nixies[4U] = seconds / (uint8_t)10U;
    nixies[5U] = seconds % (uint8_t)10U;

    /* If first digit is 0 and display_leading_zero is 0, set it to OFF */
    if (display_leading_zero == 0U) {
        if (nixies[0U] == 0U) {
            nixies[0U] = DISPLAY_NIXIE_OFF;
        }
    }

    return encode_time_digits(nixies, dot1, dot2, nixie3_dot, nixie6_dot);
}

/**
 * @brief Encode an array of digits and dots into a 64-bit value.
 *
 * @param nixies Array of 6 digits
 * @param dot1 First dot
 * @param dot2 Second dot
 * @param nixie3_dot Dot for nixie 3
 * @param nixie6_dot Dot for nixie 6
 * @return Encoded 64-bit data
 */
NOT_STATIC uint64_t encode_time_digits(const uint8_t * nixies, uint8_t dot1, uint8_t dot2, uint8_t nixie3_dot, uint8_t nixie6_dot) {
    uint64_t data = 0U;
 
    data |= ((uint64_t)dot1 & (uint64_t)0x01U) << 11U;
    data |= ((uint64_t)dot2 & (uint64_t)0x01U) << 53U;
    data |= ((uint64_t)nixie3_dot & (uint64_t)0x01U);
    data |= ((uint64_t)nixie6_dot & (uint64_t)0x01U) << 32U;
    /* 9 is the last digit that can be displayed, otherwise DISPLAY_NIXIE_OFF */
    if (nixies[0U] < 10U) {
        data |= (uint64_t)0x01 << shift_compute(nixies[0U]) << 22U; // hour hi
    }
    data |= (uint64_t)0x01 << shift_compute(nixies[1U]) << 12U; // hour low
    data |= (uint64_t)0x01 << shift_compute(nixies[2U]) << 1U; // minute hi
    data |= (uint64_t)0x01 << shift_compute(nixies[3U]) << 54U; // minute lo
    data |= (uint64_t)0x01 << shift_compute(nixies[4U]) << 43U; // second hi
    data |= (uint64_t)0x01 << shift_compute(nixies[5U]) << 33U; // second low

    return data;
}

/**
 * @brief Get pattern 1 for display.
 *
 * @param step Step number (0–9)
 * @return Encoded 64-bit pattern
 */
NOT_STATIC uint64_t display_pattern_1_get(uint8_t step) {
    uint8_t nixies[DISPLAY_NIXIE_COUNT];
    uint8_t tmp = (uint8_t)(step % (uint8_t)10U);
    uint8_t display_leading_zero = 1U;

    for (uint8_t i = 0U; i < DISPLAY_NIXIE_COUNT; ++i) {
        nixies[i] = tmp;
    }

    return encode_time_digits(nixies, 1U, 1U, 1U, display_leading_zero);
}

/**
 * @brief Initialize the display.
 */
void display_init(void) {
    hv5622_init();
}

/**
 * @brief Set the current time on the display.
 *
 * @param hours Hours (0–23)
 * @param minutes Minutes (0–59)
 * @param seconds Seconds (0–59)
 * @param dot1 First dot
 * @param dot2 Second dot
 */
void display_set_time(uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t dot1, uint8_t dot2, uint8_t display_leading_zero) {
    hv5622_send64(encode_time(hours, minutes, seconds, dot1, dot2, 0U, 0U, display_leading_zero));
}

/**
 * @brief Set pattern 1 on the display.
 *
 * @param step Step number (0–9)
 */
void display_set_pattern_1(uint8_t step) {
    hv5622_send64(display_pattern_1_get(step));
}