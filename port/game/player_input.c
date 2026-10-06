/* Player input. */
#include "player_input.h"
#include "command_publication.h"

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
