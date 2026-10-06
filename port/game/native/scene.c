/* Connected C0EFD4 scene slice: matrix/projection, horizon and map packets.
 * Object descriptors and cockpit/HUD submissions are still pending. */
#include "scene.h"
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
}
