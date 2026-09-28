#ifndef FA18_GAME_PLAYER_INPUT_H
#define FA18_GAME_PLAYER_INPUT_H

enum { MOUSE_LEFT = 1, MOUSE_RIGHT = 2 };

/* Mouse buttons currently held (MOUSE_LEFT | MOUSE_RIGHT). */
int read_mouse_buttons(void);

#endif
