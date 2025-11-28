#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/
typedef uint8_t rotary_encoder_event_t;
#define ROTARY_ENCODER_EVENT_INCREMENT   ((rotary_encoder_event_t)0U)
#define ROTARY_ENCODER_EVENT_DECREMENT   ((rotary_encoder_event_t)1U)
#define ROTARY_ENCODER_EVENT_NONE        ((rotary_encoder_event_t)2U)
#define ROTARY_ENCODER_EVENT_COUNT       ((rotary_encoder_event_t)3U)

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
rotary_encoder_event_t process_rotary_encoder(uint8_t lastA, uint8_t lastB, uint8_t currentA, uint8_t currentB);

#endif // ROTARY_ENCODER_H