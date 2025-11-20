/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include "clock.h"

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/

/******************************************************************
 * 3. Typedef definitions (simple typedef, then enum and structs)
******************************************************************/

/******************************************************************
 * 4. Variable definitions (static then global)
******************************************************************/

/******************************************************************
 * 5. Functions prototypes (static only)
******************************************************************/

/******************************************************************
 * 6. Functions definitions
******************************************************************/

/**
 * @brief Initialize the clock structure with given hours, minutes, and seconds.
 *
 * @param clk Pointer to the clock structure to initialize.
 * @param h Hours value (0-23)
 * @param m Minutes value (0-59)
 * @param s Seconds value (0-59)
 */
void clock_init(myclock_t *clk, uint8_t h, uint8_t m, uint8_t s) {
    clk->hours = h % (uint8_t)24U;
    clk->minutes = m % (uint8_t)60U;
    clk->seconds = s % (uint8_t)60U;
}

/**
 * @brief Increment the clock by one second, rolling over minutes and hours.
 *
 * @param clk Pointer to the clock structure to tick.
 */
void clock_tick(myclock_t *clk) {
    clk->seconds++;
    if (clk->seconds >= (uint8_t)60U) {
        clk->seconds = 0;
        clk->minutes++;
        if (clk->minutes >= (uint8_t)60U) {
            clk->minutes = 0;
            clk->hours++;
            if (clk->hours >= (uint8_t)24U) {
                clk->hours = 0;
            }
        }
    }
}

/**
 * @brief Increment the hours by 1.
 *
 * @param clk Pointer to the clock structure.
 */
void clock_increment_hours(myclock_t *clk) {
    clk->hours = (clk->hours + (uint8_t)1U) % (uint8_t)24U;
}

/**
 * @brief Decrement the hours by 1.
 *
 * @param clk Pointer to the clock structure.
 */
void clock_decrement_hours(myclock_t *clk) {
    clk->hours = (clk->hours - (uint8_t)1U) % (uint8_t)24U;
}

/**
 * @brief Increment the minutes by 1.
 *
 * @param clk Pointer to the clock structure.
 */
void clock_increment_minutes(myclock_t *clk) {
    clk->minutes = (clk->minutes + (uint8_t)1U) % (uint8_t)60U;
}

/**
 * @brief Decrement the minutes by 1.
 *
 * @param clk Pointer to the clock structure.
 */
void clock_decrement_minutes(myclock_t *clk) {
    clk->minutes = (clk->minutes - (uint8_t)1U) % (uint8_t)60U;
}