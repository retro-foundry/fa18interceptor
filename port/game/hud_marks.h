#ifndef FA18_GAME_HUD_MARKS_H
#define FA18_GAME_HUD_MARKS_H

#include <stdint.h>

/* The head-up display's fixed marks, in colour 10 moved by the view origin
 * (SPAN_ORIGIN_Y across, REDRAW_STATE_WORD down). */

/* The centre mark (four dots and a dash), two short dashes below it and,
 * for record types $10-$12, the glass frame: five lines clipped to the
 * screen's columns ($C34146). */
void draw_hud_marks(void);

/* A row of dots 10 pixels apart either side of HUD_CENTRE_X at row `y`
 * (the second and fourth doubled upwards), inside columns $85..$B9, after a
 * double dot at the centre ($C34066). */
void draw_tick_row(int16_t y);

/* The target box: four times, step the seeker mark toward TARGET_MARK
 * (unless POST_INPUT_EVENT), then draw one side of the box around the
 * target, clipped to the HUD frame; finally TARGET_MARK moves to
 * SELECTION_MARKER and is cleared ($C342D0). */
void draw_target_box(void);

/* The missile cue: WARNING_CAUSES bit 14 while the seeker mark is within
 * 5 pixels of the target (bit 9 once it has let go), SHOOT_CUE while the
 * target is also within the selected missile's reach, each change with its
 * sound in EVENT_BITS; then the seeker symbol (large when on the target)
 * and the cue text ($C33DC8). */
void update_missile_cue(void);

#endif
