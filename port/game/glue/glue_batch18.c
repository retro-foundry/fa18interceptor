/* Glue for read_joystick ($C16F1C). */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "memory.h"
#include "player_input.h"

/* Compiled C. Leaves A0 = &JOY1DAT, D0 = the up/down test ($100 up, 1 down,
 * else 0), D1's high word clear (ANDI.L), and D2 = the last direction
 * latched by a helper (MOVEQ). */
int glue_C16F1C(void) {
    uint8_t y_held = rd_u8(STICK_Y_HELD), x_held = rd_u8(STICK_X_HELD);
    uint16_t joy;
    int up, down;
    uint32_t d2 = D(2);

    read_joystick();

    joy = rd_u16(STICK_RAW);
    up = ((joy >> 9) ^ (joy >> 8)) & 1;
    down = ((joy >> 1) ^ joy) & 1;
    if (up) d2 = STICK_UP;
    else if (down) d2 = STICK_DOWN;
    else if (y_held) d2 = 0;
    if (joy & 0x0200) d2 = STICK_LEFT;
    else if (joy & 0x0002) d2 = STICK_RIGHT;
    else if (x_held) d2 = 0;
    D(0) = up ? 0x100 : down ? 1 : 0;
    D(1) = up ? (joy & 0x100u) : (joy & 1u); /* ANDI.L results */
    D(2) = d2;
    A(0) = 0xDFF00C;
    return glue_return();
}
