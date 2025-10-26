#ifndef STATIC_ANALYSIS
#ifndef HV5622_MOCK_H
#define HV5622_MOCK_H

#include <stdint.h>
#include <stdbool.h>

extern uint64_t last_sent_data;

void hv5622_init(void);
void hv5622_send64(uint64_t data);

#endif // HV5622_MOCK_H