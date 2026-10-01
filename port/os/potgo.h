#ifndef FA18_OS_POTGO_H
#define FA18_OS_POTGO_H

#include <stdint.h>

/* potgo.resource WritePotgo's word-exact mask and cached-word operations. */
uint16_t fa18_os_potgo_requested(uint16_t word, uint16_t mask);
uint16_t fa18_os_potgo_retained(uint16_t cached, uint16_t inverse_mask);
uint16_t fa18_os_potgo_merge(uint16_t requested, uint16_t retained);
uint16_t fa18_os_potgo_cached(uint16_t output);

#endif
