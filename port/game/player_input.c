/* Player input. */
#include "player_input.h"

#include "globals.h"
#include "hardware.h"
#include "memory.h"

int read_mouse_buttons(void) {
    int buttons = 0;
    if (!(rd_u8(CIAA_PORT_COPY) & 0x40)) buttons |= MOUSE_LEFT;   /* /FIR0 low */
    if (!(custom_read(POTINP) & 0x0400)) buttons |= MOUSE_RIGHT;  /* DATLY low */
    return buttons;
}
