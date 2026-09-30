#ifndef FA18_GAME_HUD_BARS_H
#define FA18_GAME_HUD_BARS_H

#include <stdint.h>

#include "memory.h"

/* Cockpit panel pieces drawn with the blitter, each clipped to the view
 * with bound_span: indicator bars filled or cleared in one plane, and
 * images copied through a mask into the draw page. */

/* Fill (BAR_SET) or clear (BAR_CLEAR) a bar in the draw page's plane at
 * `plane` (a plane-table offset): `words` wide at word column `position`
 * of the rows at `rows` (moved by REDRAW_STATE_LONG), BLTSIZE `size` for
 * the full width and a modulo of `modulo` plus the words cut off. The first
 * and last word masks trim the ends that are still shown. 0 when the bar
 * lies beyond the view ($C30CC4 after $C310E2). */
#define BAR_CLEAR 0x030A
#define BAR_SET   0x03FA
int fill_bar(uint16_t con0, int16_t plane, uint32_t rows, int16_t position, int16_t words, uint16_t size,
             int16_t modulo, uint16_t first_mask, uint16_t last_mask);

/* The same once bounded ($C30CC4): `cursor` the rows' offset, words cut
 * off on the right (shown_right, bound_span's result) and on the left. */
void fill_bar_words(uint16_t con0, int16_t plane, uint32_t cursor, int16_t shown_right, int16_t cut_left,
                    uint16_t size, int16_t modulo, uint16_t first_mask, uint16_t last_mask);

/* An image in all four planes ($C30EAA): A the shared `mask`, B each
 * plane's image (through the pointers at `images`), C = D the plane, with
 * the bounds as for fill_bar. Rows above the page's first are skipped. */
void blit_image(uint16_t con0, uint32_t mask, gaddr images, uint32_t rows, int16_t position, int16_t words,
                uint16_t size, int16_t modulo);

/* Three indicator bars and a marker line, each redrawn while its countdown
 * runs ($C30B5C). */
void draw_indicator_bars(void);

/* A bar, set while BAR_REDRAWS_F is positive; then, while BAR_REDRAWS_D
 * runs, the record's +$7C image and, while its +$02 bit 7 is set, a marker
 * line ($C30D34). */
void draw_mode_bar(void);

/* The compass: update it, then (when the tape position changes) blit the
 * tape image shifted to it into plane 3, and a marker line ($C30F78). */
void draw_compass_tape(void);

/* While REDRAW_FIRST counts down: the panel frame copied into all four
 * planes, and, while the view is panned (SPAN_ORIGIN), the uncovered side
 * filled with colour 12 down to LINE_LAST_ROW ($C30764). */
void draw_panel_frame(void);

/* The panel image at PANEL_IMAGE through the four plane masks ($C309B6). */
void draw_panel_image(void);

/* The panel area behind the mark redrawn from its image ($C3003A), then
 * the scaled mark polygon over it and the fixed marks of its scale: a
 * ten-pixel line in colour 2 and, in colour $D, the pixels stepping out
 * from (206, 165). Nothing when the area lies beyond the view. */
void draw_panel_mark(void);

#endif
