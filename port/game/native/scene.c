/* Connected C0EFD4 scene slice: view, terrain, scenery and aircraft records.
 * Cockpit/HUD submissions are still pending. */
#include "scene.h"
#include "model.h"
#include "../scene_placements.h"
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
void native_scene_draw(NativeFrontend *game) {
    const gaddr frame=0x4000; /* Host scratch, separate from text and recorder. */
    step_cockpit_slide();
    reset_list();
    if(rd_u16(UPDATE_DISPLAY_FLAGS)&0x2000u) return;
    submit_active_planes(NULL);
    ++game->scene_frames;
    if(!rd_u8(UPDATE_MAP_FLAGS) || rd_u8(UPDATE_MAP_OVERRIDE)) {
        FA18MapPacketDepthStageResult depth=prepare_map_packet_depth(frame);
        const MapPacketHooks hooks={.context=game,.draw_polygon=polygon};
        for(int wide=0;wide<2;++wide) {
            if(!wide && !depth.run_normal_pass) continue;
            wr_u16(CURRENT_COLOUR,6);
            if(run_map_packet_pass(frame,wide,&hooks)) {
                fprintf(stderr,"native map packet pass failed: wide=%d\n",wide); abort();
            }
        }
    }
    draw_fixed_matrix_mark();
    if(rd_s32(POSITION_BIAS)>-0x08000000) {
        const ScenePlacementHooks placements={native_scene_placement,NULL,game};
        visit_scene_placements(0,&placements);
        visit_scene_placements(1,&placements);
        const GridProjectionHooks grid={.triangle=grid_triangle,.pixel=grid_pixel};
        draw_grid_projection_packet(0x4400,&grid);
    }
    const MainControlHooks controls={control_child,NULL,game,NULL};
    const FollowupPlacementHooks followups={followup,NULL,game};
    int controls_first=rd_u8(ORIGIN_ENABLE) || flagged_slot_in_range();
    if(controls_first) advance_main_loop_control_records(0x4500,&controls);
    visit_followup_placements(&followups);
    if(!controls_first) advance_main_loop_control_records(0x4500,&controls);
}
