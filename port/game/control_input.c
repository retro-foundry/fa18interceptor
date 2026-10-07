/* Source player control state and command publication; no device I/O. */
#include "player_input.h"
#include "command_publication.h"
#include "cockpit.h"
#include "globals.h"

static uint16_t last_row_for_mode(int8_t mode) {
    if (mode < 3 || mode == 10 || mode == 11) return 0x90;
    if (mode >= 5 && mode <= 7) return 0xB3;
    return 0xA7;
}

void queue_view_key(uint8_t raw) {
    request_cockpit_redraw();
    wr_u16(LINE_LAST_ROW, last_row_for_mode((int8_t)rd_u8(VIEW_MODE)));
    publish_command_event(raw,NULL);
}

int drop_lost_selection(void) {
    gaddr record;
    if (!rd_u16(TARGET_RECORD)) return 0;
    record = CONTROL_RECORDS + (gaddr)((uint32_t)(int32_t)rd_s16(TARGET_RECORD) << 9);
    if (rd_u16(record) & 0x40) return 0;
    wr_u16(TARGET_RECORD, 0);
    wr_u16(VIEW_RECORD, 0);
    wr_u8(UPDATE_MASK, 0xFF);
    if (rd_u8(CONTEXT_SELECT)) return 0;
    wr_u8(VIEW_MODE, 0);
    wr_u16(SPAN_ORIGIN, 0);
    wr_u16(SPAN_ORIGIN_Y, 0);
    queue_view_key(0);
    return 1;
}

/* PLAYER_STICK fields: bits 0-1 throttle, 2-3 stick X, 4-5 stick Y. */
static void set_stick_bits(uint8_t keep, uint8_t value) {
    wr_u8(PLAYER_STICK, (uint8_t)((rd_u8(PLAYER_STICK) & keep) | value));
}

void set_throttle_input(uint8_t value) { set_stick_bits(0xFC, value); }

void release_throttle_keys(void) {
    wr_u8(FUNCTION_KEY_LEVEL, 0);
    set_throttle_input(THROTTLE_HOLD);
}

void set_stick_y(uint8_t value) {
    wr_u8(STICK_Y, value);
    if (rd_u8(PAUSE_A) || rd_u8(CONTEXT_STARTED)) set_stick_bits(0xCF, value);
}

void set_stick_x(uint8_t value) {
    wr_u8(STICK_X, value);
    if (rd_u8(PAUSE_A) || rd_u8(CONTEXT_STARTED)) set_stick_bits(0xF3, value);
}

void check_typed_code(void) {
    gaddr expected = EXPECTED_CODE, typed = KEY_TRANSLATED;
    int16_t left = (int16_t)(rd_s16(EXPECTED_LENGTH) + 2);
    do {
        int8_t want = (int8_t)rd_u8(expected++);
        if (want <= 0) break;
        if ((uint8_t)want != rd_u8(typed++)) {
            wr_u8(CONTEXT_REQUEST, 0xFF);
            return;
        }
    } while (left-- != 0);
    wr_u8(CONTEXT_REQUEST, 1);
}
