#ifndef FA18_FLIGHT_RENDERER_PAGE_H
#define FA18_FLIGHT_RENDERER_PAGE_H

#include <stdint.h>

#include "five_plane_page.h"
#include "line.h"
#include "projection_grid.h"

/* Native page owner for the source-proved direct-pixel and direct-line
 * children of the `$C279D0` grid pass. Far area-blit children intentionally
 * remain absent until their complete page semantics are reconstructed. */
typedef struct {
    FA18PlanarLanePage lanes;
    FA18PlanarPixelState pixel_state;
    FA18LineStyle line_style;
    FA18PlanarPixelPageRendererContext direct_pixels;
    FA18PlanarLinePageContext lines;
    FA18ProjectionPairSubmission triangle_submission;
    FA18ProjectionGridSubmission grid_submission;
    uint16_t last_dma_value;
    uint16_t dma_call_count;
} FA18FlightRendererPage;

/* Binds a five-plane page's lower lanes to `$C2F5F4/$C2F60A` and the proved
 * `$C2FF48 -> $C301F6 -> $C2FA7E` branch. All scalar triangle inputs come
 * from the surrounding live renderer state. */
int fa18_flight_renderer_page_init(
    FA18FlightRendererPage *renderer, FA18FivePlanePage *page,
    const FA18PlanarPixelState *pixel_state, const FA18LineStyle *line_style,
    int16_t display_bound_y, int16_t vertical_value, int16_t horizontal_value,
    uint32_t renderer_base_long, uint8_t mode_flag, uint32_t saved_line_scratch);

/* `$C279D4-$C279F2` writes `$C456E6-$C456ED` and `$C45954` before the
 * Hunk-25 traversal. Bind those source-owned fields to this page's direct
 * pixel and line callbacks; the input packet/scheduler remain caller-owned. */
int fa18_flight_renderer_page_apply_projection_grid_packet_state(
    FA18FlightRendererPage *renderer,
    const FA18ProjectionGridPacketState *packet_state);

const FA18ProjectionGridSubmission *fa18_flight_renderer_page_submission(
    const FA18FlightRendererPage *renderer);

#endif
