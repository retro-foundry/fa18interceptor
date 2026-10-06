#ifndef FA18_RECOMP_LOOP_INPUT_H
#define FA18_RECOMP_LOOP_INPUT_H

/* Native recordings keyed to the game's main loop, not to video frames.
 *
 * The game is CPU-paced: one pass of its main loop takes as many frames as
 * the work needs, so input keyed to frames lands at a different point in
 * the game whenever machine timing changes (a timing-model fix, or C
 * replacing translated code). Here an iteration is one entry to the main
 * update ($C0EFD4, called once per pass in the menu and in flight), and
 * input is only ever delivered at the start of an iteration: live input
 * while recording is held until then, and a replay delivers the same
 * events at the same iterations.
 *
 * Mouse motion goes straight into the mouse counters, at most 127 counts
 * per axis per iteration (the rest waits for the next), so the game sees
 * all of it in that iteration whatever the timing.
 *
 * FA18_LOOP_INPUT_V1 text: one event per line,
 *   <iteration> <frame> K <raw key> <down>
 *   <iteration> <frame> M <dx> <dy>
 *   <iteration> <frame> B <button 0 left, 1 right, 2 fire> <down>
 *   <iteration> <frame> J <up> <down> <left> <right>
 * then "end <iterations> <frames>". The frame is informational. */

#include <stdio.h>

#define FA18_LOOP_UPDATE_ENTRY 0xC0EFD4u

/* Start recording to `out` (header written now). */
void fa18_loop_record(FILE *out);
/* Load a recording to replay; 0 on a bad file. */
int fa18_loop_replay(const char *path);
/* Write the end line and close the recording, if any. */
int fa18_loop_finish(void);
/* Reference evidence: record raw keys at C1AD74, after OS delivery. The
 * FA18_GAME_INPUT_V1 rows use the same columns, with consumption iterations. */
void fa18_loop_game_record(FILE *out);
int fa18_loop_game_recording(void);
void fa18_loop_game_key(unsigned raw);

/* Live input while recording: held until the next iteration starts. */
void fa18_loop_host_key(int rawkey, int down);
void fa18_loop_host_mouse(int dx, int dy);
void fa18_loop_host_button(int button, int down);

/* Called at every entry to the main update. */
void fa18_loop_iteration(void);
/* Called at every frame end, for the informational frame numbers. */
void fa18_loop_frame(void);

int fa18_loop_recording(void);
long fa18_loop_iterations(void);
/* Where a replay ends: the recording's end iteration, else the iteration
 * after its last event (0 without a replay). */
long fa18_loop_replay_end(void);

#endif
