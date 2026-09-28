#ifndef FA18_GAME_COCKPIT_H
#define FA18_GAME_COCKPIT_H

/* Ask every cockpit display to redraw. */
void request_cockpit_redraw(void);

/* Scene start: lines may reach row 144; every display redraws. */
void finish_scene_setup(void);

/* Advance the cockpit slide animation one step: the 3D view's last row and
 * the display offset move by the step's row offset; at the end (or when
 * SPAN_ORIGIN leaves +-14) the view returns to 144 rows. */
void step_cockpit_slide(void);

#endif
