/* Glue for the player setup, steering helpers, random bits, channel stop
 * and cockpit script handlers. */
#include "glue.h"
#include "ports_glue.h"

#include "audio.h"
#include "cockpit_script.h"
#include "control_records.h"
#include "fixed_math.h"
#include "globals.h"
#include "memory.h"

int glue_C09620(void) {
    prepare_player_record();
    A(0) = CONTROL_RECORDS;
    return glue_return();
}

/* After a steering helper: the nudge leaves the field in A0, its new value
 * in D0.w and the eased value (before the nudge) in D1.w. */
static void nudge_leftovers(gaddr field, int16_t eased) {
    A(0) = field;
    SET_W(D(0), rd_u16(field));
    SET_W(D(1), eased);
}

/* Value a steering helper writes before the nudge. */
static int16_t eased_value(int16_t before, int16_t target, int shift) {
    return (int16_t)(before - (int16_t)((int16_t)(before - target) >> shift));
}

/* $C13BA0: steer_record_56(short target), compiled C; the target slot is
 * rewritten when halved. */
int glue_C13BA0(void) {
    gaddr slot = A(7) + 6, r = rd_u32(CURRENT_RECORD);
    int16_t target = rd_s16(slot), before = rd_s16(r + 0x56);
    int early = !(rd_u16(r + 0x02) & 0x80) && rd_u16(r + 0x26) != 0 && target <= 0;
    int16_t used = steer_record_56(target);

    if (early) {
        SET_W(D(0), rd_u16(r + 0x26));
        A(0) = r;
        return glue_return();
    }
    wr_s16(slot, used);
    nudge_leftovers(r + 0x56, eased_value(before, used, 2));
    return glue_return();
}

/* $C13C64: steer_record_5a(short target), compiled C. */
int glue_C13C64(void) {
    gaddr slot = A(7) + 6, r = rd_u32(CURRENT_RECORD);
    int16_t before = rd_s16(r + 0x5A);
    int shift = rd_u8(r + 0x62) == 0x14 ? 2 : 1;
    int16_t used = steer_record_5a(rd_s16(slot));

    wr_s16(slot, used);
    nudge_leftovers(r + 0x5A, eased_value(before, used, shift));
    return glue_return();
}

/* $C50B02: random_bits(long count), compiled C; the count slot ends at -1
 * (or count - 1 when count <= 0). */
int glue_C50B02(void) {
    gaddr slot = A(7) + 4;
    int32_t count = rd_s32(slot);
    D(0) = (uint32_t)random_bits(count);
    wr_s32(slot, count > 0 ? -1 : count - 1);
    return glue_return();
}

/* $C180FC: leaves free_voice's D0 = 2 * 4 and A0 = the voice. */
int glue_C180FC(void) {
    stop_channel_2();
    D(0) = 8;
    A(0) = rd_u32(VOICE_TABLE + 8);
    return glue_return();
}

/* $C1EC96: cell steps from the record selected by (A1)'s high byte. */
int glue_C1EC96(void) {
    gaddr record = control_record(rd_u16(A(1)));
    int32_t dx = cell_step((int16_t)((rd_u16(record + 6) & 0xFF) - rd_s16(A(6) - 0x1C)));
    int32_t dz = cell_step((int16_t)((rd_u16(record + 8) & 0xFF) - rd_s16(A(6) - 0x1E)));
    D(2) += (uint32_t)dx;
    D(4) += (uint32_t)dz;
    D(1) = (uint32_t)dz;
    return glue_return();
}

/* Script handlers: A2 script, A3 = the record, D0 = 0 (MOVEQ flags). */
static gaddr script_record_address(void) { return CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(SCRIPT_RECORD); }

static int script_done(void) {
    A(3) = script_record_address();
    D(0) = 0;
    flags_logic_l(0);
    return glue_return();
}

int glue_C21916(void) {
    A(2) = skip_for_type_3_to_6(A(2));
    SET_B(D(1), rd_u8(script_record_address() + 0x7C) & 0x0F);
    return script_done();
}

int glue_C21966(void) {
    A(2) = skip_counted_entries(A(2));
    SET_W(D(1), (rd_u8(SCRIPT_COUNT) & 0x7F) * 18);
    return script_done();
}

int glue_C2198C(void) {
    A(2) = skip_for_low_class(A(2));
    SET_B(D(1), (int8_t)(rd_u8(script_record_address() + 0x7C) & 0x7F) >> 4);
    return script_done();
}

int glue_C218C8(void) {
    int8_t type = (int8_t)(rd_u8(script_record_address() + 0x7C) & 0x0F);
    A(2) = skip_to_type_block(A(2));
    /* The type is counted down by SUBQ until it goes negative. */
    SET_B(D(1), type > 6 ? type : type == 6 ? 0 : -1);
    return script_done();
}

/* $C207FE: sets the caller's -$7A(A6) flag when the viewed record is
 * flagged; D0 = 0. */
int glue_C207FE(void) {
    if (!rd_u8(CONTEXT_SELECT)) {
        A(3) = CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(VIEW_RECORD);
        SET_B(D(1), rd_u8(A(3) + 4) & 0x40);
        if (viewed_record_flagged()) wr_u16(A(6) - 0x7A, 1);
    }
    D(0) = 0;
    flags_logic_l(0);
    return glue_return();
}
