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
/* $C2FA7E's from D0-D6 (glue_render_polygon.c). */
void line_registers(void);

/* $C2F64E's and $C2F63A's the same way. */
void square_registers(void);
void square_in_view_registers(void);

#endif
