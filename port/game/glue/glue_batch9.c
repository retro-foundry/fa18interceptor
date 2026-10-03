/* Glue for decay/nudge helpers, five_eighths, random_bit,
 * free_voice, update_readout, reset_mission_objects and pan_view_from_keys.
 * Several are compiled C that assign to their parameters, which rewrites the
 * caller's argument slots; the glue reproduces those writes. */
#include "glue.h"
#include "ports_glue.h"

#include "audio.h"
#include "control_records.h"
#include "fixed_math.h"
#include "globals.h"
#include "memory.h"
#include "readouts.h"
#include "text.h"
#include "view.h"

/* $C13A2A: decay_toward_zero(short *value, int shift); A0 = value, and
 * D1.w = the new value when |value| > 15. */
int glue_C13A2A(void) {
    gaddr value = rd_u32(A(7) + 4);
    int16_t before = rd_s16(value);

    decay_toward_zero(value, rd_s16(A(7) + 10));

    A(0) = value;
    if (before < -15 || before > 15) SET_W(D(1), rd_u16(value));
    return glue_return();
}

/* $C13CDE: nudge_outside_dead_zone(short *value); A0 = value, D0.w = the
 * new value, D1.w = the old one. */
int glue_C13CDE(void) {
    gaddr value = rd_u32(A(7) + 4);
    int16_t before = rd_s16(value);

    nudge_outside_dead_zone(value);

    A(0) = value;
    SET_W(D(0), rd_u16(value));
    SET_W(D(1), before);
    return glue_return();
}

/* $C13396: five_eighths(short x): the result replaces x in its slot. */
int glue_C13396(void) {
    gaddr slot = A(7) + 6;
    int16_t x = rd_s16(slot), r = five_eighths(x);
    wr_s16(slot, r);
    D(0) = (uint32_t)(int32_t)r;
    SET_W(D(1), x >> 3);
    return glue_return();
}

/* $C50AB4: random_bit(); D0 = the bit. */
int glue_C50AB4(void) {
    D(0) = (uint32_t)random_bit();
    return glue_return();
}

/* $C17B08: free_voice(int channel); leaves clear_voice_interrupt's D0/A0. */
int glue_C17B08(void) {
    uint32_t channel = rd_u32(A(7) + 4);
    free_voice((int16_t)channel);
    D(0) = channel << 2;
    A(0) = rd_u32(VOICE_TABLE + (gaddr)(int32_t)(int16_t)D(0));
    return glue_return();
}


int glue_C0840E(void) {
    reset_mission_objects();
    return glue_return();
}

/* $C258C8: leaves the rotate angle in D1.w once the guards pass. */
int glue_C258C8(void) {
    int active = rd_u8(CONTEXT_SELECT) && !rd_u8(PAUSE_A) && !rd_u8(CONTEXT_STARTED) && !rd_u8(CONTEXT_STATE);
    pan_view_from_keys();
    if (active) SET_W(D(1), rd_u16(VIEW_ROTATE));
    return glue_return();
}
