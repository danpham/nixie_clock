#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

typedef enum {
    ROTARY_ENCODER_EVENT_INCREMENT = 0,
    ROTARY_ENCODER_EVENT_DECREMENT = 1,
    ROTARY_ENCODER_EVENT_NONE = 2,
    ROTARY_ENCODER_EVENT_COUNT
} rotary_encoder_event_t;

rotary_encoder_event_t process_rotary_encoder(uint8_t lastA, uint8_t lastB, uint8_t currentA, uint8_t currentB);

#endif