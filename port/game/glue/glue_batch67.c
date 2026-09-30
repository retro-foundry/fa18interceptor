/* Glue for the cell template expansion $C1D3F4. It restores D2-D5 and
 * A0-A5 itself, so only D0, D1, D6 and D7 change: the walk below repeats
 * the expansion's register flow without writing anything, and D6 is also
 * the record index the filing pass starts from. */
#include "glue.h"
#include "ports_glue.h"

#include "control_records.h"
#include "globals.h"
#include "memory.h"
#include "stages.h"

#define TEMPLATE_PAIRS 0xC1D8B6u

/* The bounds $C1D4E4 leaves in D6 and D7 (it starts with MOVEQ #0,D6, so
 * D6 is its own search bound from there on, not the bitmap offset). */
static void sorted_word_bounds(gaddr table, int16_t key, int16_t *low_out, int16_t *high_out) {
    gaddr entries = table + 2;
    int16_t low = 0, high = (int16_t)(rd_s16(table) >> 1), mid;

    for (;;) {
        int16_t entry;
        if ((int32_t)high - low < 0) break;
        mid = (int16_t)((int16_t)((int16_t)(high - low) >> 1) + low);
        entry = rd_s16(entries + (gaddr)(int32_t)(int16_t)(mid * 2));
        if (key == entry) break;
        if (key < entry) high = (int16_t)(mid - 1);
        else low = (int16_t)(mid + 1);
    }
    *low_out = low;
    *high_out = high;
}

int glue_C1D3F4(void) {
    gaddr frame = A(6), templates = rd_u32(frame - 4), bitmap = rd_u32(frame - 8), cell, lists = A(1);
    int16_t row = (int16_t)D(0), column = (int16_t)D(1);
    uint32_t d0 = D(0), d1 = D(1), d6 = D(6), d7 = D(7);
    int enabled = rd_u8(CELL_CHECKS) != 0;
    FilingState state;

    SET_W(d0, (uint16_t)(2 * row));
    cell = templates + (gaddr)(int32_t)rd_s16(templates + (gaddr)(int32_t)(int16_t)d0);
    if (rd_s16(cell) >= 0) {
        int16_t word;
        SET_W(d0, (uint16_t)(16 * row));
        word = (int16_t)(4 * (int16_t)(column >> 5) + (int16_t)d0);
        SET_W(d6, (uint16_t)word);
        if ((rd_u32(bitmap + (gaddr)(int32_t)word) >> (column & 0x1F)) & 1) {
            gaddr entries = cell + (gaddr)(int32_t)rd_s16(cell) + 2;
            gaddr stream, cursor = A(2), list_end = 0;
            int16_t index = find_sorted_word(cell, column), low, high;
            int left = 0;

            sorted_word_bounds(cell, column, &low, &high);
            d6 = (uint16_t)low;
            SET_W(d7, (uint16_t)high);

            SET_W(d0, (uint16_t)(4 * index));
            stream = rd_u32(entries + (gaddr)(int32_t)(int16_t)d0);
            SET_B(d1, 0xFF);
            for (;;) {
                uint8_t header = rd_u8(stream++), level, flags;

                SET_B(d0, header);
                if (header == 0xFF) break;
                SET_B(d6, header);
                level = (uint8_t)(header & 0x0F);
                SET_B(d0, level);
                if (level != (uint8_t)d1) {
                    SET_B(d1, level);
                    SET_W(d7, (uint16_t)(32 * level));
                    SET_W(d0, (uint16_t)(96 * level));
                    left = 0x10;
                    cursor = lists + (gaddr)(96 * level);
                    list_end = cursor + 0x5D;
                }
                if (--left < 0) break; /* fatal error $38: it never returns */
                if (cursor >= list_end) break;
                flags = rd_u8(stream++);
                SET_B(d7, (uint8_t)(flags & 0x80));
                SET_B(d0, (uint8_t)(flags & 0x7F));
                cursor += 2;
                if (flags & 0x80) {
                    gaddr pair = TEMPLATE_PAIRS + (gaddr)(2 * ((header >> 4) & 0x0F));
                    SET_W(d6, (uint16_t)(2 * ((header >> 4) & 0x0F)));
                    SET_W(d0, (uint16_t)(int16_t)(int8_t)rd_u8(pair));
                    SET_B(d0, rd_u8(pair + 1));
                    cursor += 4;
                } else {
                    stream += 4;
                    cursor += 4;
                }
            }
        }
    }

    state.level = -1;
    state.level_offset = (int16_t)(d1 >> 16);
    state.index = (int)d6;
    expand_cell_templates(row, column, templates, bitmap, lists, A(2), &state);

    D(0) = d0;
    D(1) = d1;
    D(6) = d6;
    D(7) = d7;
    if (enabled) {
        D(1) = (uint32_t)(uint16_t)state.level_offset << 16 | (D(1) & 0xFF00u) | (uint8_t)state.level;
        D(6) = (uint32_t)state.index;
    }
    return glue_return();
}
