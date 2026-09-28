/* Renderer state and blit helpers. */
#include "render_state.h"

#include "hardware.h"

#define BLOCK_BYTES 40

void clear_renderer_blocks(gaddr blocks) {
    gaddr block[4];
    int i, n;
    for (i = 0; i < 4; i++) block[i] = rd_u32(blocks + (gaddr)(4 * i));
    for (i = 0; i < 4; i++)
        for (n = 0; n < BLOCK_BYTES; n += 4) wr_u32(block[i] + (gaddr)n, 0);
}

void start_blit(uint16_t con0, uint32_t a, uint32_t b, uint32_t cd, uint16_t size) {
    custom_write(BLTCON0, con0);
    custom_write_ptr(BLTAPT, a);
    custom_write_ptr(BLTBPT, b);
    custom_write_ptr(BLTCPT, cd);
    custom_write_ptr(BLTDPT, cd);
    custom_write(BLTSIZE, size);
}

void fill_column(gaddr *dest, uint16_t value, int16_t stride) {
    int i;
    for (i = 0; i < 16; i++) {
        wr_u16(*dest, value);
        *dest += (gaddr)(int32_t)stride;
    }
}
