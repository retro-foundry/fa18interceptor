#ifndef FA18_GLYPH_H
#define FA18_GLYPH_H

#include "display.h"

#include <stddef.h>
#include <stdint.h>

/* Native parameters for one `$C330FE` lane. `encoded_shift` retains only the
 * source packet's mode/shift word; the byte stream and selected plane replace
 * the original untyped registers and address values. */
typedef struct {
    uint8_t plane_index;
    size_t first_byte_offset;
    const uint8_t *glyph_bytes;
    size_t glyph_byte_count;
    uint16_t encoded_shift;
    uint16_t row_count;
} FA18GlyphMaskLane;

/* Apply the proved `$C330FE` mask algebra to one native plane. The destination
 * longwords are big-endian display words, separated by the observed 40-byte
 * row stride. Returns -1 for an out-of-page lane. */
int fa18_apply_glyph_mask_lane(FA18PlanarPage *page,
                               const FA18GlyphMaskLane *lane);

/* Native submission contract for `$C33058` after its glyph-table lookup. The
 * original A5 slot family is represented by the observed plane order 4..1;
 * `active_lane_mask` bit 3 controls plane 4 and bit 0 controls plane 1. */
typedef struct {
    const uint8_t *glyph_bytes;
    size_t glyph_byte_count;
    size_t first_byte_offset;
    uint16_t mask_word;
    uint8_t active_lane_mask;
    uint16_t row_count;
} FA18StaticGlyphSubmission;

/* Submit all four `$C33058` lanes after a caller has selected a glyph stream.
 * Returns -1 if any bounded lane is invalid. */
int fa18_submit_static_glyph(FA18PlanarPage *page,
                             const FA18StaticGlyphSubmission *submission);

#endif
