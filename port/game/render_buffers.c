#include "render_buffers.h"

#include "globals.h"
#include "hardware.h"
#ifdef FA18_NATIVE
#include "native/raster.h"
#endif

static void clear_longs(gaddr buffer, int count) {
    int i;
    for (i = 0; i < count; i++) wr_u32(buffer + (gaddr)(4 * i), 0);
}

void clear_render_buffers(void) {
    int i, used = rd_u8(FIFTH_BUFFER_USED) != 0;
    /* The original clears the buffers interleaved, a long of each at a
     * time; they do not overlap. */
    for (i = 0; i < RENDER_BUFFER_LONGS; i++) {
        int b;
        for (b = 0; b < 4; b++) wr_u32(rd_u32(RENDER_BUFFERS_A + (gaddr)(4 * b)) + (gaddr)(4 * i), 0);
        if (used) wr_u32(rd_u32(RENDER_BUFFERS_A + 16) + (gaddr)(4 * i), 0);
    }
    for (i = 0; i < 5; i++) clear_longs(rd_u32(RENDER_BUFFERS_B + (gaddr)(4 * i)), RENDER_BUFFER_LONGS);
}

void blit_mask_between_planes(void) {
    gaddr table = rd_u32(PAGE_PLANE_TABLE);
    uint32_t offset = rd_u32(POLY_PLANE_OFFSET);
    uint32_t plane0 = rd_u32(table) + offset, plane1 = rd_u32(table + 4) + offset;

#ifdef FA18_NATIVE
    native_raster_copy_masked(rd_u32(POLY_MASK_SOURCE),plane0,plane1,rd_u16(POLY_BLIT_SIZE));
#else
    wait_blitter();
    custom_write(BLTCON0, 0x0FCA);
    custom_write(BLTCON1, 0x0002);
    custom_write_ptr(BLTAPT, rd_u32(POLY_MASK_SOURCE));
    custom_write_ptr(BLTBPT, plane0);
    custom_write_ptr(BLTCPT, plane1);
    custom_write_ptr(BLTDPT, plane1);
    custom_write(BLTSIZE, rd_u16(POLY_BLIT_SIZE));
#endif
}

void clear_page_plane_tops(void) {
    int page, plane, i;
    for (page = 0; page < 2; page++)
        for (plane = 0; plane < 4; plane++) {
            gaddr p = rd_u32(PAGE0_PLANE_TABLE + (gaddr)(16 * page + 4 * plane));
            for (i = 0; i < 10; i++) wr_u32(p + (gaddr)(4 * i), 0);
        }
}

/* The C pointer and modulo the lane blit uses. */
static void lane_source(uint16_t size, uint32_t *c_pointer, int16_t *c_modulo) {
    int16_t row = (int16_t)(rd_s16(POLY_MAX_Y) - rd_s16(REDRAW_STATE_WORD) - 0xB7);
    int16_t modulo = (int16_t)(3 - (int16_t)(size & 0x3F));
    uint32_t c = LANE_PATTERN + (uint32_t)(int32_t)(int16_t)(row * 4) - 2;
    if (modulo != 1 && (int16_t)(rd_s16(POLY_MIN_X) >> 4) == (int16_t)(rd_s16(SPAN_ORIGIN) + 12)) c -= 2;
    *c_pointer = c;
    *c_modulo = modulo;
}

void blit_lane(int16_t plane_offset, int pattern) {
    uint32_t plane = rd_u32(rd_u32(PAGE_PLANE_TABLE) + (gaddr)(int32_t)plane_offset) + rd_u32(POLY_PLANE_OFFSET);
    uint16_t size = rd_u16(POLY_BLIT_SIZE);
    uint32_t c;
    int16_t modulo;

    lane_source(size, &c, &modulo);
#ifdef FA18_NATIVE
    native_raster_lane(rd_u32(POLY_MASK_SOURCE),c,plane,size,modulo,pattern&1);
#else
    wait_blitter();
    custom_write(BLTCON0, (pattern & 1) ? 0x0FEC : 0x0F4C);
    custom_write(BLTCON1, 0x0002);
    custom_write_ptr(BLTAPT, rd_u32(POLY_MASK_SOURCE));
    custom_write_ptr(BLTCPT, c);
    custom_write(BLTCMOD, (uint16_t)modulo);
    custom_write_ptr(BLTBPT, plane);
    custom_write_ptr(BLTDPT, plane);
    custom_write(BLTSIZE, size);
#endif
}
