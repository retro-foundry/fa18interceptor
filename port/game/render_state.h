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

/* Restart the previous blit with a new BLTCON0 and C/D pointer. */
void restart_blit_cd(uint16_t con0, uint32_t cd, uint16_t size);

/* Restart the previous blit with a new BLTCON0, A pointer and D pointer. */
void restart_blit_ad(uint16_t con0, uint32_t a, uint32_t d, uint16_t size);

#endif
