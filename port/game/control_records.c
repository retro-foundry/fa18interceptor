/* Control records. */
#include "control_records.h"

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
    wr_u32(0xC45B50u, 0);
    wr_u32(0xC45B54u, 0);
    wr_u16(0xC45AE0u, 0);
    wr_u16(0xC45ADEu, 0);
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
