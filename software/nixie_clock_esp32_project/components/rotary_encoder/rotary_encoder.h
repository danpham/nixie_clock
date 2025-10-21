#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

typedef enum {
    ROTARY_ENCODER_EVENT_INCREMENT = 0,
    ROTARY_ENCODER_EVENT_DECREMENT = 1,
    ROTARY_ENCODER_EVENT_NONE = 2,
    ROTARY_ENCODER_EVENT_COUNT
} rotary_encoder_event_t;

/**
 * @brief Calcule l'événement de l'encodeur rotatif
 * @param lastA Dernier état de la canal A
 * @param currentA État actuel de la canal A
 * @param currentB État actuel de la canal B
 * @return L'événement correspondant (increment, decrement ou none)
 */
rotary_encoder_event_t process_rotary_encoder(int lastA, int currentA, int currentB);

#endif