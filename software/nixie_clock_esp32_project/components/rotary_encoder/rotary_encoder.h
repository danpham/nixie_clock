#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

typedef enum {
    ROTARY_ENCODER_EVENT_INCREMENT = 0U,
    ROTARY_ENCODER_EVENT_DECREMENT = 1U,
    ROTARY_ENCODER_EVENT_NONE = 2U,
    ROTARY_ENCODER_EVENT_COUNT = 3U
} rotary_encoder_event_t;

rotary_encoder_event_t process_rotary_encoder(uint8_t lastA, uint8_t lastB, uint8_t currentA, uint8_t currentB);

#endif