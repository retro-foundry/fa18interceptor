#ifndef FA18_GAME_FLIGHT_RECORDER_H
#define FA18_GAME_FLIGHT_RECORDER_H

#include <stdint.h>

/* The game's own flight recorder: once per update, while RECORDER_ON is set
 * and COCKPIT_FLAGS bit 6 is up, it appends the player's stick byte and the
 * two words accumulated since the last pass to its buffers, or, playing
 * back, hands the next word pair's second word back. A full buffer ends
 * the recording (mode 4, POST_INPUT_EVENT) and rewinds the cursors
 * ($C25A6A). */
void record_flight_input(void);

/* Replay the recorded stick and word pair, update the player's input ramps,
 * and queue the recorder's trailing key ($C1B27E). */
void update_flight_input(uint32_t player, uint32_t incoming_d0);

#endif
