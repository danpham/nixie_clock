/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include "display.h"
#include "hv5622.h"


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
static uint64_t encode_time(uint8_t hours, uint8_t minutes, uint8_t seconds);


/******************************************************************
 * 6. Functions definitions
******************************************************************/
static uint64_t encode_time(uint8_t hours, uint8_t minutes, uint8_t seconds) {
    uint64_t data = 0;
    // TODO: encode according to Nixie wiring
    data |= ((uint64_t)hours & 0x3F) << 48;
    data |= ((uint64_t)minutes & 0x3F) << 24;
    data |= ((uint64_t)seconds & 0x3F);
    return data;
}

void display_init(void) {
    hv5622_init();
}

void display_set_time(uint8_t hours, uint8_t minutes, uint8_t seconds) {
    uint64_t data = encode_time(hours, minutes, seconds);
    hv5622_send64(data);
}
