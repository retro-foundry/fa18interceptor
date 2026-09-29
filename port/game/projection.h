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

/* A small shape at view-space (x, y, z) ($C2D16C): each of its parts
 * placed from byte offsets times `scale`, its points shifted down by
 * `shift`, turned by VIEW_ANGLE_MATRIX and projected; a part with a point
 * outside the view is skipped. Kind 0 draws lines (colour 13), 3 filled
 * circles of `radius`, the others triangles. Returns 1 when any was drawn. */
int draw_shape(int16_t x, int16_t y, int16_t z, uint16_t scale, int8_t kind, int16_t radius, int16_t shift);

#endif
