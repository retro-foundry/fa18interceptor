/* Control records. */
#include "control_records.h"

#include "audio.h"
#include "fault.h"
#include "fixed_math.h"
#include "globals.h"
#include "messages.h"
#include "matrix.h"
#include "stages.h"
#include "tracking.h"

gaddr control_record(uint16_t selector) {
    /* The original adds the doubled high-byte index as a signed word. */
    return CONTROL_RECORDS + (gaddr)(int32_t)(int16_t)((selector & 0xFF00) * 2);
}

gaddr workspace_record(uint16_t selector) {
    return WORKSPACE_RECORDS + ((selector & 0xFF00u) >> 3);
}

int32_t add_workspace_cell_steps(gaddr entry, int16_t column, int16_t row,
                                 uint32_t *x, uint32_t *z) {
    gaddr record = workspace_record(rd_u16(entry));
    int32_t dx = cell_step((int16_t)((rd_u16(record + 6) & 0xFFu) - column));
    int32_t dz = cell_step((int16_t)((rd_u16(record + 8) & 0xFFu) - row));
    *x += (uint32_t)dx;
    *z += (uint32_t)dz;
    return dz;
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

int32_t record_6e_step(void) {
    gaddr record = rd_u32(CURRENT_RECORD);
    int16_t step;
    if ((rd_u8(record + 0x62) & 0xF0) == 0x30) return 0x10;
    step = rd_s16(record + 0x6E);
    if (step < 0) step = (int16_t)(0u - (uint16_t)step);
    step = (int16_t)(step >> 9);
    if (step > 0x3F) step = 0x3F;
    if (!rd_u8(CONTEXT_SELECT)) step = (int16_t)(step >> 1);
    if (!step) step = 1;
    return step;
}

void begin_mission_reset(void) {
    uint8_t attempts;
    post_message(0x4005);
    if (rd_u8(MODE_SELECT) == 6) {
        attempts = rd_u8(ATTEMPTS_LEFT);
        if (attempts != 1 && attempts != 0xFF) wr_u8(ATTEMPTS_LEFT, 0);
    }
    reset_mission_objects();
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
    *x = (int32_t)(((uint32_t)*x << 8) + ((rd_u32(record + 0x14) & 0xFFFFF) >> shift));
    *z = (int32_t)(((uint32_t)*z << 8) + ((rd_u32(record + 0x1C) & 0xFFFFF) >> shift));
    *y = level >> shift;
    wr_s32(POSITION_LEVEL, (int32_t)((uint32_t)level + rd_u32(POSITION_BIAS)) >> shift);
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
            s->level_offset_valid = 1;
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
    state->level_offset_valid = 0;
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

static int16_t matrix_side_target(gaddr table, int16_t lane) {
    int16_t target = rd_s16(table + (gaddr)(2 * (lane < 0 ? -lane : lane)));
    return lane < 0 ? (int16_t)-target : target;
}

static void clear_matrix_side_status(void) {
    wr_u8(MATRIX_SIDE_STATUS, (uint8_t)(rd_u8(MATRIX_SIDE_STATUS) & (uint8_t)~0x80u));
}

void update_matrix_side_record(void) {
    int16_t index = rd_s16(MATRIX_SIDE_RECORD);
    gaddr record = CONTROL_RECORDS + (gaddr)(int32_t)(index * CONTROL_RECORD_BYTES);
    gaddr header = record + 2;
    gaddr table;
    int16_t x, y, z, target;

    wr_u32(CURRENT_RECORD, record);
    table = (index == 0 || (rd_u16(header) & 0x100))
          ? MATRIX_SIDE_ZERO_TARGETS : MATRIX_SIDE_ALT_TARGETS;
    wr_u16(header, (uint16_t)(rd_u16(header) & 0xFFBF));

    if (!(rd_u16(COCKPIT_FLAGS) & 0x40)) {
        wr_u32(WARNING_CAUSES, rd_u32(WARNING_CAUSES) & 0xFFFFFFBCu);
        clear_matrix_side_status();
        return;
    }

    if (index == 0) {
        if (rd_s32(MATRIX_SIDE_METRIC) <= 0) {
            wr_u32(MATRIX_SIDE_METRIC, 0);
            if (!(rd_u8(MATRIX_SIDE_STATUS) & 0x40)) {
                wr_u32(WARNING_CAUSES, (rd_u32(WARNING_CAUSES) & 0xFFFFFFFCu) | 0x40u);
                wr_u16(header, (uint16_t)(rd_u16(header) & 0xFFF7));
                wr_u8(BAR_REDRAWS_A, 2);
            }
        } else if (rd_s32(MATRIX_SIDE_METRIC) < 0x70800) {
            if (!(rd_u8(MATRIX_SIDE_STATUS) & 0x02))
                wr_u32(WARNING_CAUSES, (rd_u32(WARNING_CAUSES) & 0xFFFFFFBEu) | 0x02u);
        } else if (rd_s32(MATRIX_SIDE_METRIC) < 0xBB800) {
            if (!(rd_u8(MATRIX_SIDE_STATUS) & 0x01))
                wr_u32(WARNING_CAUSES, (rd_u32(WARNING_CAUSES) & 0xFFFFFFBDu) | 0x01u);
        } else {
            wr_u32(WARNING_CAUSES, rd_u32(WARNING_CAUSES) & 0xFFFFFFBCu);
        }
    }

    x = rd_s8(record + 0x28);
    y = rd_s8(record + 0x29);
    z = rd_s8(record + 0x2A);
    if (rd_u8(record + 4) & 0x08) y = z = 0;

    if (x != 0) {
        target = matrix_side_target(table, x);
        wr_s16(MATRIX_SIDE_TARGET_X, target);
        if (!(rd_u16(header) & 0x80)) {
            steer_record_56(target);
        } else if (rd_u8(record + 0x20) & 0x01) {
            update_record_56_from_66();
            update_record_5a();
        } else if (rd_s16(record + 0x6C) > 0x300 &&
                   (rd_s16(record + 0x66) == 0 || rd_s16(record + 0x66) > 0x6D60)) {
            if (x < 0) {
                int16_t lane = (int16_t)(-x & ~1);
                steer_record_56((int16_t)-rd_s16(table + (gaddr)lane));
            } else {
                update_record_56_from_66();
                update_record_5a();
            }
        } else {
            update_record_56_from_66();
            update_record_5a();
        }
    } else {
        wr_u16(MATRIX_SIDE_TARGET_X, 0);
        if (rd_u16(header) & 0x80) {
            update_record_56_from_66();
        } else if (rd_s16(record + 0x56) != 0 && rd_s16(record + 0x26) == 0) {
            decay_toward_zero(record + 0x56, index == 0 ? 2 : 1);
        }
    }

    if (y != 0) {
        target = matrix_side_target(table, y);
        wr_s16(MATRIX_SIDE_TARGET_Y, target);
        if (!(rd_u8(record + 4) & 0x02) && rd_s16(record + 0x6E) != 0)
            ease_record_58(target);
        else
            decay_toward_zero(record + 0x58, 1);
    } else if (z == 0 || !(rd_u16(header) & 0x80)) {
        wr_u16(MATRIX_SIDE_TARGET_Y, 0);
        if (rd_s16(record + 0x58) != 0) decay_toward_zero(record + 0x58, 1);
    }

    if (z != 0) {
        if (rd_u16(header) & 0x80) {
            if (z < -10) z = -10;
            else if (z > 10) z = 10;
        }
        target = matrix_side_target(table, z);
        wr_s16(MATRIX_SIDE_TARGET_Z, target);
        if (!(rd_u16(header) & 0x80)) {
            steer_record_5a((int16_t)-target);
        } else if ((rd_u8(record + 4) & 0x02) || rd_s16(record + 0x6E) == 0) {
            decay_toward_zero(record + 0x58, 1);
            decay_toward_zero(record + 0x5A, 1);
        } else {
            int16_t angle = rd_s16(record + 0x6A);
            ease_record_58(target);
            if (angle < 0x50 || angle > 0x7030 || rd_s16(record + 0x6E) <= 0x360 ||
                (angle < 0x3840 ? z >= 0 : z <= 0))
                steer_record_5a((int16_t)-target);
            else
                wr_u16(record + 0x5A, 0);
        }
    } else {
        wr_u16(MATRIX_SIDE_TARGET_Z, 0);
        if (rd_u16(header) & 0x80) update_record_5a();
        if (rd_s16(record + 0x5A) != 0) decay_toward_zero(record + 0x5A, 1);
    }

    if (index != 0) return;
    if ((rd_u16(COCKPIT_FLAGS) & 0x40) && rd_s32(MATRIX_SIDE_METRIC) != 0 &&
        (rd_u8(record + 0x20) & 0x04) && !(rd_u16(header) & 0x80)) {
        if (!(rd_u8(MATRIX_SIDE_STATUS) & 0x80)) {
            wr_u8(MATRIX_SIDE_STATUS, (uint8_t)(rd_u8(MATRIX_SIDE_STATUS) | 0x80));
            wr_u8(MATRIX_SIDE_EVENT_STATUS, (uint8_t)(rd_u8(MATRIX_SIDE_EVENT_STATUS) | 0x20));
        } else if (rd_s16(MATRIX_SIDE_RESPONSE) < 0) {
            wr_u16(MATRIX_SIDE_RESPONSE, 2);
        }
    } else {
        clear_matrix_side_status();
    }
}

/* A record's control byte +$65: bits 0-1 throttle (kept), 2-3 stick X,
 * 4-5 stick Y, 6-7 the rudder. */
#define REC_CONTROLS 0x65

static void set_controls(gaddr record, uint8_t value) {
    wr_u8(record + REC_CONTROLS, (uint8_t)((rd_u8(record + REC_CONTROLS) & 3) | value));
}

static uint8_t roll_toward(int16_t turn) { return turn == 0 ? 0 : turn < 0 ? 0x04 : 0x08; }

void steer_record_neutral(gaddr record) { set_controls(record, 0); }

void steer_record_roll(gaddr record, int16_t turn) { set_controls(record, roll_toward(turn)); }

void steer_record_turn(gaddr record, int16_t turn) {
    uint8_t flags = rd_u8(record + 0x64);
    int16_t heading = rd_s16(record + 0x6A);
    uint8_t value;

    if ((flags & 0x60) == 0x60) {
        /* Rudder toward the turn beyond +$58, with the roll that the bank
         * (+$6A, a half turn either way) still allows. */
        if (turn == 0) { set_controls(record, 0); return; }
        if (turn > 0 ? turn <= rd_s16(record + 0x58) : turn >= rd_s16(record + 0x58)) { set_controls(record, 0); return; }
        value = turn > 0 ? 0x80 : 0x40;
        if (heading > 0x3840) { if (heading <= 0x7030) value |= 0x08; }
        else if (heading > 0x50) value |= 0x04;
        set_controls(record, value);
        return;
    }
    if (flags & 0x80) {
        if (heading > 0x3840) {
            if (heading <= 0x6EF0) { set_controls(record, 0x08); return; }
        } else if (heading >= 0x190) {
            set_controls(record, 0x04);
            return;
        }
    }
    set_controls(record, roll_toward(turn));
}

void steer_record_pitch(gaddr record, int16_t climb) {
    int16_t limit = rd_s16(record + 0x56);
    uint8_t value = 0;
    if (climb >= 0 ? climb > limit : climb < limit) value = climb >= 0 ? 0x10 : 0x20;
    set_controls(record, value);
}

/* The true sign of a + b + c as the 68000 sees it after two ADD.L (the last
 * one's N xor V). */
static int true_sum_negative(int32_t a, int32_t b, int32_t c) {
    int32_t first = (int32_t)((uint32_t)a + (uint32_t)b);
    return (int64_t)first + c < 0;
}

static int32_t column_dot(gaddr record, const int16_t v[3]) {
    int k;
    uint32_t sum = 0;
    for (k = 0; k < 3; k++) sum += (uint32_t)((int32_t)rd_s16(record + 0x96 + (gaddr)(6 * k)) * v[k]);
    return (int32_t)sum;
}

void update_in_sight(gaddr target, gaddr viewer) {
    int16_t n[3], axis[3];
    int32_t d[3], facing, along;
    int k;

    if ((rd_u8(target + 0x39) & 0xF0) != 0x10) return;
    if (rd_s16(viewer + 0x4A) <= 0x3000) {
        for (k = 0; k < 3; k++)
            d[k] = (int32_t)(rd_u32(viewer + 0x14 + (gaddr)(4 * k)) - rd_u32(target + 0x14 + (gaddr)(4 * k))) >> 8;
        normalize_vector(0xC0, d[0], d[1], d[2]);
        for (k = 0; k < 3; k++) {
            n[k] = rd_s16(NORMALIZED + (gaddr)(2 * k));
            axis[k] = rd_s16(target + 0x96 + (gaddr)(6 * k));
        }
        facing = column_dot(viewer, n);
        if (true_sum_negative((int32_t)rd_s16(viewer + 0xA2) * n[2], (int32_t)rd_s16(viewer + 0x96) * n[0],
                              (int32_t)rd_s16(viewer + 0x9C) * n[1]) && facing <= -0x2C0000) {
            along = column_dot(viewer, axis);
            if (!true_sum_negative((int32_t)rd_s16(viewer + 0xA2) * axis[2], (int32_t)rd_s16(viewer + 0x96) * axis[0],
                                   (int32_t)rd_s16(viewer + 0x9C) * axis[1]) && along >= 0xD000000) {
                wr_u8(target + 4, (uint8_t)(rd_u8(target + 4) | 0x20));
                return;
            }
        }
    }
    wr_u8(target + 4, (uint8_t)(rd_u8(target + 4) & ~0x20));
}

#define ZONE_AREAS  0xC29720u /* per zone: a pointer to its box and exits */
#define ZONE_VIEWS  0xC295E0u /* word offsets from here to five view words */

void check_zone_exit(void) {
    int16_t index = rd_s16(STREAM_MODE), x, y, left, right, top, bottom, exits;
    uint16_t offset = (uint16_t)(index << 9);
    gaddr record, area;
    int8_t zone;

    if (!offset) return;
    record = CONTROL_RECORDS + (gaddr)(int32_t)(int16_t)offset;
    if ((rd_u8(record + 0x62) & 0xF0) != 0x10 || rd_u8(record + 5) == 8) return;
    zone = (int8_t)rd_u8(record + 0x5D);
    if (zone < 0) return;
    if (--zone < 0) {
        wr_u16(ERROR_CODE, 0x1E);
        fault_hook();
        return;
    }
    area = rd_u32(ZONE_AREAS + (gaddr)(4 * zone));
    x = rd_s16(record + 6);
    y = rd_s16(record + 8);
    left = rd_s16(area);
    right = rd_s16(area + 2);
    top = rd_s16(area + 4);
    bottom = rd_s16(area + 6);
    if (x >= left && x <= right && y >= top && y <= bottom) return;
    exits = rd_s16(area + 8);
    for (area += 10; exits-- > 0; area += 10) {
        gaddr view;
        if ((rd_s16(area + 4) & 0x7F) != index) continue;
        view = ZONE_VIEWS + (gaddr)(int32_t)rd_s16(ZONE_VIEWS + (gaddr)(int32_t)rd_s16(area + 6));
        if (rd_u8(record + 0x7A) == 3 || rd_u8(record + 0x7A) == 4) wr_u8(record + 0x7A, 5);
        wr_u16(record, (uint16_t)(rd_u16(record) & 0xFFFE));
        set_record_view(record, rd_s16(view), rd_s16(view + 2), rd_s16(view + 4), rd_s16(view + 6),
                        (uint32_t)(int32_t)rd_s16(view + 8));
        wr_u8(record + 0x38, 0xFF);
        return;
    }
}

/* Byte pairs a flagged template entry expands to, indexed by the record
 * header's high nibble ($C1D8B6). */
#define TEMPLATE_PAIRS 0xC1D8B6u
#define LEVEL_LIST_BYTES 0x5D
#define LEVEL_LIST_STRIDE 96

void expand_cell_templates(int16_t row, int16_t column, gaddr templates, gaddr bitmap,
                           gaddr lists, gaddr cursor, FilingState *state) {
    gaddr cell = templates + (gaddr)(int32_t)rd_s16(templates + (gaddr)(int32_t)(int16_t)(2 * row));
    int16_t word = (int16_t)(4 * (int16_t)(column >> 5) + (int16_t)(16 * row));

    if (rd_s16(cell) >= 0 && (rd_u32(bitmap + (gaddr)(int32_t)word) >> (column & 0x1F)) & 1) {
        gaddr entries = cell + (gaddr)(int32_t)rd_s16(cell) + 2;
        gaddr stream = rd_u32(entries + (gaddr)(int32_t)(int16_t)(4 * find_sorted_word(cell, column)));
        uint8_t previous = 0xFF;
        int left = 0;

        for (;;) {
            uint8_t header = rd_u8(stream++), level, flags;

            if (header == 0xFF) break;
            level = (uint8_t)(header & 0x0F);
            if (level != previous) {
                cursor = collect_records_in_cell(column, row, (int8_t)previous, cursor);
                left = 0x10;
                previous = level;
                cursor = lists + (gaddr)(LEVEL_LIST_STRIDE * level);
                wr_u32(LIST_END, cursor + LEVEL_LIST_BYTES);
            }
            if (--left < 0) fatal_error(0x38);
            if (cursor >= rd_u32(LIST_END)) break;
            flags = rd_u8(stream++);
            wr_u8(cursor++, (uint8_t)(flags & 0x80));
            wr_u8(cursor++, (uint8_t)(flags & 0x7F));
            if (flags & 0x80) {
                gaddr pair = TEMPLATE_PAIRS + (gaddr)(2 * ((header >> 4) & 0x0F));
                int16_t first = (int16_t)(int8_t)rd_u8(pair);
                wr_u16(cursor, (uint16_t)first);
                cursor += 2;
                wr_u16(cursor, (uint16_t)((first & 0xFF00) | rd_u8(pair + 1)));
                cursor += 2;
            } else {
                wr_u32(cursor, rd_u32(stream));
                stream += 4;
                cursor += 4;
            }
            wr_u8(cursor, 0xFF);
        }
        cursor = collect_records_in_cell(column, row, (int8_t)previous, cursor);
    }
    state->cursor = cursor;
    file_records_by_level(column, row, lists, state);
}

/* MOVEM.W sign-extends into the long, and SWAP then leaves that sign in
 * the low word. */
static int32_t swapped_word(int16_t v) {
    return (int32_t)(((uint32_t)(uint16_t)v << 16) | (uint16_t)(v < 0 ? 0xFFFF : 0));
}

void refresh_record_view_from_table(gaddr record) {
    gaddr entry;
    uint16_t flags;

    if (rd_u8(POST_INPUT_EVENT) || rd_u8(record + 5) != 8 ||
        (rd_u8(record + 2) & 1u) || rd_s16(record + 0x4A) > 0x480)
        return;

    entry = VIEW_PARAMETER_TABLE +
            (gaddr)(int32_t)rd_s16(VIEW_PARAMETER_TABLE +
                                   (gaddr)(rd_u8(record + 0x3A) * 2u));
    while (rd_u32(entry) != rd_u32(record + 0x2C) ||
           rd_u32(entry + 4) != rd_u32(record + 0x30)) {
        entry += 10;
        if (rd_s16(entry) < 0) {
            wr_u16(ERROR_CODE, 0x35);
            fault_hook();
            return;
        }
    }

    if (rd_s16(entry + 10) >= 0) {
        set_record_view(record, rd_s16(entry + 10), rd_s16(entry + 12),
                        rd_s16(entry + 14), rd_s16(entry + 16),
                        (uint32_t)(int32_t)rd_s16(entry + 18));
        wr_u16(record + 0x4A, 0x7FFF);
        return;
    }
    flags = rd_u16(record);
    wr_u8(record + 1, (uint8_t)(rd_u8(record + 1) & ~0x80u));
    if ((flags & 0x80u) && rd_u8(record + 0x62) == 0x15)
        wr_u16(record, rd_u16(record) | 0x0200u);
}

void update_linked_record_view(gaddr record) {
    int16_t selected = rd_s16(SELECTED_RECORD);
    uint16_t angle;

    wr_u16(record, rd_u16(record) & 0xFFFEu);
    if (rd_u8(record + 1) & 0x08u) {
        if (selected == -1 || selected == rd_s16(SCRIPT_RECORD)) goto ease_angle;
        {
            gaddr source = CONTROL_RECORDS + (gaddr)(int32_t)selected;
            set_record_view(record, rd_s16(source + 6), rd_s16(source + 8),
                            rd_s16(source + 0xC), rd_s16(source + 0xE),
                            rd_u32(source + 0x10));
            wr_u8(record + 0x38, (uint8_t)(((uint16_t)selected >> 9) | 0x80u));
        }
        return;
    }
    return;

ease_angle:
    wr_u32(record + 0x34, 0);
    angle = rd_u16(record + 0x6C);
    if ((int16_t)angle <= 0x4200) {
        angle = (uint16_t)(angle + 0x240u);
        if ((int16_t)angle > 0x4200) angle = 0x4200;
    } else {
        angle = (uint16_t)(angle - 0x240u);
        if ((int16_t)angle < 0x4200) angle = 0x4200;
    }
    wr_u16(record + 0x6C, angle);
    wr_u16(record + 0x6E, angle);
}

void resolve_record_zone_view(gaddr record) {
    gaddr list;
    uint8_t zone;

    if (!(rd_u8(record) & 0x10u)) goto done;
    list = rd_u32(POST_INPUT_RECORD_LIST);
    for (;;) {
        while (rd_s16(list) >= 0) {
            if ((rd_u16(list + 4) & 0xFFu) == rd_u16(STREAM_MODE)) {
                gaddr view = VIEW_PARAMETER_TABLE +
                             (gaddr)(int32_t)rd_s16(VIEW_PARAMETER_TABLE +
                                                    (gaddr)(int32_t)rd_s16(list + 6));
                set_record_view(record, rd_s16(view), rd_s16(view + 2),
                                rd_s16(view + 4), rd_s16(view + 6),
                                (uint32_t)(int32_t)rd_s16(view + 8));
                wr_u8(record + 0x38, 0xFF);
                wr_u8(record + 1, (uint8_t)(rd_u8(record + 1) & ~1u));
                goto done;
            }
            list += 10;
        }
        zone = rd_u8(record + 0x5D);
        if ((int8_t)zone <= 0) {
            wr_u16(ERROR_CODE, 0x34);
            fault_hook();
            goto done;
        }
        list = rd_u32(0xC29720u + (gaddr)((zone - 1u) * 4u)) + 10;
    }
done:
    wr_u8(FIRE_RECORD_PENDING, 0);
}

void finish_record_view_status(gaddr record, gaddr viewer) {
    gaddr table = 0xC2BB98u;
    int16_t range = rd_s16(viewer + 0x4A);
    uint8_t kind;
    int16_t row;

    if (rd_u8(record + 0x20) & 2u) goto done;
    update_in_sight(record, viewer);
    kind = rd_u8(record + 5);
    if (kind == 6) goto done;
    if (kind == 8 && (rd_u8(MODE_SELECT) != 5 || !rd_u8(FIRE_RECORD_PENDING)))
        goto done;
    if (!(rd_u8(record + 4) & 0x20u)) goto done;

    if (range <= 0x300) {
        int16_t difference = (int16_t)(rd_u16(record + 0x6C) - rd_u16(viewer + 0x6C));
        int negative = difference < 0;
        if (negative) difference = (int16_t)(0u - (uint16_t)difference);
        if (difference >= 0xC0) table += negative ? 0x20u : 0x40u;
    } else {
        table += 0x60;
        if (range > 0xC00) {
            table += 0x60;
            if (range > 0x1E00) {
                table += 0x60;
                goto select_row;
            }
        }
        {
            int16_t difference = (int16_t)(rd_u16(record + 0x6C) - rd_u16(viewer + 0x6C));
            int negative = difference < 0;
            if (negative) difference = (int16_t)(0u - (uint16_t)difference);
            if (difference >= 0xC0) table += negative ? 0x20u : 0x40u;
        }
        if (rd_s16(SELECTED_RECORD) >= 0 &&
            rd_s16(SELECTED_RECORD) == rd_s16(SCRIPT_RECORD) &&
            rd_u8(FIRE_RECORD_PENDING)) {
            wr_u8(FIRE_RECORD_PENDING, 0);
            table += 0x10;
        }
    }

select_row:
    row = rd_s8(SCENE_DISPATCH_LIMIT);
    if (row > 3) row = 3;
    table += (gaddr)(int32_t)(int16_t)(row * 4);
    wr_u16(record + 0x4C, rd_u16(table));
    kind = rd_u8(table + 3);
    if (rd_u8(record + 5) == 8 && (rd_u8(record + 1) & 8u)) {
        wr_u8(record + 1, (uint8_t)(rd_u8(record + 1) & ~8u));
        wr_u8(SCENE_DISPATCH_CREATED, (uint8_t)(rd_u8(SCENE_DISPATCH_CREATED) + 1u));
        wr_u8(SCENE_DISPATCH_ADMITTED, (uint8_t)(rd_u8(SCENE_DISPATCH_ADMITTED) + 1u));
    }
    wr_u8(record + 5, kind);
done:
    wr_u8(FIRE_RECORD_PENDING, 0);
}

int prepare_record_viewer(gaddr record, gaddr viewer, uint32_t *d4_state) {
    int32_t limit;
    int i;

    if (rd_u8(record + 0x7A) == 5) wr_u8(record + 0x7A, 3);
    if (!(rd_u8(viewer + 1) & 0x40u)) return 0;
    wr_u16(record, rd_u16(record) | 1u);
    if (viewer == CONTROL_RECORDS || rd_u8(record + 5) == 8) return 1;

    if (d4_state) *d4_state = rd_u32(CONTROL_RECORDS + 0x18);

    if (rd_s8(SCENE_DISPATCH_LIMIT) >= 3) limit = 0x300000;
    else if (rd_s8(SCENE_DISPATCH_LIMIT) >= 2) limit = 0x240000;
    else limit = 0x180000;
    for (i = 0; i < 3; ++i) {
        uint32_t difference = rd_u32(CONTROL_RECORDS + 0x14 + (gaddr)(4 * i)) -
                              rd_u32(record + 0x14 + (gaddr)(4 * i));
        if ((int32_t)difference < 0) difference = 0u - difference;
        if (i == 1 && d4_state) *d4_state = difference;
        if ((int32_t)difference > limit) return 1;
    }
    wr_u8(record + 0x38, 0x80);
    return 1;
}

void place_record_view_point(gaddr record, gaddr viewer, uint32_t d4_state,
                             RecordViewPointWork *work) {
    if (rd_u8(record + 2) & 1u) {
        uint8_t control = rd_u8(record + 0x64);
        if (control & 1u) {
            work->local[0] = 0;
            work->local[1] = (control & 2u) ? 0x60 : -0x60;
        } else {
            work->local[0] = (control & 2u) ? 0xA8 : -0x60;
            work->local[1] = 0;
        }
        work->local[2] = -0x30;
    } else {
        if (!(rd_u16(STREAM_SKIP) & 0xFFu)) {
            uint8_t first = rd_u8(viewer + 0x28);
            uint16_t d4_word;
            if ((int8_t)first < 0) first = (uint8_t)(0u - first);
            d4_word = (uint16_t)((d4_state & 0xFF00u) | first);
            if ((int16_t)d4_word > 0x14) {
                wr_u8(RECORD_VIEW_FLAG, (uint8_t)(rd_u16(viewer + 0x16) & 4u));
            } else {
                uint8_t second = rd_u8(viewer + 0x2A);
                if ((int8_t)second < 0) second = (uint8_t)(0u - second);
                if ((int8_t)second > 0x14)
                    wr_u8(RECORD_VIEW_FLAG, (uint8_t)(rd_u16(viewer + 0x16) & 4u));
            }
        }
        work->local[0] = 0;
        work->local[1] = 0;
        work->local[2] = -0x24;
    }

    local_to_world(viewer, viewer + RECORD_INVERSE,
                   work->local[0], work->local[1], work->local[2], work->world);
    wr_u16(record + 0x2C, (uint16_t)((int16_t)(work->world[0] >> 16) >> 6));
    wr_u16(record + 0x2E, (uint16_t)((int16_t)(work->world[2] >> 16) >> 6));
    wr_u16(record + 0x30, (uint16_t)((uint32_t)(work->world[0] >> 8) & 0x3FFFu));
    wr_u16(record + 0x32, (uint16_t)((uint32_t)(work->world[2] >> 8) & 0x3FFFu));
    wr_s32(record + 0x34, work->world[1] >> 8);
}

int update_record_view(gaddr record, gaddr incoming_viewer,
                       uint32_t incoming_d4, RecordViewUpdateWork *work) {
    uint8_t selector;

    work->record = record;
    work->viewer = incoming_viewer;
    work->d4_state = incoming_d4;
    work->dispatch_valid = 0;
    work->route = RECORD_VIEW_UPDATE_EARLY;
    if (rd_u8(POST_INPUT_EVENT)) return 1;

    refresh_record_view_from_table(record);
    if (rd_u8(record + 5) == 8) {
        work->route = RECORD_VIEW_UPDATE_MODE_EIGHT;
        finish_record_view_status(record, incoming_viewer);
        return 1;
    }
    work->dispatch_d1 = rd_u16(record);
    work->dispatch_valid = 1;
    if (rd_u16(record) & 8u) {
        work->dispatch_d1 &= 2u;
        if (rd_u16(record) & 2u) {
            work->route = RECORD_VIEW_UPDATE_LINKED;
            update_linked_record_view(record);
        }
        return 1;
    }

    selector = rd_u8(record + 0x38);
    work->viewer = CONTROL_RECORDS;
    if (selector != 0xFFu)
        work->viewer += (gaddr)((selector & 0x7Fu) << 9);
    if (!prepare_record_viewer(record, work->viewer, &work->d4_state)) {
        work->route = RECORD_VIEW_UPDATE_ZONE;
        resolve_record_zone_view(record);
        return 1;
    }
    work->route = RECORD_VIEW_UPDATE_PLACED;
    place_record_view_point(record, work->viewer, work->d4_state, &work->point);
    finish_record_view_status(record, work->viewer);
    return 1;
}

void aim_record_at_view(gaddr record, gaddr source) {
    int16_t select = rd_s16(source + 4);
    int32_t elevation = 0, azimuth = 0, x, z;

    if (select < 0) {
        int16_t at = (int16_t)(select & 0x7F00), v[5];
        gaddr entry;
        int k;

        if (!at) return;
        entry = VIEW_PARAMETER_TABLE
              + (gaddr)(int32_t)rd_s16(VIEW_PARAMETER_TABLE + (gaddr)(int32_t)(int16_t)(at >> 7));
        for (k = 0; k < 5; k++) v[k] = rd_s16(entry + (gaddr)(2 * k));
        set_record_view(record, v[0], v[1], v[2], v[3], (uint32_t)(int32_t)v[4]);
        wr_u8(record + 0x38, 0xFF);
        x = (int32_t)((uint32_t)swapped_word(v[0]) << 6) + (int32_t)((uint32_t)(int32_t)v[2] << 8);
        z = (int32_t)((uint32_t)swapped_word(v[1]) << 6) + (int32_t)((uint32_t)(int32_t)v[3] << 8);
    } else {
        uint16_t index = (uint16_t)(select & 0xFF00);
        gaddr other = CONTROL_RECORDS + (gaddr)(int32_t)(int16_t)(index * 2);

        if (!(rd_u16(other) & 0x40)) return;
        set_record_view(record, rd_s16(other + 6), rd_s16(other + 8), rd_s16(other + 0x0C),
                        rd_s16(other + 0x0E), rd_u32(other + 0x10));
        wr_u8(record + 0x38, (uint8_t)((index >> 8) | 0x80));
        x = rd_s32(other + 0x14);
        z = rd_s32(other + 0x1C);
    }
    x -= rd_s32(record + 0x14);
    z -= rd_s32(record + 0x1C);
    track_direction(&elevation, &azimuth, x, 0, z, -1);
    set_record_orientation(record, 0, rd_u16(TRACKED_HEADING), 0);
}
