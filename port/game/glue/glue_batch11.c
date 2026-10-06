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
