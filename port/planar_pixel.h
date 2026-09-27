#ifndef FA18_PLANAR_PIXEL_H
#define FA18_PLANAR_PIXEL_H

#include <stdint.h>

#include "video.h"

typedef enum {
    FA18_PLANAR_PIXEL_PRIMARY,
    FA18_PLANAR_PIXEL_TWO_ROWS
} FA18PlanarPixelTable;

/* Source state from `$C45954`, `$C456E7`, `$C456E8`, and `$C456EB`.
 * `$C2F688-$C2FA6F` writes four lanes; a native fifth palette-index bit is
 * deliberately preserved. */
typedef struct {
    uint8_t draw_mode;
    uint8_t active_plane_mask;
    int16_t output_xor_enable;
    uint8_t output_xor_plane_mask;
} FA18PlanarPixelState;

/* `$C2F688-$C2FA6F`: apply one bounded four-lane word-mask operation.
 * Returns zero after a write, one for the original nonpositive-y no-op, or
 * minus one outside the native page model. */
int fa18_apply_planar_pixel_mask(FA18Video *video,
                                 const FA18PlanarPixelState *state,
                                 FA18PlanarPixelTable table, int x, int y);

#endif
