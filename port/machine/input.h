#ifndef FA18_MACHINE_INPUT_H
#define FA18_MACHINE_INPUT_H

#include "machine.h"

/* Amiga raw key code for an SDL2 keycode (SDLK_* value), or -1. */
int fa18_amiga_rawkey(int sdl_keycode);
/* Amiga raw key code for a libretro RETROK_* code, or -1. Engine9000
 * E9K_INPUT_V1 recordings store these (SDL 1.2 numbering: F1 = 282). */
int fa18_amiga_rawkey_retro(int retrok);

/* An E9K_INPUT_V1 recording: per-frame keyboard, mouse and button events,
 * numbered from the run's restore frame. */
typedef struct {
    int frame;
    char kind;       /* 'K' key, 'm' mouse motion, 'b' mouse button, 'J' joypad, 'C' clear */
    int a, b, c, d;  /* K: key char mods down; m: port dx dy; b: port button down;
                        J: port id down (libretro pad ids: 0 fire, 4-7 up down left right) */
} FA18ReplayEvent;

typedef struct {
    FA18ReplayEvent *events;
    int count, next;
    int pad[16];     /* held joypad buttons, port 0 */
} FA18Replay;

int fa18_replay_load(FA18Replay *replay, const char *path);
void fa18_replay_free(FA18Replay *replay);
/* Apply every event recorded for `frame` (skipping earlier ones). */
void fa18_replay_apply(FA18Replay *replay, FA18Machine *m, int frame);

#endif
