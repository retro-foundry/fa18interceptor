#ifndef FA18_GAME_RENDER_STATE_H
#define FA18_GAME_RENDER_STATE_H

#include <stdint.h>

#include "memory.h"

/* Clear the four 40-byte renderer state blocks listed at `blocks`. */
void clear_renderer_blocks(gaddr blocks);

/* Start an A/B/C/D blit: C and D share one pointer. */
void start_blit(uint16_t con0, uint32_t a, uint32_t b, uint32_t cd, uint16_t size);

/* Write `value` 16 times, `stride` bytes apart, from `*dest`; advances it. */
void fill_column(gaddr *dest, uint16_t value, int16_t stride);

#endif
