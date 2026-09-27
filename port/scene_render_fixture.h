#ifndef FA18_SCENE_RENDER_FIXTURE_H
#define FA18_SCENE_RENDER_FIXTURE_H

#include <stdint.h>

#include "five_plane_page.h"
#include "hunk.h"
#include "video.h"

/* User-authorized, opt-in display diagnostic. It takes a run075 frame-392
 * Chip-RAM capture at runtime; no captured page data is compiled into the
 * port, and this is never a normal game path. */
typedef struct {
    FA18FivePlanePage page;
    uint8_t enabled;
} FA18SceneRenderFixture;

int fa18_initialize_scene_render_fixture(FA18SceneRenderFixture *fixture,
                                         const FA18Hunks *exe,
                                         const char *chip_capture_path,
                                         FA18Video *video);
int fa18_present_scene_render_fixture(const FA18SceneRenderFixture *fixture,
                                      FA18Video *video);

#endif
