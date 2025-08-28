#ifndef DISPLAY_H
#define DISPLAY_H

/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include <stdint.h>
#ifndef UNITY_TESTING
#include "hv5622.h"
#endif

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
 * 6. Functions definitions (public API in .c)
******************************************************************/
// Initialize the display
void display_init(void);

// Display the time (hh:mm:ss) and dot1, dot2
void display_set_time(uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t dot1, uint8_t dot2);

#ifdef UNITY_TESTING
uint8_t shift_compute(uint8_t number);
uint64_t encode_time(uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t dot1, uint8_t dot2, uint8_t nixie3_dot, uint8_t nixie6_dot);
#endif

#endif // DISPLAY_H
