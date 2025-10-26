#include "hv5622_mock.h"

uint64_t last_sent_data = 0ULL;

void hv5622_init(void) {
}

void hv5622_send64(uint64_t data) {
    last_sent_data = data;
}