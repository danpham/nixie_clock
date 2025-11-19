#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/

/******************************************************************
 * 3. Typedef definitions (simple typedef, then enum and structs)
******************************************************************/
typedef enum {
    ROTARY_ENCODER_EVENT_INCREMENT = 0U,
    ROTARY_ENCODER_EVENT_DECREMENT = 1U,
    ROTARY_ENCODER_EVENT_NONE = 2U,
    ROTARY_ENCODER_EVENT_COUNT = 3U
} rotary_encoder_event_t;

/******************************************************************
 * 4. Variable definitions (static then global)
******************************************************************/

/******************************************************************
 * 5. Functions prototypes (static only)
******************************************************************/

/******************************************************************
 * 6. Functions definitions
******************************************************************/
rotary_encoder_event_t process_rotary_encoder(uint8_t lastA, uint8_t lastB, uint8_t currentA, uint8_t currentB);

#endif // ROTARY_ENCODER_H