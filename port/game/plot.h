#ifndef FA18_GAME_PLOT_H
#define FA18_GAME_PLOT_H

/* Pixel plots into the draw page's four planes. The colour is
 * CURRENT_COLOUR (bit n into plane table entry 3 - n); LINE_PLANES masks
 * the planes written. While LINE_COLOUR is not negative, the planes in
 * POINT_XOR_PLANES are toggled instead and, if any are, nothing else is
 * drawn. Rows at or above 0 are not drawn. */

#include "memory.h"

/* One pixel ($C2F5F4). */
void plot_pixel(int16_t x, int16_t y);

/* Two pixels, x - 1 and x ($C2F60A); split across words when x is at a
 * word boundary. */
void plot_pixel_pair(int16_t x, int16_t y);

/* Two by two pixels ($C2F66E): a pair on rows y and y + 1, or only a pair
 * from LINE_LAST_ROW down. Not yet proven: the recordings never call it. */
void plot_pixel_block(int16_t x, int16_t y);

#endif
