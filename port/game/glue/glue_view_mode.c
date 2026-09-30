/* C1B906 enters the shared view-mode tail with mode zero. */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "memory.h"
#include "stages.h"

void view_mode_zero_registers(void) {
    uint8_t raw = (uint8_t)D(0);
    uint8_t taken = rd_u8(KEY_TAKEN), count = rd_u8(KEY_COUNT);
    int8_t slot = rd_s8(KEY_WRITE), dst = rd_s8(KEY_TRANSLATED_WRITE);
    gaddr record = CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(VIEW_RECORD);
    uint8_t type = rd_u8(record + 0x62) & 0xF0;
    int queued = !taken && !(raw & 0x80) && (int8_t)count < 10;

    SET_B(D(7), 0);
    A(0) = type == 0x30 ? record : 0xC1BAD4u;
    SET_B(D(4), type);
    if (type != 0x30) D(4) = 0; /* request_cockpit_redraw leaves D4=3, then MOVE.B mode 0 */
    if (queued) {
        if (slot >= 10) D(4) = 0;
        else SET_B(D(4), (uint8_t)slot);
        SET_W(D(4), (uint16_t)(int16_t)(int8_t)D(4));
        A(3) = KEY_RAW;
        SET_W(D(0), (uint16_t)D(0) & 0xFFu);
        A(3) = 0xC331CEu;
        SET_B(D(0), rd_u8(A(3) + (gaddr)(uint16_t)D(0)));
        SET_B(D(4), (uint8_t)(D(4) + 1));
        A(3) = KEY_TRANSLATED;
        SET_B(D(4), (uint8_t)dst);
        SET_W(D(4), (uint16_t)(int16_t)(int8_t)D(4));
    }
}

int glue_C1B906(void) {
    uint8_t raw = (uint8_t)D(0);
    /* The replay must see the queue state before the C updates it. */
    view_mode_zero_registers();
    start_view_mode_zero(raw);
    return glue_return();
}
