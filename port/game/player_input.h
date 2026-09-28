#ifndef FA18_GAME_PLAYER_INPUT_H
#define FA18_GAME_PLAYER_INPUT_H

enum { MOUSE_LEFT = 1, MOUSE_RIGHT = 2 };

/* Mouse buttons currently held (MOUSE_LEFT | MOUSE_RIGHT). */
int read_mouse_buttons(void);

/* Joystick directions ($C16F1C), as latched into STICK_Y / STICK_X. */
enum { STICK_UP = 0x10, STICK_DOWN = 0x20, STICK_LEFT = 0x08, STICK_RIGHT = 0x04 };

/* Read the joystick in port 2 and latch its directions. */
void read_joystick(void);

#endif
