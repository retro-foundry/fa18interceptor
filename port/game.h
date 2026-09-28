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
#include "scene_entry_runtime.h"
#include "scene_renderer_defaults.h"
#include "post_input_followup.h"
#include "scene_render_fixture.h"
#include "viewport_palette.h"
#include "default_scene_render_pass.h"
#include "renderer_page_setup.h"
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
    FA18SceneEntryRuntime scene_entry_runtime;
    FA18SceneRendererDefaults scene_renderer_defaults;
    /* `$C15DB4-$C1601E` has already allocated this pending render page before
     * the restored run075 state at frame 200. Its renderer/presentation
     * scheduler remains separately owned. */
    FA18RendererPageSetup renderer_page_setup;
    FA18SceneInitializationState scene_initialization;
    FA18PostInputFollowupState post_input_followup;
    FA18ViewportModeState viewport_mode;
    /* Native mutable counterpart of the callback-owned COLOR00--15 stream.
     * The outer-view pointer tables are intentionally unbound until the real
     * two-page owner is composed. */
    uint8_t viewport_copper_bytes[FA18_VIEWPORT_PALETTE_WORDS * 4u + 4u];
    FA18CopperMutableInstructionStream viewport_copper_stream;
    FA18ViewportPaletteBuffer viewport_palette_buffer;
    uint32_t viewport_left_pointer_table[2];
    uint32_t viewport_right_pointer_table[2];
    uint8_t scene_entry_armed;
    uint8_t scene_entry_complete;
    FA18MenuRecord menu_records[FA18_MENU_TEXT_SELECTORS];
    FA18SceneRenderFixture render_fixture;
    FA18C279RenderFixture c279_render_fixture;
    uint32_t frame; /* PAL video frame number, matching the recorded run */
} FA18Game;

/* Load the game from the disk and bring it to the state of run075 frame 200. */
int fa18_game_init(FA18Game *game, const char *adf_path);
void fa18_game_free(FA18Game *game);

/* Apply replay controls at the presentation boundary for the current frame. */
int fa18_game_apply_controls(FA18Game *game, const FA18ReplayControlState *controls);

/* User-authorized temporary captured-state visual bootstrap. It is opt-in and
 * does not alter the normal replay path. */
int fa18_game_enable_render_fixture(FA18Game *game, const char *chip_capture_path);

/* User-authorized temporary `$C279D0` producer diagnostic. It is opt-in and
 * does not alter the normal replay path. */
int fa18_game_enable_c279_render_fixture(FA18Game *game, const char *slow_capture_path,
                                         const char *chip_capture_path);

/* User-authorized, capture-free renderer diagnostic. It renders the current
 * scene-entry root record through the default `$C2DB18 -> $C1C54E -> $C279D0`
 * path into a fresh native page and presents it. It is deliberately separate
 * from normal scheduling until the parent/outer-loop owner is complete. */
int fa18_game_present_active_scene_render_diagnostic(FA18Game *game);

/* Advance one PAL video frame with the given control state. `post_input_ticks`
 * comes from a source-measured scheduler stream; it is never inferred from
 * presentation frames. */
int fa18_game_frame(FA18Game *game, const FA18ReplayControlState *controls,
                    uint16_t post_input_ticks);

#endif
