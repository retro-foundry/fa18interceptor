#ifndef FA18_GAME_VIEW_H
#define FA18_GAME_VIEW_H

/* Zoom all the way in ($80) and request a display update. */
void set_zoom_maximum(void);

/* While a pan or rotate key is held, turn the view 6 degrees: panning is
 * limited to the front half circle, rotation wraps. */
void pan_view_from_keys(void);

#endif
