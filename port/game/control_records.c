/* Control records. */
#include "control_records.h"

#include "audio.h"
#include "fault.h"
#include "fixed_math.h"
#include "globals.h"

gaddr control_record(uint16_t selector) {
    /* The original adds the doubled high-byte index as a signed word. */
    return CONTROL_RECORDS + (gaddr)(int32_t)(int16_t)((selector & 0xFF00) * 2);
}

void read_record_fields(gaddr record, int32_t *field_0c, int32_t *field_10, int32_t *field_0e) {
    *field_0c = rd_s16(record + REC_FIELD_0C);
    *field_10 = rd_s32(record + REC_FIELD_10);
    *field_0e = rd_s16(record + REC_FIELD_0E);
}

void release_lost_selection(void) {
    int16_t selected = rd_s16(SELECTED_RECORD);
    gaddr record;

    if (selected < 0) return;
    record = CONTROL_RECORDS + (gaddr)(int32_t)selected;
    if ((rd_u8(record + REC_FLAGS) & 0x40) && !(rd_u8(record + REC_STATUS) & 0x02)) return;
    wr_u16(SELECTED_RECORD, 0xFFFF);
    wr_u8(SELECTION_ACTIVE, 0);
    wr_u16(SELECTION_MARKER, 0xFFFF);
}

void settle_record(gaddr record) {
    if (rd_u16(record + REC_PENDING)) {
        wr_u16(record + REC_PENDING, 0);
        wr_s16(record + REC_PENDING_SIGN, (int16_t)-rd_s16(record + REC_PENDING_SIGN));
    } else {
        wr_u16(record + REC_PENDING, 0);
    }
}

static int16_t magnitude(int16_t v) { return (int16_t)(v < 0 ? -v : v); }

void classify_record_rate(gaddr record) {
    int16_t a = magnitude(rd_s16(record + 0x56));
    int16_t b = magnitude(rd_s16(record + 0x58));
    int16_t c = (int16_t)(magnitude(rd_s16(record + 0x5A)) >> 2);
    int16_t largest;

    if (b > a) largest = b > c ? b : c;
    else largest = c > a ? c : a;

    if (largest <= 0x60)
        wr_u8(RECORD_RATE, magnitude(rd_s16(record + 0x6C)) > 0x1000 ? 3 : 5);
    else
        wr_u8(RECORD_RATE, largest <= 0xC0 ? 3 : 1);
}

void reset_player_record(void) {
    gaddr p = CONTROL_RECORDS;
    wr_u16(p + 0x6C, 0);
    wr_u16(p + 0x6E, 0);
    wr_u16(p + 0x78, 0);
    wr_u32(p + 0x3E, 0);
    wr_u32(p + 0x42, 0);
    wr_u32(p + 0x46, 0);
    wr_u32(p + 0x56, 0);
    wr_u16(p + 0x5A, 0);
    wr_u32(p + 0x50, 0);
    wr_u16(p + 0x54, 0);
    wr_u8(p + 0x65, 0);
    wr_u8(p + 0x2B, 9);
    wr_u16(p + 0x00, (uint16_t)(rd_u16(p + 0x00) & 0x7FFF));
    wr_u32(WARNING_CAUSES, 0);
    wr_u32(EVENT_BITS, 0);
    wr_u16(MESSAGE_CODE, 0);
    wr_u16(MESSAGE_SHOWN, 0);
    wr_u16(0xC45AE4u, 0xFFFF);
}

void update_record_5a(void) {
    gaddr r = rd_u32(CURRENT_RECORD);
    int16_t level = rd_s16(r + 0x6A);
    if (level == 0) wr_u16(r + 0x5A, 0);
    else wr_s16(r + 0x5A, level > 14400 ? 0x20 : -0x20);
}

void ease_record_26(int16_t target) {
    gaddr r = rd_u32(CURRENT_RECORD);
    int16_t value = rd_s16(r + 0x26);
    int16_t step = (int16_t)((int16_t)(value - target) >> 3);
    wr_s16(r + 0x26, (int16_t)(value - step));
}

void mark_record_pending(gaddr record) {
    int was_pending = rd_u16(record + REC_PENDING) != 0;
    wr_u16(record + REC_PENDING, 0xFFFF);
    if (!was_pending) wr_s16(record + REC_PENDING_SIGN, (int16_t)-rd_s16(record + REC_PENDING_SIGN));
}

void set_record_view(gaddr record, int16_t a, int16_t b, int16_t c, int16_t d, uint32_t e) {
    wr_s16(record + 0x2C, a);
    wr_s16(record + 0x2E, b);
    wr_s16(record + 0x30, c);
    wr_s16(record + 0x32, d);
    wr_u32(record + 0x34, e);
}

void flag_all_records(void) {
    int i;
    for (i = 0; i < 16; i++) {
        gaddr r = CONTROL_RECORDS + (gaddr)(i * CONTROL_RECORD_BYTES) + REC_FLAGS;
        wr_u8(r, (uint8_t)(rd_u8(r) | 0x10));
    }
    for (i = 0; i < 16; i++) {
        gaddr w = WORKSPACE_RECORDS + (gaddr)(i * WORKSPACE_RECORD_BYTES) + 1;
        wr_u8(w, (uint8_t)(rd_u8(w) | 0x10));
    }
}

void read_record_pair(gaddr entry, int16_t *high, int16_t *low) {
    uint16_t head = rd_u16(entry);
    if (!(head & 0x10)) {
        uint16_t pair = rd_u16(entry + 0x0E);
        *low = (int16_t)(pair & 0xFF);
        *high = (int16_t)(pair >> 8);
    } else {
        gaddr record = control_record(head);
        *high = (int16_t)(rd_u16(record + 0x06) & 0xFF);
        *low = (int16_t)(rd_u16(record + 0x08) & 0xFF);
    }
}

/* Headings are in 1/80 degree, 0-28799. */
#define FULL_TURN 28800

void update_view_matrix(void) {
    gaddr record = CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(VIEW_RECORD);
    int16_t heading = rd_s16(record + 0x68);
    y_rotation_matrix(heading ? (int16_t)(FULL_TURN - heading) : 0, VIEW_MATRIX);
}

void update_compass(void) {
    gaddr record = CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(VIEW_RECORD);
    uint32_t tenths = (uint32_t)((int32_t)rd_s16(record + 0x68) >> 3);
    uint16_t degrees = (uint16_t)tenths;
    int16_t tape;

    /* DIVU.W: on overflow the dividend is left in place. */
    if (tenths / 10 <= 0xFFFF) degrees = (uint16_t)(tenths / 10);
    wr_u16(COMPASS_DEGREES, degrees);
    tape = (int16_t)(((uint32_t)(uint16_t)((int16_t)degrees >> 1) * 135) >> 8);
    if ((int32_t)tape - 4 < 0) tape = (int16_t)(tape - 4 + 96);
    else tape = (int16_t)(tape - 4);
    wr_s16(COMPASS_TAPE, tape);
}

void nudge_outside_dead_zone(gaddr value) {
    int16_t step = (int16_t)(rd_s16(rd_u32(CURRENT_RECORD) + 0x6C) >> 7);
    int16_t v = rd_s16(value);
    if (v >= 0) {
        if (v < 0x500) step = 0;
        wr_s16(value, (int16_t)(v + step));
    } else {
        if (v > -0x500) step = 0;
        wr_s16(value, (int16_t)(v - step));
    }
}

void reset_mission_objects(void) {
    int record, i;
    wr_u32(CONTROL_RECORDS + 0x72, 6400000);
    wr_u8(CONTROL_RECORDS + 0x5F, 0x24);
    wr_u16(CONTROL_RECORDS + 0x60, 500);
    wr_u8(REDRAW_FIRST + 0xD, 3);
    wr_u8(REDRAW_FIRST + 0xE, 3);
    wr_u8(MISSION_LEVEL_A, 0x10);
    wr_u8(MISSION_LEVEL_B, 0x10);
    wr_u8(MISSION_FLAGS_A, 0);
    wr_u8(MISSION_FLAGS_C, 0);
    wr_u8(MISSION_FLAGS_B, 0);
    wr_u16(MISSION_COUNTER, 0);
    for (record = 1; record <= 3; record++)
        for (i = 0; i < 41; i++) wr_u32(CONTROL_RECORDS + (gaddr)(record * CONTROL_RECORD_BYTES + 4 * i), 0);
}

void prepare_player_record(void) {
    gaddr p = CONTROL_RECORDS;
    wr_u8(p + 0x21, (uint8_t)(rd_u8(p + 0x21) & ~1));
    wr_u8(p + 0x63, 0x0D);
    reset_mission_objects();
    wr_u16(p + 0x00, 0x148);
    wr_u16(p + 0x00, (uint16_t)(rd_u16(p + 0x00) | 0x1080));
    wr_u16(p + 0x7E, 0x1400);
    wr_u8(PLAYER_FLAGS_A, 0);
    wr_u8(PLAYER_FLAGS_B, 0);
    wr_u8(PLAYER_FLAGS_C, 0);
    wr_u8(PLAYER_FLAGS_D, 0);
    wr_u8(PLAYER_FLAGS_E, 0);
    wr_u8(PLAYER_FLAGS_F, 0);
    wr_u8(PLAYER_FLAGS_G, 0);
    wr_u16(PLAYER_LIMIT, 0x7FFF);
    wr_u8(PLAYER_READY, 1);
    wr_u8(p + 0x71, 0xFF);
    wr_u16(SELECTED_RECORD, 0xFFFF);
    wr_u8(SELECTION_ACTIVE, 0);
    if (rd_u8(PLAYER_PHASE)) wr_u8(PLAYER_PHASE, 4);
}

static void ease_field(gaddr field, int16_t target, int shift) {
    int16_t v = rd_s16(field);
    wr_s16(field, (int16_t)(v - (int16_t)((int16_t)(v - target) >> shift)));
    nudge_outside_dead_zone(field);
}

int16_t steer_record_56(int16_t target) {
    gaddr r = rd_u32(CURRENT_RECORD);
    if (!(rd_u16(r + 0x02) & 0x80) && rd_u16(r + 0x26) != 0 && target <= 0) return target;
    if (rd_u8(r + 0x20) & 0x04) target = (int16_t)(target >> 1);
    ease_field(r + 0x56, target, 2);
    return target;
}

int16_t steer_record_5a(int16_t target) {
    gaddr r = rd_u32(CURRENT_RECORD);
    target = five_eighths(target);
    if (rd_u8(r + 0x20) & 0x04) target = (int16_t)(target >> 1);
    ease_field(r + 0x5A, target, rd_u8(r + 0x62) == 0x14 ? 2 : 1);
    return target;
}

int paired_record_ready(gaddr record) {
    int8_t partner;
    uint8_t cls;

    if (rd_u16(record) & 0x8700) return 0;
    if (rd_u8(record + 0x20) & 0x02) return 0;
    if (rd_u8(PAIR_OVERRIDE)) goto ready;
    if (!(rd_u8(record + 0x01) & 0x40)) return 0;
    partner = (int8_t)rd_u8(record + 0x38);
    if (partner < 0) {
        gaddr other = CONTROL_RECORDS + (gaddr)(int32_t)(int16_t)((partner & 0x7F) << 9);
        if (!(rd_u8(other + 0x01) & 0x40)) return 0;
        if (rd_u8(other + 0x20) & 0x02) return 0;
        if (rd_u8(other + 0x01) & 0x01) return 0;
    }
    if ((rd_u8(record + 0x64) & 0x60) != 0x60) return 0;
    if (!(rd_u8(record + 0x01) & 0x01)) return 0;
    cls = (uint8_t)(rd_u8(record + 0x63) & 0xF0);
    if (cls != 0x20 && cls != 0x30) return 0;
ready:
    wr_u8(PAIR_OVERRIDE, 0);
    return 1;
}

int16_t attitude_term(void) {
    gaddr r = rd_u32(CURRENT_RECORD);
    int16_t angle_66 = rd_s16(r + 0x66), angle_6a, size_56, sum;
    int32_t offset_18;

    if (angle_66 >= 0x3840) angle_66 = (int16_t)(angle_66 - 0x7080);
    angle_66 = (int16_t)(angle_66 >> 1);
    angle_6a = rd_s16(r + 0x6A);
    if (angle_6a >= 0x3840) angle_6a = (int16_t)(0x7080 - angle_6a);
    if (angle_6a >= 0x1C20) angle_6a = (int16_t)(0x3840 - angle_6a);
    angle_6a = (int16_t)(angle_6a >> 3);
    size_56 = rd_s16(r + 0x56);
    if (size_56 < 0) size_56 = (int16_t)-size_56;
    sum = (int16_t)(size_56 * 2 + angle_6a - angle_66);
    offset_18 = rd_s32(REFERENCE_18) - rd_s32(r + 0x18);
    offset_18 >>= (rd_u16(r + 0x02) & 0x08) ? 11 : 13;
    return (int16_t)(sum + offset_18);
}

void record_position_history(void) {
    int16_t offset = rd_s16(SCRIPT_RECORD);
    gaddr record, slot;
    int copies, i;
    int8_t length;

    if (offset != rd_s16(HISTORY_RECORD)) return;
    record = CONTROL_RECORDS + (gaddr)(int32_t)offset;
    if (rd_u8(POST_INPUT_EVENT)) return;

    slot = HISTORY_SLOTS + (gaddr)(int32_t)(int16_t)(rd_s8(HISTORY_NEXT) * 12);
    copies = (int8_t)rd_u8(HISTORY_COUNT) < 5 ? 2 : 1;
    while (copies--)
        for (i = 0; i < 12; i += 4, slot += 4) wr_u32(slot, rd_u32(record + 0x14 + (gaddr)i));
    wr_u8(HISTORY_NEXT, (uint8_t)(rd_u8(HISTORY_NEXT) + 1));
    if ((int8_t)rd_u8(HISTORY_NEXT) > 5) wr_u8(HISTORY_NEXT, 0);
    if ((int8_t)rd_u8(HISTORY_COUNT) < 6) wr_u8(HISTORY_COUNT, (uint8_t)(rd_u8(HISTORY_COUNT) + 1));

    if (rd_u16(record + 0x6E) && (rd_u8(record + 0x02) & 0x10)) {
        length = (int8_t)(rd_u8(HISTORY_COUNT) - 1);
        if (length >= 5) length = 4;
    } else {
        length = (int8_t)rd_u8(record + 0x3D);
        if (length == 0 || --length <= 0) {
            wr_u8(HISTORY_COUNT, 0);
            wr_u8(HISTORY_NEXT, 0);
        }
    }
    wr_u8(record + 0x3D, (uint8_t)length);
}

static gaddr collect_in(gaddr base, int stride, uint8_t kind, int16_t column, int16_t row,
                        int8_t level, gaddr events) {
    int i;
    for (i = 0; i < 16; i++) {
        gaddr r = base + (gaddr)(i * stride);
        uint8_t flags = rd_u8(r + 1);
        if ((flags & 0x40) && (flags & 0x10) && rd_s16(r + 6) == column && rd_s16(r + 8) == row &&
            (int8_t)rd_u8(r + 0x0A) == level) {
            wr_u8(r + 1, (uint8_t)(flags & ~0x10));
            wr_u8(events++, kind);
            wr_u8(events++, (uint8_t)i);
            wr_u8(events, 0xFF);
        }
    }
    return events;
}

gaddr collect_records_in_cell(int16_t column, int16_t row, int8_t level, gaddr events) {
    if (level < 0 || !rd_u8(CELL_CHECKS)) return events;
    wr_u16(CELL_TIMER, 0x50);
    events = collect_in(CONTROL_RECORDS, CONTROL_RECORD_BYTES, 0x10, column, row, level, events);
    return collect_in(WORKSPACE_RECORDS, WORKSPACE_RECORD_BYTES, 0x40, column, row, level, events);
}

void accumulate_record_position(gaddr record, int shift, int32_t *x, int32_t *y, int32_t *z) {
    int32_t level = rd_s32(record + 0x18);
    *x = (int32_t)((uint32_t)*x << 8) + ((int32_t)(rd_u32(record + 0x14) & 0xFFFFF) >> shift);
    *z = (int32_t)((uint32_t)*z << 8) + ((int32_t)(rd_u32(record + 0x1C) & 0xFFFFF) >> shift);
    *y = level >> shift;
    wr_s32(POSITION_LEVEL, (int32_t)(level + rd_s32(POSITION_BIAS)) >> shift);
    wr_u8(POSITION_VALID, 1);
}

/* The 2.14 sine table as 2.8: entry i and i + 1. */
static int16_t sine8(int16_t index) { return (int16_t)(rd_s16(SINE_TABLE + (gaddr)(int32_t)(int16_t)(index * 2)) >> 6); }

static int16_t table_by_magnitude(gaddr table, int16_t v) {
    int16_t i = (int16_t)((int16_t)(v < 0 ? -v : v) >> 7);
    if (i > 30) i = 30;
    return rd_s16(table + (gaddr)(int32_t)(int16_t)(i * 2));
}

void update_record_76_78(void) {
    gaddr r = CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(SCRIPT_RECORD);
    int16_t limit, angle, cosine_term, current, step, next;

    if (!rd_u8(RECORD_UPDATES_ON)) {
        wr_u16(r + 0x76, 0);
        wr_u16(r + 0x78, 0);
        return;
    }
    limit = table_by_magnitude(TABLE_78_LIMIT, rd_s16(r + 0x6C));
    angle = (int16_t)(rd_s16(r + 0x66) >> 3);
    if (angle <= 0x708) {
        cosine_term = (int16_t)(0x100 - sine8((int16_t)(0x384 - angle)));
    } else {
        cosine_term = (int16_t)-(int16_t)(0x100 - sine8((int16_t)(0x384 - (0xE10 - angle))));
    }
    limit = (int16_t)(((int32_t)limit * cosine_term) >> 3);

    current = rd_s16(r + 0x78);
    if (rd_u8(r + 0x03) & 0x80) {
        next = 0;
    } else {
        if ((int8_t)rd_u8(r + 0x7C) >= 0) limit = (int16_t)(limit - (int16_t)(limit >> 2));
        if (current <= limit) {
            next = (int16_t)(current - (int16_t)((int16_t)(current - limit) >> 5));
        } else {
            step = (int16_t)(current - limit);
            if (step > 0xFF) next = (int16_t)(current - (int16_t)(step >> 8));
            else if (current == 0) next = 0;
            else if (current < 0) next = 0;
            else next = (int16_t)(current - 1);
        }
    }
    wr_s16(r + 0x78, next);
    wr_s16(r + 0x76, table_by_magnitude(TABLE_76_TARGET, rd_s16(r + 0x6E)));
}

void update_record_56_from_66(void) {
    gaddr r = rd_u32(CURRENT_RECORD);
    int16_t angle = rd_s16(r + 0x66), target, value;
    int shift;

    if (angle <= 0) return;
    if (angle < 400) {
        uint16_t flags = rd_u16(r + 0x02);
        if (!(flags & 0x40)) {
            wr_u16(r + 0x02, (uint16_t)(flags | 0x40));
            if ((rd_u16(r) & 0x1600) == 0x1000 && !(rd_u8(r + 0x04) & 0x02)) play_alert_tone(15);
        }
        wr_u16(r + 0x56, 0);
        return;
    }
    shift = rd_s16(r + 0x6C) >= 0x6C0 ? 1 : 0;
    target = angle >= 0x3840 ? 0x40 : -0x40;
    value = rd_s16(r + 0x56);
    wr_s16(r + 0x56, (int16_t)(value - (int16_t)((int16_t)(value - target) >> shift)));
}

/* File one bank; 0 when a list was full (the whole filing stops). */
static int file_bank(gaddr base, int stride, uint8_t kind, int16_t column, int16_t row, gaddr lists,
                     FilingState *s, uint16_t error) {
    s->level = -1;
    for (s->index = 0; s->index < 16; s->index++) {
        gaddr r = base + (gaddr)(s->index * stride);
        uint8_t flags = rd_u8(r + 1);
        int8_t level;
        if (!(flags & 0x40) || !(flags & 0x10) || rd_s16(r + 6) != column || rd_s16(r + 8) != row) continue;
        level = (int8_t)rd_u8(r + 0x0A);
        if (level != s->level) {
            gaddr end;
            s->level = level;
            if (level < 0) fatal_error(error);
            s->level_offset = (int16_t)(level * 32);
            s->cursor = lists + (gaddr)(int32_t)(int16_t)(level * 96);
            end = s->cursor + 0x5D;
            wr_u32(LIST_END, end);
            for (;;) {
                int8_t b;
                if (s->cursor >= end) return 0;
                b = (int8_t)rd_u8(s->cursor);
                if (b < 0) break;
                s->cursor += (b & 0x50) ? 2 : 4;
            }
        }
        wr_u8(r + 1, (uint8_t)(rd_u8(r + 1) & ~0x10));
        wr_u8(s->cursor, kind);
        wr_u8(s->cursor + 1, (uint8_t)s->index);
        wr_u8(s->cursor + 2, 0xFF);
        s->cursor += 2;
    }
    return 1;
}

void file_records_by_level(int16_t column, int16_t row, gaddr lists, FilingState *state) {
    if (!rd_u8(CELL_CHECKS)) return;
    wr_u16(CELL_TIMER, 0x51);
    if (!file_bank(CONTROL_RECORDS, CONTROL_RECORD_BYTES, 0x10, column, row, lists, state, 0x0E)) return;
    file_bank(WORKSPACE_RECORDS, WORKSPACE_RECORD_BYTES, 0x40, column, row, lists, state, 0x36);
}

int16_t ease_record_58(int16_t target) {
    gaddr r = rd_u32(CURRENT_RECORD), value = r + 0x58;
    int16_t old;

    target = five_eighths(target);
    if (rd_u8(r + 0x20) & 4) target = (int16_t)(target >> 1);
    old = rd_s16(value);
    wr_s16(value, (int16_t)(old - (int16_t)((int16_t)(old - target) >> 2)));
    nudge_outside_dead_zone(value);
    return target;
}
