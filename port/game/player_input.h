#ifndef FA18_GAME_PLAYER_INPUT_H
#define FA18_GAME_PLAYER_INPUT_H

#include <stdint.h>

enum { MOUSE_LEFT = 1, MOUSE_RIGHT = 2 };

/* Mouse buttons currently held (MOUSE_LEFT | MOUSE_RIGHT). */
int read_mouse_buttons(void);

/* Joystick directions ($C16F1C), as latched into STICK_Y / STICK_X. */
enum { STICK_UP = 0x10, STICK_DOWN = 0x20, STICK_LEFT = 0x08, STICK_RIGHT = 0x04 };

/* Read the joystick in port 2 and latch its directions. */
void read_joystick(void);

/* $C1BA86: request a cockpit redraw and set LINE_LAST_ROW for VIEW_MODE
 * (modes 5-7: $B3; 3-4 and 8-9, 12 up: $A7; others $90); then queue key
 * `raw` (when none was taken this update and it is a press) in KEY_RAW and,
 * translated through KEY_TABLE, in KEY_TRANSLATED, up to ten keys. */
void queue_view_key(uint8_t raw);

#endif
