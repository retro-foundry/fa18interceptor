/* Glue for the ground-point transform, the voice release, message posting,
 * the observer position, two more post-input stages, the long table, the
 * chosen-record alert, the start position and the typed-code check. */
#include "glue.h"
#include "ports_glue.h"

#include "audio.h"
#include "globals.h"
#include "memory.h"
#include "messages.h"
#include "player_input.h"
#include "post_input.h"
#include "stages.h"
#include "view.h"
#include "view_transform.h"

#define SEXT(v) ((uint32_t)(int32_t)(int16_t)(v))
#define W(n) ((int16_t)D(n))

void play_sound_registers(uint32_t sound, uint32_t channel); /* glue_batch20.c */

/* $C098C6: D0.w the count, D7.w the point offset; the caller's frame holds
 * the shift at -8(A6), the offsets at -$86..-$74(A6), and the count at
 * -$A(A6), which is counted down there. */
int glue_C098C6(void) {
    gaddr f = A(6), src = rd_u32(BOUND_RECORD) + 6 + SEXT(D(7));
    gaddr out = WORKSPACES + SEXT((uint16_t)((W(7) >> 1) + W(7)));
    int16_t count = W(0), shift = rd_s16(f - 8), offset[6], x = 0, z = 0;
    static const int16_t frame[6] = {-0x86, -0x84, -0x82, -0x78, -0x76, -0x74};
    int k, bits = shift & 63, left;

    for (k = 0; k < 6; k++) offset[k] = rd_s16(f + (gaddr)(int32_t)frame[k]);
    transform_ground_points(src, count, shift, offset, out);
    SET_W(D(0), (uint16_t)(offset[0] + rd_s16(BOUND_OFFSET_X)));
    SET_W(D(1), (uint16_t)(offset[2] + rd_s16(BOUND_OFFSET_Z)));
    D(3) = SEXT(offset[3]);
    D(6) = SEXT(offset[4]);
    left = count;
    do {
        x = rd_s16(src);
        z = rd_s16(src + 2);
        src += 4;
        out += 6;
    } while (--left > 0);
    wr_s16(f - 0xA, (int16_t)left);
    x = (int16_t)((bits >= 16 ? (x < 0 ? -1 : 0) : x >> bits) + W(0));
    z = (int16_t)((bits >= 16 ? (z < 0 ? -1 : 0) : z >> bits) + W(1));
    {
        gaddr m = VIEW_ANGLE_MATRIX;
        D(5) = (uint32_t)((int32_t)x * rd_s16(m + 6));
        D(7) = (uint32_t)(((int32_t)(D(5) + (uint32_t)((int32_t)z * rd_s16(m + 10)))) >> 8);
        SET_W(D(7), W(7) + W(6));
        D(2) = (uint32_t)((int32_t)x * rd_s16(m + 12));
        D(4) = (uint32_t)(((int32_t)(D(2) + (uint32_t)((int32_t)z * rd_s16(m + 16)))) >> 8);
        SET_W(D(4), W(4) + offset[5]);
        A(0) = m + 14;
        A(4) = m;
    }
    A(1) = src;
    A(3) = out;
    return glue_return();
}

int glue_C0F4A6(void) {
    free_all_voices();
    D(0) = 3 << 2; /* free_voice's registers for the last channel */
    A(0) = rd_u32(VOICE_TABLE + 12);
    return glue_return();
}

int glue_C25704(void) {
    uint16_t code = (uint16_t)D(0);
    post_message(code);
    SET_W(D(0), code & 0xFF00);
    return glue_return();
}

int glue_C0915A(void) {
    int32_t x = (int32_t)D(0), y = (int32_t)D(1), z = (int32_t)D(2);
    set_observer_position(x, y, z);
    D(0) = 0u - ((uint32_t)x & 0x3FFFFF);
    D(1) = 0u - (uint32_t)y;
    D(2) = 0u - ((uint32_t)z & 0x3FFFFF);
    return glue_return();
}

int glue_C11078(void) {
    SET_W(D(0), rd_u16(POST_INPUT_COUNTDOWN));
    if (rd_s16(POST_INPUT_COUNTDOWN) < 0) {
        D(0) = 1;
        A(0) = ROUTINE_AFTER_EVENT;
    }
    raise_event_after_countdown();
    return glue_return();
}

/* $C11ACC: the source long at 4(A7); A0/A1 at the last long copied. */
int glue_C11ACC(void) {
    gaddr src = rd_u32(A(7) + 4), dst = rd_u32(LONG_TABLE);
    load_long_table(src);
    A(0) = src + 60;
    A(1) = dst + 60;
    return glue_return();
}

/* $C1803C: the volume long at 4(A7). */
int glue_C1803C(void) {
    int32_t volume = rd_s32(A(7) + 4);
    int enabled = (rd_u8(SOUND_FLAGS) & 4) != 0;
    sound_chosen_record_alert(volume);
    if (enabled) {
        SET_W(D(0), rd_u16(CHOSEN_RECORD));
        SET_W(D(1), rd_u16(VIEW_RECORD));
        if ((uint16_t)D(0) == (uint16_t)D(1) && rd_u32(ALERT_VOICE)) {
            A(0) = rd_u32(ALERT_VOICE);
            D(0) = 5;
            play_sound_registers(5, 2);
        }
    }
    return glue_return();
}

int glue_C0910C(void) {
    int32_t p[3];
    start_position(p);
    D(0) = (uint32_t)p[0];
    D(1) = (uint32_t)p[1];
    D(2) = (uint32_t)p[2];
    return glue_return();
}

int glue_C25246(void) {
    gaddr expected = EXPECTED_CODE, typed = KEY_TRANSLATED;
    check_typed_code();
    SET_W(D(1), rd_u16(EXPECTED_LENGTH) + 2);
    for (;;) {
        SET_B(D(0), rd_u8(expected));
        expected += 1;
        if ((int8_t)D(0) <= 0) break;
        typed += 1;
        if ((uint8_t)D(0) != rd_u8(typed - 1)) break;
        SET_W(D(1), W(1) - 1);
        if (W(1) == -1) break;
    }
    A(0) = typed;
    A(1) = expected;
    return glue_return();
}
