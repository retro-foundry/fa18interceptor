#ifndef FA18_GAME_VIEW_H
#define FA18_GAME_VIEW_H

#include <stdint.h>

/* Zoom all the way in ($80) and request a display update. */
void set_zoom_maximum(void);

/* While the stick is held, turn the view 6 degrees: panning is
 * limited to the front half circle, rotation wraps. */
void pan_view_from_keys(void);

/* The 45-degree sector (0-7) of the view angle into VIEW_OCTANT. */
void update_view_octant(void);

/* Aim the view and build its matrices ($C2D9BA). Outside a context view
 * the stick pans it; in one, VIEW_PAN and VIEW_ROTATE turn toward a point
 * just ahead of the followed record (behind it for kind $30 records): at
 * once for a new target or without CONTEXT_SMOOTH, else by up to $230 or
 * $7D0 a step. Then VIEW_ANGLE_MATRIX (scaled by MATRIX_ROW_SCALES),
 * LIST_MATRIX from the rotation, and ATTITUDE_A-C from the player's
 * orientation words. */
void aim_view(void);

/* The observer at (x, y, z): its position, and the negated position with x
 * and z kept to 22 bits ($C0915A). */
void set_observer_position(int32_t x, int32_t y, int32_t z);

/* The fixed start position ($C0910C). */
void start_position(int32_t out[3]);

#endif
