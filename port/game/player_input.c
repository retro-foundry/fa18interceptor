/* Player input. */
#include "player_input.h"

#include "cockpit.h"

#include "globals.h"
#include "hardware.h"
#include "memory.h"

int read_mouse_buttons(void) {
    int buttons = 0;
    if (!(rd_u8(CIAA_PORT_COPY) & 0x40)) buttons |= MOUSE_LEFT;   /* /FIR0 low */
    if (!(custom_read(POTINP) & 0x0400)) buttons |= MOUSE_RIGHT;  /* DATLY low */
    return buttons;
}

/* Latch one axis; while paused (or a context is starting) it is also
 * mirrored into the player record under `mask`. */
static void latch_axis(gaddr axis, uint8_t direction, uint8_t mask) {
    wr_u8(axis, direction);
    if (rd_u8(PAUSE_A) || rd_u8(CONTEXT_STARTED))
        wr_u8(PLAYER_STICK, (uint8_t)((rd_u8(PLAYER_STICK) & ~mask) | direction));
}

void read_joystick(void) {
    uint16_t joy = custom_read(JOY1DAT);
    int up = ((joy >> 9) ^ (joy >> 8)) & 1, down = ((joy >> 1) ^ joy) & 1;

    wr_u16(STICK_RAW, joy);
    if (up) {
        latch_axis(STICK_Y, STICK_UP, 0x30);
        wr_u8(STICK_Y_HELD, 1);
    } else if (down) {
        latch_axis(STICK_Y, STICK_DOWN, 0x30);
        wr_u8(STICK_Y_HELD, 1);
    } else if (rd_u8(STICK_Y_HELD)) {
        latch_axis(STICK_Y, 0, 0x30);
        wr_u8(STICK_Y_HELD, 0);
    }
    if (joy & 0x0200) {
        latch_axis(STICK_X, STICK_LEFT, 0x0C);
        wr_u8(STICK_X_HELD, 1);
    } else if (joy & 0x0002) {
        latch_axis(STICK_X, STICK_RIGHT, 0x0C);
        wr_u8(STICK_X_HELD, 1);
    } else if (rd_u8(STICK_X_HELD)) {
        latch_axis(STICK_X, 0, 0x0C);
        wr_u8(STICK_X_HELD, 0);
    }
}

static uint16_t last_row_for_mode(int8_t mode) {
    if (mode < 3 || mode == 10 || mode == 11) return 0x90;
    if (mode >= 5 && mode <= 7) return 0xB3;
    return 0xA7;
}

void queue_view_key(uint8_t raw) {
    int8_t slot;
    uint8_t translated;

    request_cockpit_redraw();
    wr_u16(LINE_LAST_ROW, last_row_for_mode((int8_t)rd_u8(VIEW_MODE)));
    if (!rd_u8(KEY_TAKEN) && !(raw & 0x80)) {
        wr_u8(KEY_TAKEN, 1);
        if ((int8_t)rd_u8(KEY_COUNT) < 10) {
            slot = (int8_t)rd_u8(KEY_WRITE);
            if (slot >= 10) slot = 0;
            wr_u8(KEY_RAW + (gaddr)(int32_t)slot, raw);
            translated = rd_u8(KEY_TABLE + raw);
            wr_u8(KEY_WRITE, (uint8_t)(slot + 1));
            wr_u8(KEY_COUNT, (uint8_t)(rd_u8(KEY_COUNT) + 1));
            wr_u8(KEY_TRANSLATED + (gaddr)(int32_t)(int8_t)rd_u8(KEY_TRANSLATED_WRITE), translated);
        }
    }
    wr_u8(KEY_STATE, 0);
    wr_u8(KEY_STATE + 1, 0);
    wr_u8(KEY_STATE + 2, 0);
}

void drop_lost_selection(void) {
    gaddr record;
    if (!rd_u16(TARGET_RECORD)) return;
    record = CONTROL_RECORDS + (gaddr)((uint32_t)(int32_t)rd_s16(TARGET_RECORD) << 9);
    if (rd_u16(record) & 0x40) return;
    wr_u16(TARGET_RECORD, 0);
    wr_u16(VIEW_RECORD, 0);
    wr_u8(UPDATE_MASK, 0xFF);
    if (rd_u8(CONTEXT_SELECT)) return;
    wr_u8(VIEW_MODE, 0);
    wr_u16(SPAN_ORIGIN, 0);
    wr_u16(SPAN_ORIGIN_Y, 0);
    queue_view_key(0);
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
