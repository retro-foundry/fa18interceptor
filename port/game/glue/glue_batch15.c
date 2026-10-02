/* Glue for collect_records_in_cell, accumulate_record_position and
 * update_record_76_78. */
#include "glue.h"
#include "ports_glue.h"

#include "control_records.h"
#include "globals.h"
#include "memory.h"

/* $C1D520: D3.w column, D2.w row, D1.b level, A2 events (advanced). */
int glue_C1D520(void) {
    A(2) = collect_records_in_cell((int16_t)D(3), (int16_t)D(2), (int8_t)D(1), A(2));
    return glue_return();
}

/* $C1D0B6: D7 selects the record (high byte) and shift (low nibble);
 * D2/D4 accumulate, D3 = level; leaves D1 = the biased level, D6.w =
 * shift and A2 = the record. */
int glue_selected_position(int workspace);
int glue_C1D0B6(void) { return glue_selected_position(0); }

static int16_t sine8_at(int16_t index) {
    return (int16_t)(rd_s16(SINE_TABLE + (gaddr)(int32_t)(int16_t)(index * 2)) >> 6);
}

/* $C26428: leaves the trig terms in D3.w/D4.w; D1's high word is MULS's,
 * D0's is cleared when a table index was clamped. */
void record_76_78_registers(gaddr r, int16_t m6c, int16_t m6e,
                            int16_t old76, int enabled) {
    int16_t angle = (int16_t)(rd_s16(r + 0x66) >> 3);
    int16_t i6c = (int16_t)((int16_t)(m6c < 0 ? -m6c : m6c) >> 7);
    int16_t i6e = (int16_t)((int16_t)(m6e < 0 ? -m6e : m6e) >> 7);
    int16_t index, cosine_term, limit, target76;
    int32_t product;

    if (!enabled) return;

    if (i6c > 30) i6c = 30;
    limit = rd_s16(TABLE_78_LIMIT + (gaddr)(int32_t)(int16_t)(i6c * 2));
    index = angle <= 0x708 ? (int16_t)(0x384 - angle) : (int16_t)(0x384 - (0xE10 - angle));
    cosine_term = (int16_t)(0x100 - sine8_at(index));
    if (angle > 0x708) cosine_term = (int16_t)-cosine_term;
    product = ((int32_t)limit * cosine_term) >> 3;
    target76 = rd_s16(r + 0x76);

    SET_W(D(3), cosine_term);
    SET_W(D(4), sine8_at((int16_t)(index + 1)));
    D(1) = ((uint32_t)product & 0xFFFF0000u) | (uint16_t)(old76 - (int16_t)((int16_t)(old76 - target76) >> 6));
    if ((int16_t)((int16_t)(m6c < 0 ? -m6c : m6c) >> 7) > 30 || i6e > 30) D(0) &= 0xFFFFu;
    SET_W(D(0), (int16_t)((int16_t)(old76 - target76) >> 6));
}

int glue_C26428(void) {
    gaddr r = CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(SCRIPT_RECORD);
    int16_t m6c = rd_s16(r + 0x6C), m6e = rd_s16(r + 0x6E), old76 = rd_s16(r + 0x76);
    int enabled = rd_u8(RECORD_UPDATES_ON) != 0;
    update_record_76_78();
    record_76_78_registers(r, m6c, m6e, old76, enabled);
    return glue_return();
}
