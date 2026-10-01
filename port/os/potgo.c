#include "potgo.h"

uint16_t fa18_os_potgo_requested(uint16_t word, uint16_t mask) {
    return word & mask;
}

uint16_t fa18_os_potgo_retained(uint16_t cached, uint16_t inverse_mask) {
    return cached & inverse_mask;
}

uint16_t fa18_os_potgo_merge(uint16_t requested, uint16_t retained) {
    return requested | retained;
}

uint16_t fa18_os_potgo_cached(uint16_t output) {
    return output & 0xFF00u;
}
