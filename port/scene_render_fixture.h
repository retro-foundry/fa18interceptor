#ifndef FA18_SCENE_RENDER_FIXTURE_H
#define FA18_SCENE_RENDER_FIXTURE_H

#include <stdint.h>

#include "flight_renderer_packet.h"
#include "flight_renderer_page.h"
#include "five_plane_page.h"
#include "hunk.h"
#include "projection_grid.h"
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

/* User-authorized, opt-in `$C279D0` producer diagnostic. It imports only the
 * mutable pre-call page and Slow-RAM input state from caller-supplied files;
 * Hunk-25 traversal, packet setup, direct pixel operations, and bounded line
 * submission execute through the native port. It is not a normal game path. */
typedef struct {
    FA18FivePlanePage page;
    FA18FlightRendererPage renderer;
    FA18ProjectionGridPacketState packet_state;
    FA18ProjectionGridPacketRoute packet_route;
    uint16_t submitted_record_count;
    uint8_t enabled;
} FA18C279RenderFixture;

int fa18_initialize_c279_render_fixture(FA18C279RenderFixture *fixture,
                                        const FA18Hunks *exe,
                                        const FA18ProjectionGrid *grid,
                                        const char *slow_capture_path,
                                        const char *chip_capture_path,
                                        FA18Video *video);
int fa18_present_c279_render_fixture(const FA18C279RenderFixture *fixture,
                                     FA18Video *video);

#endif
