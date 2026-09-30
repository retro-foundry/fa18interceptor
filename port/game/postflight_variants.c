/* Source-order drawing heads of the postflight tuple and fixed-point variants. */
#include "postflight_variants.h"

#include "globals.h"
#include "memory.h"
#include "plot.h"
#include "render_line.h"

#define POSTFLIGHT_TUPLES 0xC3128Au
#define POSTFLIGHT_POINT_TABLE 0xC4E71Cu
#define POSTFLIGHT_PREFIX_GATE 0xC45838u
#define POSTFLIGHT_PREFIX_TICK 0xC45883u
#define POSTFLIGHT_PREFIX_BITS 0xC4586Du
#define POSTFLIGHT_SCAN_REQUEST 0xC457B9u

static int16_t add_word(int16_t a, int16_t b) {
    return (int16_t)((uint16_t)a + (uint16_t)b);
}

static int horizontal_visible(int16_t x) {
    return x >= 0 && x < 0x140;
}

void draw_postflight_tuple_pairs(void) {
    gaddr tuple = POSTFLIGHT_TUPLES;
    int pair;
    wr_u16(CURRENT_COLOUR, 3);
    for (pair = 0; pair < 2; ++pair, tuple += 8) {
        int16_t x0 = add_word(rd_s16(tuple), rd_s16(SPAN_ORIGIN_Y));
        int16_t x1 = add_word(rd_s16(tuple + 4), rd_s16(SPAN_ORIGIN_Y));
        int16_t y0, y1;
        if (!horizontal_visible(x0) || !horizontal_visible(x1)) continue;
        y0 = add_word(rd_s16(tuple + 2), rd_s16(REDRAW_STATE_WORD));
        y1 = add_word(rd_s16(tuple + 6), rd_s16(REDRAW_STATE_WORD));
        draw_line_to_row(x0, y0, x1, y1, 0xC7);
    }
}

void draw_postflight_fixed_quad(void) {
    int16_t x, y;
    x = add_word(0x9D, rd_s16(SPAN_ORIGIN_Y));
    if (!horizontal_visible(x)) return;
    y = add_word(0xA8, rd_s16(REDRAW_STATE_WORD));
    wr_u16(CURRENT_COLOUR, 0xC);
    plot_pixel(x, y);

    x = add_word(0x9F, rd_s16(SPAN_ORIGIN_Y));
    if (!horizontal_visible(x)) return;
    y = add_word(0xA8, rd_s16(REDRAW_STATE_WORD));
    plot_pixel(x, y);

    x = add_word(0x9E, rd_s16(SPAN_ORIGIN_Y));
    y = add_word(0xA8, rd_s16(REDRAW_STATE_WORD));
    plot_pixel(x, y);
    y = add_word(0xA7, rd_s16(REDRAW_STATE_WORD));
    plot_pixel(x, y);
}

int begin_postflight_variant_tail(PostflightVariantWork *work) {
    gaddr cursor;
    int i;
    if (!rd_u8(POSTFLIGHT_PREFIX_GATE)) return 0;

    work->table = POSTFLIGHT_POINT_TABLE + (rd_u16(DRAW_PAGE) ? 0x28u : 0u);
    cursor = work->table;
    wr_u16(CURRENT_COLOUR, 0);
    for (i = 0; i < 11; ++i) {
        int16_t x = rd_s16(cursor);
        int16_t y;
        cursor += 2;
        if (x == -1) break;
        y = rd_s16(cursor);
        cursor += 2;
        if (x < 0) plot_pixel_pair((int16_t)((uint16_t)x & 0x7FFFu), y);
        else plot_pixel(x, y);
    }
    wr_u8(POSTFLIGHT_PREFIX_TICK, (uint8_t)(rd_u8(POSTFLIGHT_PREFIX_TICK) + 1u));
    wr_u8(POSTFLIGHT_PREFIX_BITS, 0);
    work->vector_stream = LIST_BUFFER + 12;
    work->records_seen = 1;
    for (i = 0; i < 3; ++i)
        work->vector[i] = rd_s32(LIST_BUFFER + (gaddr)(4 * i));
    work->has_vector = (work->vector[0] | work->vector[1] | work->vector[2]) != 0;
    return 1;
}

static int32_t shifted_with_carry(int32_t value, int shift) {
    if (!shift) return value;
    int32_t shifted = value >> shift;
    return shifted + (((uint32_t)value >> (shift - 1)) & 1u);
}

int select_postflight_variant_record(PostflightVariantWork *work) {
    uint16_t offset;
    uint8_t flags;
    if (!work->has_vector) return 0;

    work->source_x = work->vector[0];
    work->source_z = work->vector[2];
    work->screen_x = shifted_with_carry(work->source_x, 13);
    work->screen_y = shifted_with_carry(work->source_z, 13);
    work->record_word = rd_u16(work->vector_stream);
    if (!(work->record_word & 0x10u)) return 0;

    offset = (uint16_t)((work->record_word & 0xFF00u) << 1);
    work->record = CONTROL_RECORDS + (gaddr)(int32_t)(int16_t)offset;
    flags = rd_u8(work->record);
    if (offset == rd_u16(VIEW_RECORD) || !(rd_u8(work->record + 1) & 0x40u) ||
        (rd_u8(work->record + 3) & 0x80u)) goto reject;

    work->screen_x = (int32_t)(0u - (uint32_t)work->screen_x);
    if (work->screen_x > 0x1B || work->screen_x < -0x1B) goto reject;
    work->screen_y = (int32_t)(0u - (uint32_t)work->screen_y);
    if (work->screen_y >= 0x16 || work->screen_y < -0x10) goto reject;
    return 1;

reject:
    wr_u8(work->record, (uint8_t)(flags & (uint8_t)~0x40u));
    return 0;
}

static int32_t abs_long(int32_t value) {
    return value < 0 ? (int32_t)(0u - (uint32_t)value) : value;
}

static void set_prefix_bit(unsigned bit) {
    wr_u8(POSTFLIGHT_PREFIX_BITS,
          (uint8_t)(rd_u8(POSTFLIGHT_PREFIX_BITS) | (uint8_t)(1u << bit)));
}

void classify_postflight_variant_record(PostflightVariantWork *work) {
    gaddr record = work->record;
    uint8_t mode = rd_u8(MODE_SELECT), category;

    if (mode != 0x7Du && mode != 2u) {
        if (abs_long(work->source_x) > 0x10000 ||
            abs_long(work->source_z) > 0x10000) {
            wr_u8(record + 0x20, (uint8_t)(rd_u8(record + 0x20) & ~0x40u));
            goto finish;
        }
        wr_u8(record + 0x20, (uint8_t)(rd_u8(record + 0x20) | 0x40u));
        category = rd_u8(record + 0x62) & 0xF0u;
        if (category != 0x20u) {
            if (rd_s16(SELECTED_RECORD) >= 0 && category == 0x10u &&
                rd_u16(VIEW_RECORD) == 0) {
                int8_t countdown = rd_s8(PLAYER_FLAGS_F);
                if (countdown == 0) wr_u8(PLAYER_FLAGS_F, 0x1E);
                else if (countdown > 0)
                    wr_u8(PLAYER_FLAGS_F, (uint8_t)(countdown == 1 ? 0xFF : countdown - 1));
            }
            if (!(rd_u8(record + 0x20) & 2u) &&
                !(rd_u16(record) & 0x0600u) &&
                !(rd_u8(record + 3) & 0x80u) && category != 0x30u) {
                if (category == 0) {
                    if (!(rd_u8(record + 1) & 8u) &&
                        (rd_u8(record + 0x38) & 0x7Fu) == rd_u16(TARGET_RECORD)) {
                        set_prefix_bit(rd_u8(record + 0x62) == 0 ? 4 : 5);
                    }
                } else if (rd_u8(record + 1) & 8u) {
                    set_prefix_bit(1);
                } else if (rd_u8(record + 0x62) == 0x15u) {
                    set_prefix_bit(3);
                } else {
                    set_prefix_bit(2);
                }
            }
        }
    }
finish:
    if (work->record_word & 0x10u)
        wr_u8(record, (uint8_t)(rd_u8(record) | 0x40u));
}

int submit_postflight_variant_record(PostflightVariantWork *work) {
    gaddr viewed = CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(VIEW_RECORD);
    uint16_t viewed_shift = rd_u8(viewed + 0x63) & 15u;
    int32_t x = (int32_t)(0u - (uint32_t)shifted_with_carry(work->source_x, viewed_shift));
    int32_t y = (int32_t)(0u - (uint32_t)shifted_with_carry(work->source_z, viewed_shift));
    uint16_t offset = (uint16_t)((work->record_word & 0xFF00u) << 1);
    gaddr selected = CONTROL_RECORDS + (gaddr)(int32_t)(int16_t)offset;
    uint8_t category;
    int pair;
    int16_t screen_x, screen_y;

    if (x > 0x1B || x < -0x1B || y >= 0x16 || y < -0x10) return 0;
    screen_x = add_word(add_word((int16_t)x, 0x9E), rd_s16(SPAN_ORIGIN_Y));
    if (!horizontal_visible(screen_x) || !(work->record_word & 0x10u)) return 1;
    screen_y = add_word((int16_t)y, 0xA7);
    if (offset == rd_u16(SELECTED_RECORD) &&
        !(rd_u8(POSTFLIGHT_PREFIX_TICK) & 1u)) return 1;

    pair = (int32_t)(0u - (uint32_t)rd_s32(PROJECTION_Y)) <= rd_s32(selected + 0x10);
    category = rd_u8(selected + 0x62) & 0xF0u;
    if (category == 0x20u || category == 0x30u) category = 5;
    else if (!(rd_u8(selected + 0x20) & 0x40u) ||
             (rd_u8(selected + 0x20) & 2u)) category = 8;
    else if (rd_u8(selected + 1) & 8u) category = 4;
    else category = category == 0 ? 2 : 1;
    wr_u16(CURRENT_COLOUR, category);

    if (work->table >= POSTFLIGHT_POINT_TABLE + (rd_u16(DRAW_PAGE) ? 0x50u : 0x28u))
        return 1;
    screen_y = add_word(screen_y, rd_s16(REDRAW_STATE_WORD));
    work->submit_x = screen_x;
    work->submit_y = screen_y;
    wr_u16(work->table, (uint16_t)screen_x | (pair ? 0x8000u : 0u));
    wr_u16(work->table + 2, (uint16_t)screen_y);
    work->table += 4;
    if (pair) plot_pixel_pair(screen_x, screen_y);
    else plot_pixel(screen_x, screen_y);
    return 1;
}

void advance_postflight_variant_record(PostflightVariantWork *work, int marked) {
    int i;
    work->record_word = (uint16_t)((work->record_word & ~0x20u) |
                                    (marked ? 0x20u : 0u));
    wr_u16(work->vector_stream, work->record_word);
    work->vector_stream += 16;
    work->records_seen = (uint16_t)(work->records_seen + 1u);
    for (i = 0; i < 3; ++i)
        work->vector[i] = rd_s32(work->vector_stream - 12 + (gaddr)(4 * i));
    work->has_vector = (work->vector[0] | work->vector[1] | work->vector[2]) != 0;
}

void process_postflight_variant_records(PostflightVariantWork *work) {
    while (work->has_vector) {
        int marked = 0;
        if (select_postflight_variant_record(work)) {
            classify_postflight_variant_record(work);
            marked = submit_postflight_variant_record(work);
        }
        advance_postflight_variant_record(work, marked);
    }
}

static void add_postflight_event(uint32_t bits) {
    wr_u32(EVENT_BITS, rd_u32(EVENT_BITS) | bits);
}

void resolve_postflight_variant_status(PostflightVariantWork *work) {
    uint8_t current = rd_u8(POSTFLIGHT_PREFIX_BITS);
    uint8_t previous = rd_u8(THREAT_EVENTS);
    wr_u16(work->table, 0xFFFFu);

    if (current & 0x10u) {
        if (!(previous & 0x10u)) add_postflight_event(1);
    } else if (current & 0x20u) {
        if (!(previous & 0x20u)) add_postflight_event(1);
    } else if (current & 8u) {
        if (!(previous & 8u)) {
            add_postflight_event(2);
            wr_u8(INFO_DELAY, 0x18);
        } else if (current & 4u) {
            if (!(previous & 4u)) {
                add_postflight_event(2);
                wr_u8(INFO_DELAY, 0x18);
            }
        } else if ((current & 2u) && !(previous & 2u) &&
                   rd_u8(MODE_SELECT) != 5u) {
            add_postflight_event(0x800);
        }
    } else if (current & 4u) {
        if (!(previous & 4u)) {
            add_postflight_event(2);
            wr_u8(INFO_DELAY, 0x18);
        } else if ((current & 2u) && !(previous & 2u) &&
                   rd_u8(MODE_SELECT) != 5u) {
            add_postflight_event(0x800);
        }
    } else if (!(current & 2u) || rd_u8(MODE_SELECT) == 5u) {
        /* The source masks MESSAGE_CODE to one byte before comparing it
         * with $800E; no word can satisfy that comparison. */
    } else if (!(previous & 2u)) {
        add_postflight_event(0x800);
    }
    wr_u8(THREAT_EVENTS, current);
}

void scan_postflight_variant_records(void) {
    uint16_t offset = 0;
    if (!rd_u8(POSTFLIGHT_SCAN_REQUEST)) return;
    wr_u8(POSTFLIGHT_SCAN_REQUEST, 0);

    if (rd_s16(SELECTED_RECORD) >= 0) {
        for (;;) {
            uint16_t word = rd_u16(LIST_BUFFER + offset + 12u);
            uint16_t record_offset = (uint16_t)((word & 0xFF00u) << 1);
            if (record_offset == rd_u16(SELECTED_RECORD)) {
                offset = (uint16_t)(offset + 16u);
                break;
            }
            offset = (uint16_t)(offset + 16u);
            if (offset >= 0x460u) {
                wr_u16(MESSAGE_CODE, 0x30u);
                offset = 0;
                break;
            }
        }
    }
    for (;;) {
        gaddr entry = LIST_BUFFER + offset;
        uint32_t combined = rd_u32(entry) | rd_u32(entry + 4) | rd_u32(entry + 8);
        uint16_t word, record_offset;
        gaddr record;
        uint8_t category;
        if (!combined) {
            wr_u8(SELECTION_ACTIVE, 0);
            wr_u16(SELECTED_RECORD, 0xFFFFu);
            return;
        }
        word = rd_u16(entry + 12);
        if ((word & 0x30u) != 0x30u) goto next;
        record_offset = (uint16_t)((word & 0xFF00u) << 1);
        record = CONTROL_RECORDS + (gaddr)(int32_t)(int16_t)record_offset;
        category = rd_u8(record + 0x62) & 0xF0u;
        if (category == 0 || category == 0x20u || category == 0x30u ||
            (rd_u8(record + 0x20) & 2u)) goto next;
        wr_u16(SELECTED_RECORD, record_offset);
        wr_u16(INFO_PAGE, 1);
        wr_u8(INFO_REQUEST, 1);
        wr_u8(INFO_DELAY, 0xFFu);
        wr_u8(SELECTION_ACTIVE, (uint8_t)((offset >> 4) + 1u));
        wr_u16(MESSAGE_STATE, (uint16_t)(rd_u16(MESSAGE_STATE) & 0xDFFFu));
        return;
next:
        offset = (uint16_t)(offset + 16u);
    }
}

static void run_postflight_variant_tail(const PostflightVariantHooks *hooks) {
    PostflightVariantWork work = {0};
    int active = begin_postflight_variant_tail(&work);
    if (hooks && hooks->after_prefix) hooks->after_prefix(&work, hooks->context);
    if (!active) return;
    process_postflight_variant_records(&work);
    resolve_postflight_variant_status(&work);
    scan_postflight_variant_records();
}

void draw_postflight_tuple_variant_with_hooks(const PostflightVariantHooks *hooks) {
    draw_postflight_tuple_pairs();
    if (hooks && hooks->after_head) hooks->after_head(hooks->context);
    run_postflight_variant_tail(hooks);
}

void draw_postflight_fixed_variant_with_hooks(const PostflightVariantHooks *hooks) {
    draw_postflight_fixed_quad();
    if (hooks && hooks->after_head) hooks->after_head(hooks->context);
    run_postflight_variant_tail(hooks);
}

void draw_postflight_tuple_variant(void) {
    draw_postflight_tuple_variant_with_hooks(0);
}

void draw_postflight_fixed_variant(void) {
    draw_postflight_fixed_variant_with_hooks(0);
}
