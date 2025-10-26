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
void clock_init(clock_t *clk, uint8_t h, uint8_t m, uint8_t s) {
    clk->hours = h;
    clk->minutes = m;
    clk->seconds = s;
}

void clock_tick(clock_t *clk) {
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

void clock_increment_hours(clock_t *clk) {
    clk->hours = (clk->hours + (uint8_t)1U) % (uint8_t)24U;
}

void clock_decrement_hours(clock_t *clk) {
    clk->hours = (clk->hours - (uint8_t)1U) % (uint8_t)24U;
}

void clock_increment_minutes(clock_t *clk) {
    clk->minutes = (clk->minutes + (uint8_t)1U) % (uint8_t)60U;
}

void clock_decrement_minutes(clock_t *clk) {
    clk->minutes = (clk->minutes - (uint8_t)1U) % (uint8_t)60U;
}