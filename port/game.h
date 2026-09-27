#ifndef FA18_GAME_H
#define FA18_GAME_H

#include <stdint.h>

#include "disk.h"
#include "hunk.h"
#include "menu_text.h"
#include "replay.h"
#include "video.h"

/* Everything the running game owns. Fields are added as routines are ported. */
typedef struct {
    FA18Disk disk;
    FA18Hunks exe;
    FA18Video video;
    FA18MenuTextState menu_text;
    uint32_t frame; /* PAL video frame number, matching the recorded run */
} FA18Game;

/* Load the game from the disk and bring it to the state of run075 frame 200. */
int fa18_game_init(FA18Game *game, const char *adf_path);
void fa18_game_free(FA18Game *game);

/* Advance one PAL video frame with the given control state. */
void fa18_game_frame(FA18Game *game, const FA18ReplayControlState *controls);

#endif
