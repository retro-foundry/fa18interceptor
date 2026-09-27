#ifndef FA18_GAME_H
#define FA18_GAME_H

#include <stdint.h>

#include "disk.h"
#include "hunk.h"
#include "menu_text.h"
#include "menu_flow.h"
#include "menu_record.h"
#include "menu_render.h"
#include "projection_grid.h"
#include "scene_record_table.h"
#include "scene_component_magnitude.h"
#include "scene_dispatch_table.h"
#include "replay.h"
#include "video.h"

/* Everything the running game owns. Fields are added as routines are ported. */
typedef struct {
    FA18Disk disk;
    FA18Hunks exe;
    FA18ProjectionGrid projection_grid;
    FA18SceneRecordTable scene_record_table;
    FA18LoadedSceneMagnitudeTable scene_magnitude_table;
    FA18SceneDispatchTable scene_dispatch_table;
    FA18Video video;
    FA18MenuTextState menu_text;
    FA18MenuFlow menu_flow;
    FA18MenuRecord menu_records[FA18_MENU_TEXT_SELECTORS];
    uint32_t frame; /* PAL video frame number, matching the recorded run */
} FA18Game;

/* Load the game from the disk and bring it to the state of run075 frame 200. */
int fa18_game_init(FA18Game *game, const char *adf_path);
void fa18_game_free(FA18Game *game);

/* Apply replay controls at the presentation boundary for the current frame. */
int fa18_game_apply_controls(FA18Game *game, const FA18ReplayControlState *controls);

/* Advance one PAL video frame with the given control state. */
int fa18_game_frame(FA18Game *game, const FA18ReplayControlState *controls,
                    uint16_t post_input_ticks);

#endif
