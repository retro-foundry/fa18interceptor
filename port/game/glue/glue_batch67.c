/* Glue for the cell template expansion $C1D3F4. It restores D2-D5 and
 * A0-A5 itself, so only D0, D1, D6 and D7 change: the walk below repeats
 * the expansion's register flow without writing anything, and D6 is also
 * the record index the filing pass starts from. */
#include "glue.h"
#include "ports_glue.h"

#include "control_records.h"
#include "globals.h"
#include "memory.h"
#include "glue_text.h"
#include "hud_bars.h"
#include "matrix.h"
#include "render_span.h"
#include "scene_setup.h"
#include "stages.h"

#define SEXT(v) ((uint32_t)(int32_t)(int16_t)(v))
#define TEMPLATE_PAIRS 0xC1D8B6u

/* MOVEM.W's sign extension left in the low word by SWAP. */
static int32_t swapped_word(int16_t v) {
    return (int32_t)(((uint32_t)(uint16_t)v << 16) | (uint16_t)(v < 0 ? 0xFFFF : 0));
}

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

/* $C3003A is not registered. The marks it ends with are replayed from the
 * pen position below, and their low words match, but every one of them
 * writes only register low words, so D0 and D4-D7 keep high words from
 * further back: past the line, past the blit, and through the chain inside
 * draw_mark_polygon ($C3019C), whose own glue can only replay its
 * registers by doing its drawing again. Registering this needs that chain
 * split into work and register replay, as the clipper's is. */
int glue_C3003A(void) {
    int16_t origin = rd_s16(SPAN_ORIGIN), x = 0xCE, y = 0xA5;
    int32_t across;
    int k;

    draw_panel_mark();

    SET_W(D(7), (uint16_t)(0x0C - origin));
    if ((int16_t)D(7) < 0) return glue_return();
    SET_W(D(7), (uint16_t)(6 - origin));
    if ((int16_t)D(7) < 0) return glue_return();
    D(1) = (uint32_t)(0x1990 + rd_s32(REDRAW_STATE_LONG));
    A(4) = 2;
    SET_W(D(6), 0x542);
    A(5) = 0x25;
    SET_W(D(7), 0x0C);
    bound_span_registers();
    if ((int16_t)D(5) < 0) return glue_return();

    /* The last mark: the pen where plot_pixel_in_view left it, stepped on. */
    across = (int32_t)x + rd_s16(SPAN_ORIGIN_Y);
    x = (int16_t)across;
    if (across >= 0 && (int16_t)across < 0x140) y = (int16_t)(y + rd_u16(REDRAW_STATE_WORD));
    {
        static const int8_t dx[8] = {0, -2, -1, -2, 0x11, 0, 0, -1};
        static const int8_t dy[8] = {0, 1, 1, 3, 1, 1, 1, 2};
        for (k = 1; k < 8; k++) {
            x = (int16_t)(x + dx[k]);
            y = (int16_t)(y + dy[k]);
        }
    }
    SET_W(D(0), (uint16_t)x);
    SET_W(D(1), (uint16_t)y);
    restored_plot_registers();
    A(5) = 0x25;
    return glue_return();
}

/* $C28800: its two callees' registers now come from their replay helpers,
 * so the glue never repeats their work. Everything the body computes is
 * overwritten by the orientation's own registers except D1's high word
 * (from the track), A4 (the view table entry) and the two early returns. */
int glue_C28800(void) {
    gaddr record = A(0), source = A(2);
    int16_t select = rd_s16(source + 4);
    uint32_t d1 = D(1), d2 = D(2);
    int32_t x, y = 0, z;

    aim_record_at_view(record, source);

    if (select < 0) {
        int16_t at = (int16_t)(select & 0x7F00), v[5];
        gaddr entry;
        int k;

        SET_W(d1, (uint16_t)at);
        if (!at) {
            D(1) = d1;
            return glue_return();
        }
        at = (int16_t)(at >> 7);
        SET_W(d1, (uint16_t)at);
        entry = VIEW_PARAMETER_TABLE
              + (gaddr)(int32_t)rd_s16(VIEW_PARAMETER_TABLE + (gaddr)(int32_t)at);
        for (k = 0; k < 5; k++) v[k] = rd_s16(entry + (gaddr)(2 * k));
        A(4) = entry + 10;
        x = (int32_t)((uint32_t)swapped_word(v[0]) << 6) + (int32_t)((uint32_t)(int32_t)v[2] << 8);
        z = (int32_t)((uint32_t)swapped_word(v[1]) << 6) + (int32_t)((uint32_t)(int32_t)v[3] << 8);
    } else {
        gaddr other = CONTROL_RECORDS + (gaddr)(int32_t)(int16_t)((select & 0xFF00) * 2);

        A(1) = other;
        SET_W(d2, (uint16_t)(select & 0xFF00));
        SET_W(d1, (uint16_t)(rd_u16(other) & 0x40));
        if (!(uint16_t)d1) {
            D(1) = d1;
            D(2) = d2;
            return glue_return();
        }
        x = rd_s32(other + 0x14);
        z = rd_s32(other + 0x1C);
    }
    x -= rd_s32(record + 0x14);
    z -= rd_s32(record + 0x1C);

    D(1) = d1;
    track_direction_registers(0, 0, 0, &x, &y, &z, -1, 1);
    D(0) = 0;
    D(3) = 0;
    A(0) = record;
    A(1) = record;
    record_orientation_registers(record, 0, 0xFFFF0000u | rd_u16(TRACKED_HEADING), 0);
    return glue_return();
}

/* $C0924A, $C09266 and $C092A0: three entry points into one sequence, all
 * ending in the root's orientation. The pose choice is replayed here from
 * the entry byte as it was before the C, because a rejected negative entry
 * clears it and leaves the record pointer moved. */
static int scene_setup_return(int8_t which) {
    gaddr record = CONTROL_RECORDS, entry;
    uint32_t d4, d5, d6;

    for (;;) {
        int16_t v[5], index, tail[3];
        int k;

        entry = SCENE_POSE_TABLE + (gaddr)(int32_t)(int16_t)((int16_t)which << 4);
        for (k = 0; k < 5; k++) v[k] = rd_s16(entry + (gaddr)(2 * k));
        if (v[0] >= 0) {
            uint32_t product;
            for (k = 0; k < 3; k++) tail[k] = rd_s16(entry + 10 + (gaddr)(2 * k));
            product = (uint32_t)((uint16_t)tail[2] * 10u);
            d4 = 0;
            d6 = 0;
            d5 = ((uint32_t)((int32_t)tail[0] << 4) & 0xFFFF0000u)
               | (uint16_t)((uint16_t)product << 3);
            A(0) = entry + 16;
            A(2) = GRID_ADJUST_WORDS;
            D(1) = rd_u32(record + 0x1C); /* the z it last held */
            D(3) = (uint32_t)((-(int32_t)rd_s32(TARGET_POINT)) >> 8);
            break;
        }
        index = (int16_t)(v[0] & 0x7FFF);
        record += (gaddr)(int32_t)(int16_t)((int16_t)(index << 8) * 2);
        if (!(rd_u8(record + 1) & 0x40)) {
            which = 0;
            continue;
        }
        d4 = SEXT(rd_u16(CONTROL_RECORDS + 0x66));
        d5 = SEXT(rd_u16(CONTROL_RECORDS + 0x68));
        d6 = SEXT(rd_u16(CONTROL_RECORDS + 0x6A));
        A(0) = entry + 10;
        A(2) = CONTROL_RECORDS;
        A(4) = rd_u32(SCENE_POINTERS + 0x10 + (gaddr)(int32_t)(int16_t)(index * 20));
        /* z and x swapped and masked down to their quadrant bytes. */
        D(1) = (uint32_t)(uint16_t)rd_u32(CONTROL_RECORDS + 0x1C) << 16;
        D(3) = (uint32_t)((rd_s32(CONTROL_RECORDS + 0x14) & 0x3FFFFF) >> 8);
        record = CONTROL_RECORDS; /* $C0952E puts it back before the matrix */
        break;
    }
    record_orientation_registers(record, d4, d5, d6);
    return glue_return();
}

int glue_C092A0(void) {
    int8_t which = (int8_t)rd_u8(SCENE_POSE_ENTRY);
    place_scene_root();
    return scene_setup_return(which);
}

int glue_C09266(void) {
    int8_t which = (int8_t)rd_u8(SCENE_POSE_ENTRY);
    reset_scene_recorder();
    return scene_setup_return(which);
}

int glue_C0924A(void) {
    int8_t which = (int8_t)rd_u8(SCENE_POSE_ENTRY);
    reset_scene_context();
    return scene_setup_return(which);
}
