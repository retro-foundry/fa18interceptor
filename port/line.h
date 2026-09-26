#ifndef FA18_LINE_H
#define FA18_LINE_H

#include <stdint.h>

#include "renderer.h"

/* Caller-level endpoints proved at $C2FA7E. They are screen coordinates, not
 * pointers or Amiga register values. */
typedef struct {
    int16_t x0;
    int16_t y0;
    int16_t x1;
    int16_t y1;
} FA18LineSegment;

/* The source selects one bit value for every active plane. The negative mode
 * selects control_plane_bits; a nonnegative mode selects plane_bits. */
typedef struct {
    uint8_t active_plane_mask;
    int16_t plane_mode;
    uint8_t plane_bits;
    uint8_t control_plane_bits;
} FA18LineStyle;

typedef struct {
    uint16_t bltcon1;
    uint16_t bltbmod;
    uint16_t bltamod;
    uint16_t bltsize;
    uint32_t destination_byte_offset;
    uint8_t active_plane_mask;
} FA18LinePacket;

/* Frame-395 `$C2FB7A` packet values after CPU line preparation. */
int fa18_validate_line_packet(const FA18LinePacket *packet);

/* Native translation of the proved, in-bounds $C2FA7E line-mode path. The
 * original first endpoint row is advanced before line-mode stepping. Returns
 * 0 after drawing, 1 when the original start-row limit rejects the segment,
 * or -1 when clipping/out-of-buffer behavior has not yet been proved.
 */
int fa18_draw_line(FA18IndexedFrameBuffer *framebuffer,
                   const FA18LineStyle *style, FA18LineSegment segment,
                   int16_t row_limit);

#endif
