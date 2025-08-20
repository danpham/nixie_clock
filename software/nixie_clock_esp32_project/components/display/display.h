#ifndef DISPLAY_H
#define DISPLAY_H

/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include <stdint.h>
#include "hv5622.h"  // Assumes HV5622 is managed in this component


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

// Display the time (hh:mm:ss)
void display_set_time(uint8_t hours, uint8_t minutes, uint8_t seconds);

#endif // DISPLAY_H
