#ifndef FA18_GAME_VIEW_H
#define FA18_GAME_VIEW_H

/* Zoom all the way in ($80) and request a display update. */
void set_zoom_maximum(void);

/* While the stick is held, turn the view 6 degrees: panning is
 * limited to the front half circle, rotation wraps. */
void pan_view_from_keys(void);

/* The 45-degree sector (0-7) of the view angle into VIEW_OCTANT. */
void update_view_octant(void);

#endif
