#include "rotary_encoder.h"

rotary_encoder_event_t process_rotary_encoder(int lastA, int currentA, int currentB) {
    if (currentA == lastA) return ROTARY_ENCODER_EVENT_NONE;
    return (currentA != currentB) ? ROTARY_ENCODER_EVENT_INCREMENT : ROTARY_ENCODER_EVENT_DECREMENT;
}