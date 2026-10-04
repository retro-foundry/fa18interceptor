#ifndef FA18_GLUE_TEXT_H
#define FA18_GLUE_TEXT_H

/* Register replays for the text plotters, shared by the glue of routines
 * that end in them. Each runs after the C has drawn. */

#include <stdint.h>

#include "memory.h"
#include "text.h"

/* The small-text loop ($C32794), glue_batch16.c. */
typedef struct {
    SmallText text;
    int last;       /* the character plotted last, or -1 */
    gaddr cell_at;  /* its glyph's last row */
    uint32_t cell;  /* its cell's bits there */
    uint32_t before;/* that long before the C drew */
} SmallTextProbe;
SmallText small_text_from_registers(void);
void small_text_probe(SmallTextProbe *p, const SmallText *line);
void small_text_registers(const SmallTextProbe *p);
/* The registers at the loop ($C32794) for `line`, its `digits` hex digits
 * ending at `end` formatted first ($C32740/$C3271A/$C32726). */
void small_line_entry(const SmallText *line, gaddr end, int digits, int keep_zeros, int in_view);
/* D3/D4 as the digit loop at $C32750 leaves them for `count` digits. */
void small_digits_registers(int count, int keep_zeros);

/* The 8-pixel line at the view's column ($C32AB4) and its decimal digits
 * first ($C32AA6), from their entry registers; glue_batch56.c. */
void text_in_view_registers(void);
void bcd_text_registers(void);

/* $C31C20's D0/D2 and flags for `value` against the cache, taken before
 * the C updates the cache; glue_batch10.c. */
void cached_value_registers(gaddr cache, int16_t value);

/* $C2F5C0's registers from D0/D1 as on entry, after the C plotted;
 * glue_batch33.c. */
void plot_in_view_registers(void);
/* $C310E2 from D7/D1/A4 (bound_span has no other effect); glue_batch5.c. */
void bound_span_registers(void);

/* $C2F5D4's (the plot with D0.w/D1.w put back) and $C2F5F4's. */
void restored_plot_registers(void);
void plot_registers(gaddr masks, gaddr writers);
void plot_registers_colour(gaddr masks, gaddr writers, uint16_t colour);
void pair_registers_colour(uint16_t colour);
/* $C2FA7E's from D0-D6 (glue_render_polygon.c). */
void line_registers(void);
void line_registers_to_row(int16_t last_row);
void plot_registers(gaddr masks, gaddr writers);

/* $C348B2's from D0/D1/D4 (glue_batch49.c) and $C31E6C's (glue_batch59.c). */
void shoot_cue_registers(void);
/* $C345A0's (glue_batch38.c), $C347F2's (glue_batch61.c) and $C31D16's
 * (glue_batch59.c), from their entry registers. */
void signed_readout_registers(void);
/* $C34066's from D1 (glue_batch61.c); the 8-pixel digits line's entry and
 * replay, as $C32AA4/$C32AA6 are reached (glue_batch59.c). */
void bcd_entry(gaddr layout, gaddr rows, int16_t x, int count, int digits, gaddr end, int keep_zeros);

/* $C123FA's D1 and A0 (glue_batch41.c); x, y, z come back adjusted as the
 * call left them, and nothing here writes game memory. */
void track_direction_registers(int32_t elevation, int32_t azimuth, int32_t before, int32_t *x_io,
                               int32_t *y_io, int32_t *z_io, int32_t max_step, int snap);

/* $C2D954's, from the record and the whole D4-D6 it was called with
 * (glue_batch23.c); safe to replay after the orientation is already set. */
void record_orientation_registers(gaddr record, uint32_t d4, uint32_t d5, uint32_t d6);

/* Pure register tails of $C092A0, $C1B906 and $C2FD22. */
void scene_setup_registers(int8_t which);
void view_mode_zero_registers(void);
void clear_render_buffers_registers(void);
/* $C301F0, $C304FA, $C304B2, $C3019C: polygon work already drawn. */
int prepare_polygon_to_row_registers(uint16_t last_size);
void clear_mask_registers(void);
/* $C091E0's transform products and result, from D3-D5 and the record. */
void world_registers(gaddr record, gaddr matrix);

/* $C2F64E's and $C2F63A's the same way. */
void square_registers(void);
void square_in_view_registers(void);

#endif
