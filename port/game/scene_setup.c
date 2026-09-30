/* Setting the scene up around the player's record (see scene_setup.h). */
#include "scene_setup.h"

#include "control_records.h"
#include "fault.h"
#include "globals.h"
#include "matrix.h"

#define ROOT CONTROL_RECORDS

/* MOVEM.W sign-extends into the long, and SWAP then leaves that sign in
 * the low word. */
static int32_t swapped_word(int16_t v) {
    return (int32_t)(((uint32_t)(uint16_t)v << 16) | (uint16_t)(v < 0 ? 0xFFFF : 0));
}

/* One of the two bytes GRID_ADJUST_BYTES holds for a grid square. */
static int16_t grid_adjust(int16_t square, int second) {
    return (int16_t)(int8_t)rd_u8(GRID_ADJUST_BYTES + (gaddr)(int32_t)(int16_t)(2 * square) + (gaddr)second);
}

/* The scene pointers for the chosen input: the first is always the same
 * routine, the rest come from the table. */
static void take_scene_pointers(uint8_t input) {
    gaddr from = SCENE_POINTER_TABLE + (gaddr)(input == 0x10 ? 0x3C : 0x50);
    int k;

    wr_u32(SCENE_POINTERS, 0x00C1ED4Cu);
    for (k = 1; k < 5; k++) wr_u32(SCENE_POINTERS + (gaddr)(4 * k), rd_u32(from + (gaddr)(4 * k)));
}

/* The root's kind byte, from the scene's second pointer: its first word,
 * or one of the two after it, indexes a byte list. */
static void take_root_kind(void) {
    gaddr scene = rd_u32(SCENE_POINTERS + 4);
    int16_t word = rd_s16(scene), at;

    if (word >= 0) word = (word & 0x4000) ? rd_s16(scene + 2) : rd_s16(scene + 4);
    at = (int16_t)(word & 0xFFF);
    wr_u8(ROOT + 0x7D, (uint8_t)(rd_u8(scene + 6 + (gaddr)(int32_t)at) & 0x0F));
}

/* The flags the root starts a scene with. */
static void clear_root_flags(void) {
    wr_u16(ROOT, (uint16_t)(rd_u16(ROOT) & 0xF9FF));
    wr_u16(ROOT + 2, (uint16_t)(rd_u16(ROOT + 2) & 0xDEFF));
    wr_u16(ROOT + 2, (uint16_t)(rd_u16(ROOT + 2) & 0x7FF7));
    wr_u8(ROOT + 2, (uint8_t)(rd_u8(ROOT + 2) & (uint8_t)~0x10u));
    wr_u16(ROOT + 2, (uint16_t)(rd_u16(ROOT + 2) | 0x80));
    wr_u8(ROOT + 4, (uint8_t)(rd_u8(ROOT + 4) & 0x3F));
    wr_u8(ROOT + 4, (uint8_t)(rd_u8(ROOT + 4) & (uint8_t)~0x02u));
    wr_u8(ROOT + 4, (uint8_t)(rd_u8(ROOT + 4) & 0xF7));
    wr_u8(ROOT + 0x20, (uint8_t)(rd_u8(ROOT + 0x20) & (uint8_t)~0x01u));
    wr_u8(ROOT + 0x20, (uint8_t)(rd_u8(ROOT + 0x20) & (uint8_t)~0x04u));
    wr_u8(ROOT + 0x20, (uint8_t)(rd_u8(ROOT + 0x20) & (uint8_t)~0x02u));
    wr_u8(ROOT + 0x7C, 0);
}

/* The pose an entry whose first word is not negative gives: the first two
 * words are the grid square, the rest and the entry's three-word tail the
 * position within it. The orientation comes from the tail's last word.
 * `record` is the root, except after a rejected negative entry: the
 * original does not put the record pointer back, so a retry writes
 * through the record that entry had named. */
static void place_root_from_entry(gaddr record, gaddr entry, const int16_t v[5]) {
    int16_t tail[3], square = v[2], k;
    gaddr pair = GRID_ADJUST_WORDS + (gaddr)(int32_t)(int16_t)(square * 4);
    int32_t x, z, dx, dz;

    wr_s16(CONDITION_KEY_A, v[0]);
    wr_s16(CONDITION_KEY_B, v[1]);
    wr_u8(record + 0x0B, (uint8_t)square);
    for (k = 0; k < 3; k++) tail[k] = rd_s16(entry + 10 + (gaddr)(2 * k));

    wr_s16(record + 6, (int16_t)((int16_t)(v[0] << 2) + grid_adjust(square, 0)));
    wr_s16(GRID_ORIGIN_X, rd_s16(record + 6));
    wr_s16(GRID_ORIGIN_Z, (int16_t)((int16_t)(v[1] << 2) + grid_adjust(square, 1)));
    wr_s16(record + 8, rd_s16(GRID_ORIGIN_Z));

    x = (int32_t)((uint32_t)swapped_word(v[0]) << 8);
    z = (int32_t)((uint32_t)swapped_word(v[1]) << 8);
    x += (int32_t)((uint32_t)(int32_t)rd_s16(pair) << 10);
    z += (int32_t)((uint32_t)(int32_t)rd_s16(pair + 2) << 10);
    dx = (int32_t)((uint32_t)(int32_t)v[3] << 10) + (int32_t)((uint32_t)(int32_t)tail[0] << 4);
    dz = (int32_t)((uint32_t)(int32_t)v[4] << 10) + (int32_t)((uint32_t)(int32_t)tail[1] << 4);
    wr_u32(record + 0x18, 0x708);
    x += dx;
    z += dz;
    wr_u32(record + 0x14, (uint32_t)x);
    wr_u32(record + 0x1C, (uint32_t)z);
    wr_u32(TARGET_POINT, (uint32_t)-dx);
    wr_u32(TARGET_POINT + 8, (uint32_t)-dz);
    wr_u32(POSITION_BIAS, 0xFFFFF8F8u);
    wr_s16(record + 0x0C, (int16_t)(dx >> 8));
    wr_s16(record + 0x0E, (int16_t)(dz >> 8));
    wr_u32(record + 0x10, 0xFFFFFFF9u);
    set_record_orientation(record, 0, (uint16_t)((uint16_t)((uint16_t)tail[2] * 10) << 3), 0);
}

/* Two bits of a coordinate as a quadrant byte, the way the root keeps it. */
static uint8_t quadrant(int16_t across, int16_t along) {
    uint8_t a = (uint8_t)-(uint8_t)((uint8_t)(across & 3) - 3);
    uint8_t b = (uint8_t)-(uint8_t)((uint8_t)(along & 3) - 3);
    return (uint8_t)((uint8_t)(b * 4) + a);
}

/* The pose a negative entry gives: the record it names carries the root's
 * angles, and the root stands at a point through that record's inverse
 * orientation. */
static void place_root_from_record(gaddr selected, int16_t index) {
    gaddr descriptor = rd_u32(SCENE_POINTERS + 0x10 + (gaddr)(int32_t)(int16_t)(index * 20));
    int32_t height = rd_s32(descriptor + 2), out[3], x, z;
    int16_t across, along;

    if (height >= 0) {
        fatal_error(0x28);
        height = 0;
    }
    height &= 0x7FFFFFFF;
    wr_u32(ROOT + 0x10, (uint32_t)(height + 7));
    wr_u32(ROOT + 0x18, (uint32_t)((int32_t)((uint32_t)height << 8) + 0x708));
    wr_u8(ROOT + 4, (uint8_t)(rd_u8(ROOT + 4) | 0xC0));
    wr_u8(ROOT + 4, (uint8_t)(rd_u8(ROOT + 4) | 0x08));
    wr_u16(ROOT + 0xB8, 0);
    wr_u32(ROOT + 0x66, rd_u32(selected + 0x66));
    wr_u16(ROOT + 0x6A, rd_u16(selected + 0x6A));

    local_to_world(selected, selected + RECORD_INVERSE, 0x0B, 0, 0x68, out);
    wr_u32(ROOT + 0x14, (uint32_t)out[0]);
    wr_u32(ROOT + 0x1C, (uint32_t)out[2]);
    wr_s16(ROOT + 0x0C, (int16_t)((out[0] & 0x3FFFFF) >> 8));
    wr_s16(ROOT + 0x0E, (int16_t)((out[2] & 0x3FFFFF) >> 8));

    x = out[0] & 0x1FFFFFFF;
    z = out[2] & 0x1FFFFFFF;
    across = (int16_t)((int16_t)(x >> 16) >> 4);
    along = (int16_t)((int16_t)(z >> 16) >> 4);
    wr_u8(ROOT + 0x0A, quadrant(across, along));
    across = (int16_t)(across >> 2);
    along = (int16_t)(along >> 2);
    wr_s16(ROOT + 6, across);
    wr_s16(ROOT + 8, along);
    wr_u8(ROOT + 0x0B, quadrant(across, along));

    set_record_orientation(ROOT, rd_u16(ROOT + 0x66), rd_u16(ROOT + 0x68), rd_u16(ROOT + 0x6A));
}

void place_scene_root(void) {
    gaddr record;
    uint8_t input;

    wr_u8(UPDATE_MASK, 0xFF);
    prepare_player_record();
    reset_player_record();
    wr_u16(CONTEXT_RECORD, 0);
    wr_u8(ROOT + 0x2B, 9);
    wr_s16(CONTROL_ACCUMULATOR_Y, 9 << 3);
    wr_s16(CONTROL_ACCUMULATOR_COMPANION, 9 << 3);
    wr_u8(FIRE_STATE, 0xFE);
    input = rd_u8(POSTFLIGHT_FAILURE_INPUT);
    wr_u8(ROOT + 0x62, input);
    take_scene_pointers(input);
    take_root_kind();
    wr_u8(SCENE_ROOT_READY, 0);
    clear_root_flags();
    wr_u8(SCRIPT_COUNT, 0);
    wr_u8(FUNCTION_KEY_LEVEL, 0);
    wr_u8(BAR_E_FLAG, 0);
    wr_u8(BAR_REDRAWS_E, 0);

    record = ROOT;
    for (;;) {
        gaddr entry = SCENE_POSE_TABLE
                    + (gaddr)(int32_t)(int16_t)((int16_t)(int8_t)rd_u8(SCENE_POSE_ENTRY) << 4);
        int16_t v[5], index;
        int k;

        for (k = 0; k < 5; k++) v[k] = rd_s16(entry + (gaddr)(2 * k));
        if (v[0] >= 0) {
            place_root_from_entry(record, entry, v);
            return;
        }
        index = (int16_t)(v[0] & 0x7FFF);
        record += (gaddr)(int32_t)(int16_t)((int16_t)(index << 8) * 2);
        if (!(rd_u8(record + 1) & 0x40)) {
            wr_u8(SCENE_POSE_ENTRY, 0);
            continue;
        }
        place_root_from_record(record, index);
        return;
    }
}

void reset_scene_recorder(void) {
    wr_u32(RECORDER_CURSOR, rd_u32(RECORDER_START));
    wr_u32(RECORDER_WORD_CURSOR, rd_u32(RECORDER_WORDS));
    wr_u32(PLAYBACK_BYTES, rd_u32(RECORDER_START));
    wr_u32(PLAYBACK_WORDS, rd_u32(RECORDER_WORDS));
    wr_u32(PLAYBACK_WORDS, rd_u32(PLAYBACK_WORDS) + 4);
    wr_u16(RECORDER_COUNT, 0);
    wr_u16(PLAYBACK_COUNT, 0);
    place_scene_root();
}

void reset_scene_context(void) {
    wr_u8(CONTEXT_SELECT, 0);
    wr_u8(TRACK_STARTED, 0);
    wr_u8(CONTEXT_SMOOTH, 1);
    wr_u8(CONTEXT_STARTED, 1);
    reset_scene_recorder();
}
