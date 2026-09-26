#ifndef FA18_GLYPH_H
#define FA18_GLYPH_H

#include "display.h"

#include <stddef.h>
#include <stdint.h>

/* `$C32740`: expand the low `digit_count` nibbles of a packed value into
 * scratch bytes in display order. `output` must have room for `digit_count`
 * bytes. Leading-zero suppression is a separate draw-loop decision. */
int fa18_format_packed_decimal(uint32_t packed_value, uint8_t digit_count,
                               uint8_t *output);

/* `$C25A08`: convert an unsigned workspace value to packed decimal nibbles. */
int fa18_pack_decimal_workspace(uint32_t value, uint32_t *packed_value);

typedef struct {
    uint32_t raw_value;
    uint32_t packed_value;
    uint8_t digit_count;
    uint8_t digits[8];
} FA18CockpitNumericValue;

typedef struct {
    uint16_t compositor_mask;
    FA18CockpitNumericValue value;
} FA18NumericDrawRequest;

typedef struct {
    int16_t previous_word;
    uint8_t repeat_count;
    uint8_t disable_conversion;
} FA18ScaledNumericState;

/* Compose `$C25A08` and `$C32740` into one native numeric render value. */
int fa18_prepare_cockpit_numeric(uint32_t raw_value, uint8_t digit_count,
                                 FA18CockpitNumericValue *value);

/* `$C321D2`: scale a selected record long before the four-digit render. */
int fa18_prepare_scaled_record_numeric(int32_t record_value,
                                       FA18CockpitNumericValue *value);

/* `$C321D2`: update redraw state and produce the two observed submissions. */
int fa18_update_scaled_numeric(FA18ScaledNumericState *state,
                               int32_t record_value,
                               FA18NumericDrawRequest requests[2]);

/* The draw loop's leading-zero check, retaining one digit for zero. */
const uint8_t *fa18_skip_leading_zero_digits(const uint8_t *digits,
                                             uint8_t digit_count);

/* `$C32858-$C3287D`: merge one glyph byte per row into a strided longword
 * destination. `destination_offset` is relative to the selected native
 * plane, and each following row is 40 bytes later. */
int fa18_merge_glyph_stream(FA18PlanarPage *page, uint8_t plane_index,
                            size_t destination_offset,
                            const uint8_t *glyph_bytes, uint16_t row_count,
                            uint8_t shift_count);

/* `$C327A0`: a native font owns its offset table and byte stream. Offsets are
 * relative to `glyph_bytes`, matching the original word table semantics. */
typedef struct {
    const uint16_t *offsets;
    size_t offset_count;
    const uint8_t *glyph_bytes;
    size_t glyph_byte_count;
} FA18GlyphTable;

/* Select the stream for an ASCII character after the observed space bias. */
int fa18_select_glyph(const FA18GlyphTable *table, uint8_t character,
                      const uint8_t **glyph_stream,
                      size_t *remaining_bytes);

/* Address-free form of `$C327A0-$C327C8`. The original routine uses these
 * values as register arithmetic; keeping them named here makes the caller's
 * unresolved producer inputs visible without reproducing Amiga addresses. */
typedef struct {
    int32_t visible_coordinate;
    int32_t destination_offset;
    uint16_t compositor_shift;
} FA18GlyphPlacement;

int fa18_prepare_glyph_placement(int16_t lane_base,
                                 int16_t doubled_render_lane,
                                 int16_t glyph_position,
                                 int32_t geometry_base,
                                 int32_t selected_pointer_value,
                                 uint16_t compositor_shift,
                                 FA18GlyphPlacement *placement);

/* Select one native glyph and apply its bounded rows to a planar page. */
int fa18_render_glyph(FA18PlanarPage *page, const FA18GlyphTable *table,
                      uint8_t character, uint8_t plane_index,
                      size_t destination_offset, uint8_t shift_count,
                      uint16_t row_count);

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
