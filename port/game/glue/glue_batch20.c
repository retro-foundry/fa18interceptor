/* Glue for the inverse orientation $C2D970, the stream operation $C21940,
 * the attitude flags $C122A2, the vertex tail $C0D384, the runtime divide
 * $C52EC8 and the interrupt server $C06132. */
#include "glue.h"
#include "ports_glue.h"

#include "attitude.h"
#include "audio.h"
#include "fixed_math.h"
#include "globals.h"
#include "interrupts.h"
#include "matrix.h"
#include "memory.h"
#include "stages.h"
#include "vertex_tail.h"

void alternate_rotation_registers(void); /* glue_batch19.c */

/* $C2D970: A1 record, D5-D7.w angles. Continues into $C2E514 with the
 * negated angles in D0/D2/D4. */
int glue_C2D970(void) {
    uint16_t x = (uint16_t)D(5), y = (uint16_t)D(6), z = (uint16_t)D(7);
    A(1) += 0x92;
    D(0) = x ? (uint16_t)(0x7080 - x) : 0;
    D(2) = y ? (uint16_t)(0x7080 - y) : 0;
    D(4) = z ? (uint16_t)(0x7080 - z) : 0;
    SET_W(D(1), 0x7080);
    alternate_rotation_registers();
    return glue_return();
}

/* $C21940: A2 stream. Leaves D0 = 0 (flags as MOVEQ #0), D1.w = the record
 * flag bit, D2 = the skip (or 0), A3 = the shown record. */
int glue_C21940(void) {
    gaddr record = CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(SCRIPT_RECORD);
    uint16_t skip = rd_u16(A(2));
    uint16_t bit = (uint16_t)(rd_u16(record + 2) & 0x08);
    SET_W(D(2), skip);
    if (!bit) D(2) = 0;
    SET_W(D(1), bit);
    A(2) = skip_if_shown_record_flag(A(2));
    A(3) = record;
    D(0) = 0;
    flags_logic_l(0);
    return glue_return();
}

/* $C122A2: D0.w ends as angle B (or STATUS_CA when it was set); in the
 * attitude path D0 and D1 hold the two long angles >> 3 first. */
int glue_C122A2(void) {
    int16_t a, b;
    int context = rd_u8(CONTEXT_SELECT) != 0;
    attitude_angles(&a, &b);
    if (!context) {
        D(0) = (uint32_t)(rd_s32(ATTITUDE_A) >> 3);
        D(1) = (uint32_t)(rd_s32(ATTITUDE_B) >> 3);
    }
    update_attitude_flags();
    if (rd_u16(STATUS_CA) & 0x0002) SET_W(D(0), rd_u16(STATUS_CA));
    else SET_W(D(0), b);
    return glue_return();
}

/* $C0D384: A3 workspace. The caller reads D6, D7 and A4: the last sum,
 * loaded by MOVEM.W from $4E (sign-extended) and advanced by word adds. */
void vertex_tail_registers(void);
void vertex_tail_registers(void) {
    gaddr w = A(3);
    int16_t x = rd_s16(w + 0x4E), y = rd_s16(w + 0x50), z = rd_s16(w + 0x52);
    int16_t nz;
    derive_vertex_tail(w);
    D(6) = ((uint32_t)(int32_t)x & 0xFFFF0000u) | rd_u16(w + 0xD2);
    D(7) = ((uint32_t)(int32_t)y & 0xFFFF0000u) | rd_u16(w + 0xD4);
    nz = (int16_t)(rd_u16(w + 0xD6) - (uint16_t)z);
    A(4) = (uint32_t)((int32_t)z + nz);
}

int glue_C0D384(void) {
    vertex_tail_registers();
    return glue_return();
}

/* $C52EC8: D0 / D1 -> D0 quotient, D1 remainder. */
int glue_C52EC8(void) {
    int32_t remainder, quotient = long_divide((int32_t)D(0), (int32_t)D(1), &remainder);
    D(0) = (uint32_t)quotient;
    D(1) = (uint32_t)remainder;
    return glue_return();
}

/* $C06132: A6 server data. D0 = A6 with MOVE.L's flags; X from the count's
 * ADDI. */
int glue_C06132(void) {
    uint16_t before = rd_u16(A(6) + SERVER_COUNT);
    D(0) = count_interrupt(A(6));
    flags_logic_l(D(0));
    FLAG_X = before == 0xFFFF ? XFLAG_SET : XFLAG_CLEAR;
    return glue_return();
}

/* $C17B2C: play_sound(sound, channel, volume), compiled C with long
 * arguments at 4/8/12(A7). Every register is live after it. Without a voice
 * record: D0 = sound << 2, A0 its SOUND_VOICES entry. Otherwise
 * clear_voice_interrupt's D0 = channel << 2 and A0 = that channel's voice
 * record, D1 = channel << 2, A1 the SOUND_VOICES entry. */
void play_sound_registers(uint32_t sound, uint32_t channel);
void play_sound_registers(uint32_t sound, uint32_t channel) {
    gaddr entry = SOUND_VOICES + (sound << 2);
    int plays = rd_u32(entry) != 0;
    if (!plays) {
        D(0) = sound << 2;
        A(0) = entry;
        flags_logic_l(0); /* TST.L of the empty entry */
    } else {
        D(0) = channel << 2;
        A(0) = rd_u32(VOICE_TABLE + (gaddr)(int32_t)(int16_t)D(0));
        D(1) = channel << 2;
        A(1) = entry;
        flags_logic_w(rd_u16(A(0) + 0x14)); /* the interrupt bit written to INTREQ */
    }
}

int glue_C17B2C(void) {
    uint32_t sound = rd_u32(A(7) + 4), channel = rd_u32(A(7) + 8), volume = rd_u32(A(7) + 12);
    play_sound((int)sound, (int)channel, (int32_t)volume);
    play_sound_registers(sound, channel);
    return glue_return();
}
