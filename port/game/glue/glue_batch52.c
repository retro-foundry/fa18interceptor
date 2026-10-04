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
