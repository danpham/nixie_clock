#ifndef CLOCK_H
#define CLOCK_H

/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include <stdint.h>
#include <stdbool.h>

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/

/******************************************************************
 * 3. Typedef definitions (simple typedef, then enum and structs)
******************************************************************/
typedef struct {
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
} myclock_t;

/******************************************************************
 * 4. Variable definitions (static then global)
******************************************************************/

/******************************************************************
 * 5. Functions prototypes (static only)
******************************************************************/

/******************************************************************
 * 6. Functions definitions (public API in .c)
******************************************************************/
// Initialize the clock
void clock_init(myclock_t *clk, uint8_t h, uint8_t m, uint8_t s);

// Advance the clock by one second (called by tick)
void clock_tick(myclock_t *clk);

// Increment hours immediately (e.g., called from button)
void clock_increment_hours(myclock_t *clk);

// Decrement hours immediately (e.g., called from button)
void clock_decrement_hours(myclock_t *clk);

// Increment minutes immediately (e.g., called from button)
void clock_increment_minutes(myclock_t *clk);

// Decrement minutes immediately (e.g., called from button)
void clock_decrement_minutes(myclock_t *clk);

#endif // CLOCK_H