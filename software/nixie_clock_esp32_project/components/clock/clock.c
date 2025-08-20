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
    if(clk->seconds >= 60) {
        clk->seconds = 0;
        clk->minutes++;
        if(clk->minutes >= 60) {
            clk->minutes = 0;
            clk->hours++;
            if(clk->hours >= 24) {
                clk->hours = 0;
            }
        }
    }
}

void clock_set(clock_t *clk, uint8_t h, uint8_t m, uint8_t s) {
    clk->hours = h % 24;
    clk->minutes = m % 60;
    clk->seconds = s % 60;
}
