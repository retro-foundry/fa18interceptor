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

/* $C12242: when the selected record has lost its bit 6 (+$00), drop the
 * selection and the view record, request a full update and, outside a
 * context, reset the view mode and span origins and queue_view_key(0)
 * (TARGET_RECORD holds the selection). */
void drop_lost_selection(void);

/* The throttle keys' field of PLAYER_STICK ($C1B4D0 up, $C1B4D4 down,
 * $C1B4DE hold). */
enum { THROTTLE_HOLD = 0, THROTTLE_UP = 1, THROTTLE_DOWN = 2 };
void set_throttle_input(uint8_t value);
/* Hold, clearing FUNCTION_KEY_LEVEL first ($C1B4D8). */
void release_throttle_keys(void);

/* The stick's Y ($10 up, $20 down, 0) and X ($04 right, $08 left, 0)
 * directions: into STICK_Y/STICK_X, and into PLAYER_STICK while paused or
 * in a context run ($C1B50C-$C1B514, $C1B558-$C1B560). */
void set_stick_y(uint8_t value);
void set_stick_x(uint8_t value);

#endif
