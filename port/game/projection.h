#ifndef FA18_GAME_PROJECTION_H
#define FA18_GAME_PROJECTION_H

#include <stdint.h>

/* Project a signed view-space point to the reflected screen pair
 * ($C2EC90); return whether it was accepted. */
int project_view_point(int16_t x, int16_t y, int16_t depth);

/* The adjacent entries project and choose a pixel, pair, block, or filled
 * circle from their mode and size ($C2EC94/$C2EC9C/$C2ECA4). */
int project_view_point_mode(int16_t x, int16_t y, int16_t depth,
                            int16_t mode, int16_t size, int16_t radius);

/* A stream word selects a view-space triplet; the next word supplies its
 * colour ($C1FE24/$C1FE46). */
void draw_display_stream_point(uint32_t stream, int16_t mode,
                               int16_t size, int16_t radius);

/* Transform the fixed tuple through the view matrix and submit its circle
 * ($C0DAEE). */
void draw_fixed_matrix_mark(void);

/* Shift one view-space triplet, size its circle by the triplet's magnitude,
 * and project it ($C0CFFA). */
void draw_scaled_view_circle(uint32_t point, int16_t shift, int16_t radius);
void draw_scaled_stream_circle(uint32_t stream, int16_t shift);

#endif
