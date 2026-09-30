#ifndef FA18_GAME_SCENE_SETUP_H
#define FA18_GAME_SCENE_SETUP_H

/* Setting the scene up around the player's record, the root of
 * CONTROL_RECORDS. Three entry points into one sequence: each does its own
 * part and falls into the next ($C0924A, $C09266, $C092A0). */

#include "memory.h"

/* Clear the context and start it turning gradually, then reset the
 * recorder and place the root ($C0924A). */
void reset_scene_context(void);

/* Put the recorder and playback cursors back to the start of their
 * buffers (playback's word cursor one pair in) and clear their counts,
 * then place the root ($C09266). */
void reset_scene_recorder(void);

/* Give the root its scene: the scene pointers for the chosen input, the
 * record flags it starts with, and its pose from SCENE_POSE_TABLE
 * ($C092A0). A pose entry whose first word is not negative places the
 * root directly from the entry and the grid adjustment tables; a negative
 * one follows the control record its bits 0-14 name, taking its position
 * through that record's inverse orientation. A named record that is not
 * active (+$1 bit 6) clears SCENE_POSE_ENTRY and the choice is made
 * again. Either way the root is then given its orientation. */
void place_scene_root(void);

#endif
