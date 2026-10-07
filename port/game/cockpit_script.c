/* Cockpit display script handlers. */
#include "cockpit_script.h"

#include "globals.h"

static gaddr script_record(void) { return CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(SCRIPT_RECORD); }

gaddr skip_for_type_3_to_6(gaddr script) {
    int8_t type = (int8_t)(rd_u8(script_record() + 0x7C) & 0x0F);
    return script + (type >= 3 && type <= 6 ? 8 : 0);
}

gaddr skip_counted_entries(gaddr script) {
    int16_t count = (int16_t)(rd_u8(SCRIPT_COUNT) & 0x7F);
    return script + (gaddr)(int32_t)(int16_t)(count * 18);
}

gaddr select_workspace_script_block(gaddr script) {
    int16_t index=(int16_t)(rd_u16(STREAM_MODE)*32u);
    int16_t value=rd_s16(WORKSPACE_RECORDS+(gaddr)(int32_t)index+4);
    if(value<0) return script;
    script+=2u+0x86u*(((unsigned)value&7u)/2u);
    if(value<12) script+=0x26u+(gaddr)(int32_t)(int16_t)(((11-value)>>1)*8);
    else if(value>116) script+=0x56u+(gaddr)(int32_t)(int16_t)(((value-116)>>1)*8);
    return script;
}

gaddr skip_for_low_class(gaddr script) {
    int8_t cls = (int8_t)((int8_t)(rd_u8(script_record() + 0x7C) & 0x7F) >> 4);
    return script + (cls < 6 ? 2 : 0);
}

gaddr skip_to_type_block(gaddr script) {
    static const uint16_t block[7] = {0x00, 0x14, 0x52, 0x7C, 0xA6, 0xD0, 0xFA};
    int8_t type = (int8_t)(rd_u8(script_record() + 0x7C) & 0x0F);
    return script + (type <= 6 ? block[type] : 0);
}

int viewed_record_flagged(void) {
    if (rd_u8(CONTEXT_SELECT)) return 0;
    return (rd_u8(CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(VIEW_RECORD) + 0x04) & 0x40) != 0;
}
