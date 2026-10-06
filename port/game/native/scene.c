/* Connected C0EFD4 scene slice: view, terrain, scenery and aircraft records.
 * The HUD/panel slice follows in native/hud.c. */
#include "scene.h"
#include "model.h"
#include "../scene_placements.h"
#include "../update_sequence.h"
#include "../globals.h"
#include "../matrix_route.h"
#include "../fixed_math.h"
#include "../view.h"
#include "../attitude.h"
#include "../cockpit.h"
#include "../stages.h"
#include "../active_planes.h"
#include "../map_packet.h"
#include "../polygon_clip.h"
#include "../projection.h"
#include "../followup_placements.h"
#include "../main_loop_control_messages.h"
#include "../grid_projection_packet.h"
#include "../render_line.h"
#include "../render_polygon.h"
#include "../plot.h"
#include <stdio.h>
#include <stdlib.h>

void native_scene_project(void) {
    dispatch_matrix_route(NULL,NULL);
    seed_projection();
    update_view_octant();
    update_attitude_flags();
}
static int polygon(void *context,gaddr end,int16_t origin) {
    NativeFrontend *game=context;
    (void)end; (void)origin;
    if(clip_and_draw_polygon()) ++game->terrain_polygons;
    return 0; /* Packet walker expects submission success, including rejection. */
}
static int32_t followup(void *context,const FollowupPlacementEvent *call) {
    ScenePlacementCall descriptor={.routine=call->routine,.parameters=call->parameters,.header=call->header};
    return native_scene_placement(context,&descriptor);
}
static MessageWorking control_child(void *context,enum MainControlChild child) {
    (void)context;
    if(child==MC_RESET_FACE_STATE) {reset_line_style();return (MessageWorking){0};}
    fprintf(stderr,"native scene control child unavailable: %u\n",(unsigned)child);abort();
}
static void grid_triangle(void *context) {(void)context;draw_polygon();}
static void grid_pixel(void *context,int16_t x,int16_t y,int adjacent) {
    (void)context;if(adjacent) plot_pixel_pair(x,y);else plot_pixel(x,y);
}
static UpdateSequenceResult scene_child(void *context,enum UpdateSequenceChild child) {
    NativeFrontend *game=context;
    const gaddr frame=0x4000; /* Host scratch, separate from text and recorder. */
    const ScenePlacementHooks placements={native_scene_placement,NULL,game};
    const MainControlHooks controls={control_child,NULL,game,NULL};
    const FollowupPlacementHooks followups={followup,NULL,game};
    UpdateSequenceResult result={0,0};
    switch(child) {
    case UPDATE_COCKPIT_SLIDE: step_cockpit_slide(); break;
    case UPDATE_LIST_RESET: reset_list(); break;
    case UPDATE_BUFFERS: {
        const UpdateSequenceHooks hooks={scene_child,NULL,game};
        submit_update_display_buffers(&hooks); break;
    }
    case UPDATE_DISPLAY_PLANES:
        submit_active_planes(NULL); ++game->scene_frames; break;
    case UPDATE_MAP: {
        FA18MapPacketDepthStageResult depth=prepare_map_packet_depth(frame);
        const MapPacketHooks hooks={.context=game,.draw_polygon=polygon};
        for(int wide=0;wide<2;++wide) {
            if(!wide && !depth.run_normal_pass) continue;
            wr_u16(CURRENT_COLOUR,6);
            if(run_map_packet_pass(frame,wide,&hooks)) {
                fprintf(stderr,"native map packet pass failed: wide=%d tick=%u height=%d metric=%d octant=%d table=%d\n",
                        wide,game->ticks,rd_s32(CONTROL_RECORDS+24),
                        rd_s32(frame-0x28),rd_s8(VIEW_OCTANT),rd_s8(0xc45850)); abort();
            }
        }
        break;
    }
    case UPDATE_MATRIX_MARK: draw_fixed_matrix_mark(); break;
    case UPDATE_PRIMARY_SCENE: visit_scene_placements(0,&placements); break;
    case UPDATE_ALTERNATE_SCENE: visit_scene_placements(1,&placements); break;
    case UPDATE_GRID: {
        const GridProjectionHooks grid={.triangle=grid_triangle,.pixel=grid_pixel};
        draw_grid_projection_packet(0x4400,&grid); break;
    }
    case UPDATE_RANGE_DECISION: result.value=flagged_slot_in_range(); break;
    case UPDATE_FLAGGED_SCENE: case UPDATE_TRUE_SCENE: case UPDATE_FALSE_SCENE:
        advance_main_loop_control_records(0x4500,&controls); break;
    case UPDATE_TRUE_FOLLOWUP: case UPDATE_FALSE_FOLLOWUP:
        visit_followup_placements(&followups); break;
    /* C0DA38 builds a full-viewport selection and exits the source frame.
     * TODO(port): connect that selection and propagate its early owner exit. */
    case UPDATE_DISPLAY_END:
    default: fprintf(stderr,"native scene child unavailable: %u\n",(unsigned)child); abort();
    }
    return result;
}
int native_scene_draw(NativeFrontend *game) {
    const UpdateSequenceHooks hooks={scene_child,NULL,game};
    return run_game_scene_sequence(&hooks);
}
