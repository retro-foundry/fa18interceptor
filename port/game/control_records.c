/* Control records. */
#include "control_records.h"

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
