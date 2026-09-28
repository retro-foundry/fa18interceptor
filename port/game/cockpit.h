#ifndef FA18_GAME_COCKPIT_H
#define FA18_GAME_COCKPIT_H

/* Ask every cockpit display to redraw. */
void request_cockpit_redraw(void);

/* Scene start: lines may reach row 144; every display redraws. */
void finish_scene_setup(void);

#endif
