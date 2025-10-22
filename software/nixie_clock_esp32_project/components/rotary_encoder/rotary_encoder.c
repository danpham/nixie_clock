/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include <stdint.h>
#include "rotary_encoder.h"

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/


/******************************************************************
 * 3. Typedef definitions (simple typedef, then enum and structs)
******************************************************************/
typedef enum {
    ROTARY_ENCODER_CODE_0 = 0,
    ROTARY_ENCODER_CODE_1 = 1,
    ROTARY_ENCODER_CODE_2 = 2,
    ROTARY_ENCODER_CODE_3 = 3,
    ROTARY_ENCODER_CODE_COUNT
} rotary_encoder_code_t;

/******************************************************************
 * 4. Variable definitions (static then global)
******************************************************************/
/* Transition table : lastCode -> currentCode 
  +1 = clockwise, -1 = counterclockwise, 0 = no movement */
static const int8_t transition_table[4][4] = {
    {  0, -1, +1,  0 },
    { +1,  0,  0, -1 },
    { -1,  0,  0, +1 },
    {  0, +1, -1,  0 }
};

/******************************************************************
 * 5. Functions prototypes (static only)
******************************************************************/

/**
 * @brief Calculates the rotary encoder event
 * @param lastA Last state of channel A (0 or 1)
 * @param lastB Last state of channel B (0 or 1)
 * @param currentA Current state of channel A (0 or 1)
 * @param currentB Current state of channel B (0 or 1)
 * @return The corresponding event (increment, decrement, or none)
 */
rotary_encoder_event_t process_rotary_encoder(int8_t lastA, int8_t lastB, int8_t currentA, int8_t currentB)
{
    int8_t lastCode = 0;
    int8_t currentCode = 0;
    int8_t direction = 0;

    /* Encode states into 0..3 */
    lastCode = (lastA << 1) | lastB;
    currentCode = (currentA << 1) | currentB;

    /* Safety check: indices must be 0..3 */
    if (lastCode < 0 || lastCode > 3 || currentCode < 0 || currentCode > 3) {
        return ROTARY_ENCODER_EVENT_NONE;
    }

    direction = transition_table[lastCode][currentCode];

    if (direction > 0) return ROTARY_ENCODER_EVENT_INCREMENT;
    if (direction < 0) return ROTARY_ENCODER_EVENT_DECREMENT;
    
    return ROTARY_ENCODER_EVENT_NONE;
}