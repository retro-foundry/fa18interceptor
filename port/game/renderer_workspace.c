/* Source renderer workspace operations; no device registers. */
#include "render_state.h"
#define BLOCK_BYTES 40

void clear_renderer_blocks(gaddr blocks) {
    gaddr block[4];
    int i, n;
    for (i = 0; i < 4; i++) block[i] = rd_u32(blocks + (gaddr)(4 * i));
    for (i = 0; i < 4; i++)
        for (n = 0; n < BLOCK_BYTES; n += 4) wr_u32(block[i] + (gaddr)n, 0);
}

void fill_column(gaddr *dest, uint16_t value, int16_t stride) {
    int i;
    for (i = 0; i < 16; i++) {
        wr_u16(*dest, value);
        *dest += (gaddr)(int32_t)stride;
    }
}
