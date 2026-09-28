/* Glue for the sound routines $C17CF6, $C17DAA, $C17E4A, $C17EF2, $C18096
 * and the record orientation $C2D954. The sound routines are compiled C
 * with long arguments at 4(A7) on; their callers read what the last inner
 * call left. */
#include "glue.h"
#include "ports_glue.h"

#include "audio.h"
#include "fixed_math.h"
#include "globals.h"
#include "matrix.h"
#include "memory.h"

void play_sound_registers(uint32_t sound, uint32_t channel); /* glue_batch20.c */
void alternate_rotation_registers(void);                    /* glue_batch19.c */

#define ARG(n) rd_s32(A(7) + 4 + 4 * (n))

/* free_voice's leftovers (clear_voice_interrupt's D0 and A0). */
static void free_voice_registers(uint32_t channel) {
    D(0) = channel << 2;
    A(0) = rd_u32(VOICE_TABLE + (gaddr)(int32_t)(int16_t)D(0));
}

int glue_C17CF6(void) {
    int enabled = (rd_u8(SOUND_FLAGS) & 0x02) && rd_u32(SOUND_VOICES + 4 * SOUND_ENGINE_HIGH);
    play_engine(ARG(0), ARG(1));
    if (enabled) play_sound_registers(SOUND_ENGINE_HIGH, 1);
    return glue_return();
}

/* $C17DAA: leaves D0 = ticks, D1 = the last division's remainder, A0 the
 * channel 1 voice, A1 the channel 0 voice. */
int glue_C17DAA(void) {
    int32_t period = ARG(0), volume = ARG(1), ticks = ARG(2), remainder;
    int enabled = (rd_u8(SOUND_FLAGS) & 0x02) && rd_u32(VOICE_SLOTS + 4);
    gaddr low = rd_u32(VOICE_SLOTS);
    int32_t volume_now = enabled ? rd_s32(low + VOICE_VOLUME) : 0;
    slide_engine(period, volume, ticks);
    if (enabled) {
        (void)long_divide((int32_t)((uint32_t)volume << 16) - volume_now, ticks, &remainder);
        D(0) = (uint32_t)ticks;
        D(1) = (uint32_t)remainder;
        A(0) = rd_u32(VOICE_SLOTS + 4);
        A(1) = low;
    }
    return glue_return();
}

int glue_C17E4A(void) {
    int noise = (rd_u8(SOUND_FLAGS) & 0x10) != 0;
    int exists = rd_u32(SOUND_VOICES + 4 * SOUND_NOISE_HIGH) != 0;
    play_noise(ARG(0));
    if (!noise) free_voice_registers(1);
    else if (exists) play_sound_registers(SOUND_NOISE_HIGH, 1);
    return glue_return();
}

/* $C17EF2: when sound 4 is missing D1 keeps nothing new; otherwise
 * play_sound's leftovers. */
int glue_C17EF2(void) {
    int32_t args[9];
    int i, exists = rd_u32(SOUND_VOICES + 4 * SOUND_PROGRAMMED) != 0;
    for (i = 0; i < 9; i++) args[i] = ARG(i);
    play_programmed_sound(args);
    if (exists) play_sound_registers(SOUND_PROGRAMMED, 3);
    return glue_return();
}

/* $C18096: D0.w/D1.w hold the two record words it compared. */
int glue_C18096(void) {
    int enabled = (rd_u8(SOUND_FLAGS) & 0x40) != 0;
    int same = rd_u16(SCRIPT_RECORD) == rd_u16(VIEW_RECORD);
    int exists = rd_u32(SOUND_VOICES + 4 * SOUND_SCRIPTED) != 0;
    if (enabled) {
        SET_W(D(0), rd_u16(SCRIPT_RECORD));
        SET_W(D(1), rd_u16(VIEW_RECORD));
    }
    play_scripted_sound(ARG(0));
    if (enabled && same && exists) play_sound_registers(SOUND_SCRIPTED, 2);
    return glue_return();
}

/* $C2D954: A1 record, D4-D6.w angles. Ends through $C2D970 with the angles
 * in D5-D7 and A1 + $92, so every register is as $C2E514 leaves it. */
int glue_C2D954(void) {
    uint16_t x = (uint16_t)D(4), y = (uint16_t)D(5), z = (uint16_t)D(6);
    gaddr record = A(1);
    uint32_t d4 = D(4), d5 = D(5), d6 = D(6);
    set_record_orientation(record, x, y, z);
    /* MOVEM.L restores D1/D5-D7/A1 from the D1/D4-D6/A1 it saved. */
    D(5) = d4;
    D(6) = d5;
    D(7) = d6;
    A(1) = record + 0x92;
    D(0) = x ? (uint16_t)(0x7080 - x) : 0;
    D(2) = y ? (uint16_t)(0x7080 - y) : 0;
    D(4) = z ? (uint16_t)(0x7080 - z) : 0;
    SET_W(D(1), 0x7080);
    alternate_rotation_registers();
    return glue_return();
}
