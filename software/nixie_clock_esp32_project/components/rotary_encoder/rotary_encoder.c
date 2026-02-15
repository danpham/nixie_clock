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

/******************************************************************
 * 4. Variable definitions (static then global)
******************************************************************/

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
rotary_encoder_event_t process_rotary_encoder(uint8_t lastA, uint8_t lastB, uint8_t currentA, uint8_t currentB)
{
    int8_t lastCode = 0;
    int8_t currentCode = 0;
    /* Transition table : lastCode -> currentCode 
      +1 = clockwise, -1 = counterclockwise, 0 = no movement */
    static const int8_t transition_table[4][4] = {
        {  0, +1, -1,  0 },
        { -1,  0,  0, +1 },
        { +1,  0,  0, -1 },
        {  0, -1, +1,  0 }
    };
    
    rotary_encoder_event_t ret = ROTARY_ENCODER_EVENT_NONE;

    /* Encode states into 0..3 */
    lastCode = (lastA << 1) | lastB;
    currentCode = (currentA << 1) | currentB;

    /* Safety check: indices must be 0..3 */
    if ((lastCode < 0) || (lastCode > 3) || (currentCode < 0) || (currentCode > 3)) {
        ret = ROTARY_ENCODER_EVENT_NONE;
    }
    else
    {
        int8_t direction = 0;
        direction = transition_table[lastCode][currentCode];
        
        if (direction > 0) {
          ret = ROTARY_ENCODER_EVENT_INCREMENT;
        }
        else if (direction < 0) {
          ret = ROTARY_ENCODER_EVENT_DECREMENT;
        }
        else {
          ret = ROTARY_ENCODER_EVENT_NONE;
        }
    }

    return ret;
}