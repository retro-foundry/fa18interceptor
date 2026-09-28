/* Glue for unpack_display_value, find_sorted_word and the +$56/+$66
 * record update with its alert tone. */
#include "glue.h"
#include "ports_glue.h"

#include "audio.h"
#include "control_records.h"
#include "globals.h"
#include "memory.h"
#include "numbers.h"
#include "stages.h"

/* $C259C2: saves and restores everything it uses. */
int glue_C259C2(void) {
    unpack_display_value();
    return glue_return();
}

/* $C1D4E4: A0 table (count word first), D1.w key -> D0.w index. Leaves the
 * search bounds in D6 (MOVEQ'd, so its high word is 0) and D7.w, the last
 * probe * 2 in D4.w and A0 past the count. */
int glue_C1D4E4(void) {
    gaddr table = A(0), entries = table + 2;
    int16_t key = (int16_t)D(1), low = 0, high = (int16_t)(rd_s16(table) >> 1), mid = 0;
    int16_t found = find_sorted_word(table, key);

    for (;;) {
        int16_t entry;
        if ((int32_t)high - low < 0) {
            SET_W(D(0), high - low);
            break;
        }
        mid = (int16_t)((int16_t)((int16_t)(high - low) >> 1) + low);
        SET_W(D(4), mid * 2);
        entry = rd_s16(entries + (gaddr)(int32_t)(int16_t)(mid * 2));
        if (key == entry) {
            SET_W(D(0), mid);
            break;
        }
        if (key < entry) high = (int16_t)(mid - 1);
        else low = (int16_t)(mid + 1);
    }
    (void)found;
    D(6) = (uint16_t)low;
    SET_W(D(7), high);
    A(0) = entries;
    return glue_return();
}

/* $C13A8E: compiled C. D1 is live after it: the eased value on the easing
 * path; on the alert path whatever play_alert_tone's chain left. */
int glue_C13A8E(void) {
    gaddr r = rd_u32(CURRENT_RECORD);
    int16_t angle = rd_s16(r + 0x66);
    int alert = 0;

    if (angle > 0 && angle < 400 && !(rd_u16(r + 0x02) & 0x40) && (rd_u16(r) & 0x1600) == 0x1000 &&
        !(rd_u8(r + 0x04) & 0x02))
        alert = 1;

    update_record_56_from_66();

    if (angle >= 400) {
        SET_W(D(1), rd_u16(r + 0x56));
    } else if (alert && (rd_u8(SOUND_FLAGS) & 0x04)) {
        SET_W(D(1), rd_u16(VIEW_RECORD));
        if (rd_u16(SCRIPT_RECORD) == rd_u16(VIEW_RECORD) && rd_u32(SOUND_VOICES + 4 * SOUND_ALERT))
            D(1) = 2 * 4; /* play_sound: channel * 4 */
    }
    return glue_return();
}
