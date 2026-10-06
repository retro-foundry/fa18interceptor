/* Glue for the stick setters and small helpers.
 * Complete postflight callbacks now live in glue_postflight_completion.c. */
#include "glue.h"
#include "ports_glue.h"

#include "control_records.h"
#include "faces.h"
#include "flight_recorder.h"
#include "globals.h"
#include "memory.h"
#include "messages.h"
#include "player_input.h"
#include "post_input.h"
#include "stages.h"

#define SEXT(v) ((uint32_t)(int32_t)(int16_t)(v))

/* Stick: D1 the byte stored, D2 the field value. */

static int stick_glue(uint8_t value, int y) {
    if (y) set_stick_y(value);
    else set_stick_x(value);
    D(2) = value;
    if (rd_u8(PAUSE_A) || rd_u8(CONTEXT_STARTED)) SET_B(D(1), rd_u8(PLAYER_STICK));
    return glue_return();
}

int glue_C1B50C(void) { return stick_glue(0x20, 1); }
int glue_C1B510(void) { return stick_glue(0x10, 1); }
int glue_C1B514(void) { return stick_glue(0, 1); }
int glue_C1B558(void) { return stick_glue(0x08, 0); }
int glue_C1B55C(void) { return stick_glue(0x04, 0); }
int glue_C1B560(void) { return stick_glue(0, 0); }

/* ---- the rest ------------------------------------------------------------ */

/* $C33DA4: D5 = the tested bits. */
int glue_C33DA4(void) {
    D(5) = rd_u32(WARNING_CAUSES) & 0x4200;
    take_warning_events();
    return glue_return();
}

/* $C21C4C: A3 points; D0 = 0 (MOVEQ). */

/* $C1FED4: A2 stream; D0 = 0 with MOVEQ's flags. */
int glue_C1FED4(void) {
    A(2) = skip_word_for_mode_57(A(2));
    D(0) = 0;
    flags_logic_l(0);
    return glue_return();
}
