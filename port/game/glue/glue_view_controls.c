/* Register effects of the view and targeting control pass ($C12098). */
#include "glue.h"
#include "ports_glue.h"

#include "audio.h"
#include "globals.h"
#include "memory.h"
#include "stages.h"

void view_key_leftovers(uint8_t raw, int queued);

int glue_C12098(void) {
    uint8_t context = rd_u8(CONTEXT_SELECT);
    int start_zero = !context && !rd_u8(0xC45891u);
    uint8_t key_taken = rd_u8(KEY_TAKEN);
    int8_t key_count = rd_s8(KEY_COUNT);
    int8_t key_dst = rd_s8(KEY_TRANSLATED_WRITE);
    gaddr viewed = CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(VIEW_RECORD);
    uint8_t viewed_type = rd_u8(viewed + 0x62) & 0xF0;
    int16_t target = rd_s16(TARGET_RECORD);
    gaddr record = CONTROL_RECORDS + (gaddr)((int32_t)target << 9);
    uint8_t type = rd_u8(record + 0x62) & 0xF0;
    uint16_t command = rd_u16(COMMAND_WORD);
    uint16_t span = rd_u16(SPAN_ORIGIN);
    uint8_t selection = context;
    uint8_t state = rd_u8(CONTEXT_STATE);
    int32_t height = rd_s32(0xC45C42u);
    uint16_t row = rd_u16(LINE_LAST_ROW);
    int reset = (command & 2) != 0;
    int type_resets_span;
    uint32_t initial_d4 = D(4);

    update_view_controls();

    if (start_zero) {
        SET_B(D(7), 0);
        if (viewed_type != 0x30) D(4) = 0;
        else SET_B(D(4), viewed_type);
        if (viewed_type != 0x30) {
            span = (uint16_t)rd_s8(0xC1BAD4u);
            row = 0x90;
        }
        if (!key_taken && key_count < 10) {
            D(4) = (uint16_t)(int16_t)key_dst;
            A(3) = KEY_TRANSLATED;
        }
        key_taken = 1;
    } else {
        D(4) = initial_d4;
    }
    D(0) = (uint32_t)((int32_t)target * 512);
    D(1) = command;
    SET_B(D(0), type);
    A(0) = record;

    if (reset) {
        SET_W(D(1), command & (uint16_t)~2u);
        if (context) {
            selection = 0;
            span = 0;
        } else {
            selection = rd_u8(0xC45833u);
            span = 0x32;
        }
        D(0) = 12;
        A(0) = rd_u32(VOICE_TABLE + 12);
        if (type == 0x30) span = 0x32;
        SET_W(D(0), (uint16_t)(span << 4));
        {
            uint8_t raw = (uint8_t)D(0);
            int queued = !key_taken && !(raw & 0x80) && key_count < 10;
            view_key_leftovers(raw, queued);
            if (!key_taken && !(raw & 0x80)) key_taken = 1;
        }
        if (type == 0x30 || selection) row = rd_u8(PAUSE_A) ? 0xB3 : 0xA7;
    }

    type_resets_span = type == 0x30 && span == 0;
    if (type == 0x30) {
        SET_W(D(0), span);
        if (type_resets_span) {
            uint8_t raw = (uint8_t)D(0);
            int queued = !key_taken && key_count < 10;
            span = 0x32;
            view_key_leftovers(raw, queued);
            row = 0xA7;
        }
    }
    if (selection) {
        SET_W(D(0), reset ? (command & (uint16_t)~2u) : command);
        if (D(0) & 0x10u) SET_B(D(0), rd_u8(TARGET_ENABLED));
        SET_B(D(0), state);
        SET_B(D(0), (uint8_t)(D(0) - 6));
        if (state == 6 && height < 0x24000 && row != 0xA7) {
            SET_W(D(0), row);
            D(4) = 3;
        }
    }
    return glue_return();
}
