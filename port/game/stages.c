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
