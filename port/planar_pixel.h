#ifndef FA18_PLANAR_PIXEL_H
#define FA18_PLANAR_PIXEL_H

#include <stddef.h>
#include <stdint.h>

#include "planar_lane_page.h"
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

/* Exact low-lane fields read by `$C2F688-$C2FA6F`: `$C45954`, `$C456E7`,
 * `$C456E8`, and the low byte at `$C456EB`. No address or page identity is
 * retained at this boundary. */
typedef struct {
    uint16_t draw_mode_word;
    uint8_t active_plane_mask_byte;
    int16_t output_xor_enable_word;
    uint8_t output_xor_plane_mask_byte;
} FA18PlanarPixelSourceState;

int fa18_decode_planar_pixel_state(const FA18PlanarPixelSourceState *source,
                                   FA18PlanarPixelState *state);

enum {
    FA18_PLANAR_PIXEL_LANES = FA18_PLANAR_LANE_COUNT,
    FA18_PLANAR_PIXEL_ROW_BYTES = FA18_PLANAR_LANE_ROW_BYTES,
    FA18_PLANAR_PIXEL_PAGE_BYTES = FA18_PLANAR_LANE_PAGE_BYTES
};

/* Four caller-owned byte planes in ascending Copper colour-bit order. Source
 * `$C456B6` stores the same pointers in reverse: mask bit 0 uses its +$0C
 * pointer (BPL1), through mask bit 3 at +$00 (BPL4). The fifth Copper plane
 * is intentionally absent: `$C2F688-$C2FA6F` writes only these four lanes,
 * while the presentation owner supplies the remaining display plane. */
typedef FA18PlanarLanePage FA18PlanarPixelPage;

/* Caller-owned state at the `$C27C62 -> $C2F5F4/$C2F60A` boundary. */
typedef struct {
    FA18Video *video;
    const FA18PlanarPixelState *state;
} FA18PlanarPixelRendererContext;

typedef struct {
    FA18PlanarPixelPage *page;
    const FA18PlanarPixelState *state;
} FA18PlanarPixelPageRendererContext;

/* `$C2F688-$C2FA6F`: apply one bounded four-lane word-mask operation.
 * Returns zero after a write, one for the original nonpositive-y no-op, or
 * minus one outside the native page model. */
int fa18_apply_planar_pixel_mask(FA18Video *video,
                                 const FA18PlanarPixelState *state,
                                 FA18PlanarPixelTable table, int x, int y);

/* Same `$C2F688-$C2FA6F` operation at its native four-plane ownership
 * boundary. The caller holds the fifth Copper plane and page lifecycle. */
int fa18_apply_planar_pixel_mask_to_page(FA18PlanarPixelPage *page,
                                         const FA18PlanarPixelState *state,
                                         FA18PlanarPixelTable table, int x, int y);

/* `$C2F5F4-$C2F609`: select the primary one-word mask/handler tables. */
int fa18_submit_primary_renderer_pixel(FA18Video *video,
                                       const FA18PlanarPixelState *state,
                                       int16_t x, int16_t y);
int fa18_submit_primary_renderer_pixel_to_page(FA18PlanarPixelPage *page,
                                               const FA18PlanarPixelState *state,
                                               int16_t x, int16_t y);

/* `$C2F626-$C2F639`: select the alternate mask table with the primary
 * one-row handlers. */
int fa18_submit_alternate_renderer_pixel(FA18Video *video,
                                         const FA18PlanarPixelState *state,
                                         int16_t x, int16_t y);
int fa18_submit_alternate_renderer_pixel_to_page(FA18PlanarPixelPage *page,
                                                 const FA18PlanarPixelState *state,
                                                 int16_t x, int16_t y);

/* `$C2F60A-$C2F621`: a low-nibble-zero X emits two adjacent primary pixels;
 * every other X selects the alternate one-row mask route. */
int fa18_submit_adjacent_renderer_pixels(FA18Video *video,
                                         const FA18PlanarPixelState *state,
                                         int16_t x, int16_t y);
int fa18_submit_adjacent_renderer_pixels_to_page(FA18PlanarPixelPage *page,
                                                 const FA18PlanarPixelState *state,
                                                 int16_t x, int16_t y);

/* `$C2F66E-$C2F687`: below the source row limit, use the alternate two-row
 * handlers; at or above it, take the adjacent-renderer route. */
int fa18_submit_bounded_renderer_pixels(FA18Video *video,
                                        const FA18PlanarPixelState *state,
                                        int16_t x, int16_t y,
                                        int16_t row_limit);
int fa18_submit_bounded_renderer_pixels_to_page(FA18PlanarPixelPage *page,
                                                const FA18PlanarPixelState *state,
                                                int16_t x, int16_t y,
                                                int16_t row_limit);

/* `$C2F5C0-$C2F5F3`: apply the wrapping signed pair adjustments, reject an
 * X outside [0,320), then enter the primary renderer route. */
int fa18_submit_adjusted_renderer_pixels(FA18Video *video,
                                         const FA18PlanarPixelState *state,
                                         int16_t x, int16_t y,
                                         int16_t x_adjustment,
                                         int16_t y_adjustment);
int fa18_submit_adjusted_renderer_pixels_to_page(FA18PlanarPixelPage *page,
                                                 const FA18PlanarPixelState *state,
                                                 int16_t x, int16_t y,
                                                 int16_t x_adjustment,
                                                 int16_t y_adjustment);

/* Callback adapters for the source-selected direct-pair renderer entries. */
int fa18_emit_primary_renderer_pixel(void *context, int16_t x, int16_t y);
int fa18_emit_adjacent_renderer_pixels(void *context, int16_t x, int16_t y);
int fa18_emit_primary_renderer_pixel_to_page(void *context, int16_t x, int16_t y);
int fa18_emit_adjacent_renderer_pixels_to_page(void *context, int16_t x, int16_t y);

#endif
