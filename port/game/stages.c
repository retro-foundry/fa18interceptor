/* Small update stages. */
#include "stages.h"

#include "globals.h"

void empty_stage(void) {}

void reset_list(void) {
    wr_u16(LIST_COUNT, 0);
    wr_u32(LIST_WRITE, LIST_BUFFER);
}

void tick_timer(gaddr timer) {
    if (rd_s8(timer) >= 0) wr_u8(timer, (uint8_t)(rd_u8(timer) - 1));
}

int zero_result(void) { return 0; }

void clear_long_table(void) {
    gaddr p = rd_u32(LONG_TABLE);
    int i;
    wr_u8(TABLE_CLEAR_MODE, 2);
    for (i = 0; i < 16; i++, p += 4) wr_u32(p, 0);
}

void sort_by_depth(int16_t count) {
    gaddr out = DEPTH_ORDER;
    int i;

    for (i = 0; i < 0x2C; i += 4) wr_u32(DEPTH_KEYS + (gaddr)i, rd_u32(DEPTH_KEYS_SOURCE + (gaddr)i));
    for (;;) {
        int16_t left = (int16_t)(count - 1), best;
        gaddr key = DEPTH_KEYS, best_key;
        /* First unused key. */
        while ((best = rd_s16(key)) < 0) {
            key += 2;
            if (--left < 0) return;
        }
        best_key = key;
        key += 2;
        if (--left < 0) {
            /* It was the last key: output it, without advancing the list. */
            wr_u16(out, rd_u16(best_key + 0x2C));
            return;
        }
        for (; left >= 0; left--, key += 2) {
            if (best < rd_s16(key)) {
                best_key = key;
                best = rd_s16(key);
            }
        }
        wr_u16(best_key, 0xFFFF);
        wr_u16(out, rd_u16(best_key + 0x2C));
        out += 2;
    }
}

void reset_message_sequence(void) {
    wr_u16(MESSAGE_QUEUE, 0);
    wr_u16(MESSAGE_QUEUE + 2, 0);
    wr_u32(MESSAGE_TIMER, 0x1B8);
    wr_u8(MESSAGE_STATE_A, 0);
    wr_u8(MESSAGE_STATE_B, 0);
    wr_u8(MESSAGE_STATE_C, 0);
    wr_u8(MESSAGE_STATE_D, 0);
}

int16_t mode_offset(void) {
    int8_t mode = (int8_t)rd_u8(MODE_SELECT);
    if (mode == 0x7E || mode == 0x7F) return 0;
    return (int16_t)(rd_s8(rd_u32(MODE_TABLE) + 0x12 + (gaddr)(int32_t)mode) * 2);
}

gaddr skip_stream_records(gaddr stream) {
    int n = rd_u16(STREAM_SKIP) & 15;
    return stream + (gaddr)(n * 0x34);
}

int16_t display_value_to_draw(gaddr cache, int16_t value) {
    int16_t cached;
    if ((int8_t)rd_u8(REDRAW_FIRST + 1) > 0) {
        wr_u16(cache, (uint16_t)(value | 0x8000));
        return value;
    }
    cached = rd_s16(cache);
    if (cached >= 0) {
        if (cached == value || (rd_u8(DISPLAY_FORCE) & 1)) return -1;
        if (!rd_u8(POST_INPUT_EVENT)) {
            wr_u16(cache, (uint16_t)(value | 0x8000));
            return value;
        }
    }
    wr_u16(cache, (uint16_t)(rd_u16(cache) & 0x7FFF));
    return (int16_t)(cached & 0x7FFF);
}
