/* Source stage tables, counters and display-list ordering. */
#include "stages.h"
#include "globals.h"
#include "fixed_math.h"
#include "fault.h"

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

int16_t find_sorted_word(gaddr table, int16_t key) {
    gaddr entries = table + 2;
    int16_t low = 0, high = (int16_t)(rd_s16(table) >> 1);
    for (;;) {
        int16_t mid, entry;
        if ((int32_t)high - low < 0) {
            wr_u16(ERROR_CODE, 0x1C);
            return -1;
        }
        mid = (int16_t)((int16_t)((int16_t)(high - low) >> 1) + low);
        entry = rd_s16(entries + (gaddr)(int32_t)(int16_t)(mid * 2));
        if (key == entry) return mid;
        if (key < entry) high = (int16_t)(mid - 1);
        else low = (int16_t)(mid + 1);
    }
}

gaddr skip_if_shown_record_flag(gaddr stream) {
    int16_t skip = rd_s16(stream);
    gaddr record = CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(SCRIPT_RECORD);
    stream += 2;
    if (rd_u16(record + 2) & 0x08) stream += (gaddr)(int32_t)skip;
    return stream;
}

gaddr skip_word_for_mode_57(gaddr stream) {
    return rd_u16(STREAM_MODE) == 0x57 ? stream + 2 : stream;
}

void load_long_table(gaddr src) {
    gaddr p = rd_u32(LONG_TABLE);
    int i;
    for (i = 0; i < 16; i++, src += 4, p += 4) wr_u32(p, rd_u32(src));
    wr_u8(TABLE_CLEAR_MODE, 2);
}

#define ENTRY_BYTES 24
#define MOST_SORTED 22

/* One entry's key. */
static int16_t entry_key(gaddr entry, DisplaySortResult *result) {
    uint16_t flags = rd_u16(entry);
    int16_t shift = (int16_t)(flags & 15), x, y, z, depth;

    wr_u16(BOUND_SHIFT, (uint16_t)shift);
    if (flags & 0x40) return 0x7FFF;
    depth = rd_s16(entry + 0x10);
    if (depth) return (int16_t)((uint16_t)depth << shift);
    x = rd_s16(entry + 6);
    y = rd_s16(entry + 8);
    z = rd_s16(entry + 10);
    wr_u8(POSITION_VALID, 0);
    if (flags & 0x10) {
        gaddr record = CONTROL_RECORDS + (gaddr)(int32_t)(int16_t)((flags & 0xFF00) * 2);
        int s = flags & 15;
        x = (int16_t)(x + ((rd_s16(record + 0xC) & 0xFFF) >> s));
        z = (int16_t)(z + ((rd_s16(record + 0xE) & 0xFFF) >> s));
        y = (int16_t)(rd_s32(record + 0x10) >> s);
        wr_u32(POSITION_LEVEL, (uint32_t)((int32_t)(rd_u32(record + 0x10) + rd_u32(PROJECTION_Y)) >> s));
        wr_u8(POSITION_VALID, 1);
    }
    {
        DistanceResult distance = target_distance_result(x, y, z);
        result->planar_factor = distance.planar_factor;
        result->has_factor = 1;
        return (int16_t)((uint16_t)distance.length << (rd_s16(BOUND_SHIFT) & 63));
    }
}

static void sort_list(gaddr list, int16_t count, DisplaySortResult *result) {
    gaddr copy = WORKSPACES;
    int16_t n = count > MOST_SORTED ? MOST_SORTED : count, i;

    for (i = 0; i < n; i++) wr_u16(DEPTH_KEYS_SOURCE + (gaddr)(2 * i), (uint16_t)entry_key(list + (gaddr)(ENTRY_BYTES * i), result));
    /* C1E4A6 saves the current planar value before sorting the keys. */
    result->retained_word = (uint16_t)(result->planar_factor >> 16);
    result->has_output = result->has_factor;
    sort_by_depth(n);
    for (i = 0; i < n * ENTRY_BYTES; i++) wr_u8(copy + (gaddr)i, rd_u8(list + (gaddr)i));
    for (i = 0; i < n; i++) {
        gaddr from = copy + (gaddr)(int32_t)(int16_t)(rd_s16(DEPTH_ORDER + (gaddr)(2 * i)) * ENTRY_BYTES);
        int k;
        for (k = 0; k < ENTRY_BYTES; k++) wr_u8(list + (gaddr)(ENTRY_BYTES * i + k), rd_u8(from + (gaddr)k));
        /* C1E472's six-long copy leaves the entry's fourth long for the
         * following list's cached-depth paths. */
        result->planar_factor = rd_u32(from + 12);
        result->has_factor = 1;
    }
}

void sort_display_list_retained(int all, DisplaySortResult *result) {
    if (!rd_u8(SORT_LISTS_ON)) return;
    for (;;) {
        int8_t index = (int8_t)(rd_u8(SORT_LIST_NEXT) - 1);
        if (index >= 0) {
            gaddr slot = SORT_LISTS + (gaddr)(6 * index);
            int16_t count;
            if (rd_s16(slot) < 0) return;
            count = rd_s16(slot + 4);
            if (count <= 0) fatal_error(0x37);
            sort_list(rd_u32(slot), count, result);
        }
        wr_u8(SORT_LIST_NEXT, (uint8_t)(rd_u8(SORT_LIST_NEXT) - 1));
        if ((int8_t)rd_u8(SORT_LIST_NEXT) < 0) {
            wr_u8(SORT_LIST_NEXT, rd_u8(SORT_LIST_COUNT));
            return;
        }
        if (!all) return;
    }
}

void sort_display_list(int all) {
    DisplaySortResult result = {0};
    sort_display_list_retained(all, &result);
}


void clear_scene_startup_state(void) {
    gaddr p;
    /* $C090C2: CLR.B across 53 bytes, then CLR.W across 52 words. */
    for (p = 0xC45790u; p < 0xC457C5u; ++p) wr_u8(p, 0);
    for (p = 0xC458C0u; p < 0xC45928u; p += 2) wr_u16(p, 0);
}

void enable_scene_record_updates(void) {
    gaddr p;
    /* $C090F2: twelve consecutive byte stores. */
    for (p = RECORD_UPDATES_ON; p < 0xC45790u; ++p) wr_u8(p, 1);
}
