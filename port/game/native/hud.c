/* Source C0F138-C0F29E gates and cadence for HUD and panel owners.
 * Stores and complete frame ownership remain unconnected. */
#include "hud.h"
#include "../globals.h"
#include "../postflight_hud.h"
#include "../hud_readouts.h"
#include "../view_marks.h"
#include "../hud_bars.h"
#include "../postflight_variants.h"
#include "../message_line.h"

void native_hud_draw(uint16_t saved_tick) {
    wr_u16(UPDATE_STAGE_MARKER,0xd0);
    if(rd_u8(ORIGIN_ENABLE) && !rd_u8(UPDATE_HUD_MODE)) {
        wr_u8(CONTEXT_READOUTS,1);
        draw_speed_readout();draw_altitude_readout();draw_heading_readout();
        if(rd_u8(MODE_SELECT)==2) draw_message_line();
        return;
    }
    const gaddr record=CONTROL_RECORDS+((uint32_t)(int32_t)rd_s16(TARGET_RECORD)<<9);
    if(rd_u8(record+0x62)==0x30 && !rd_u8(UPDATE_HUD_MODE)) return;
    draw_panel_frame();draw_panel_image();
    draw_postflight_renderer_dispatch();
    draw_postflight_hud();
    draw_threat_lights();
    draw_compass_tape();
    if(rd_s8(UPDATE_ACTIVITY)>0 || (saved_tick&3u)>1) {
        draw_heading_readout();draw_speed_readout();
    }
    if(rd_s8(UPDATE_ACTIVITY)>0 || (saved_tick&3u)<2) draw_record_2b_readout();
    if(rd_s8(UPDATE_ACTIVITY)>0 || (saved_tick&3u)<2 || rd_s32(POSITION_BIAS)>-0x8000)
        draw_altitude_readout();
    if(rd_s8(UPDATE_ACTIVITY)>0 || (saved_tick&7u)<2) draw_record_72_readout();
    if(rd_s8(UPDATE_ACTIVITY)>0 || (saved_tick&15u)<2) draw_gauge_bar();
    wr_u16(UPDATE_STAGE_MARKER,0x1a0);
    draw_panel_mark();
    draw_weapon_status();
    draw_grid_z_readout();draw_grid_x_readout();draw_zoom_readout();
    draw_mode_bar();
    draw_scale_readout();
    draw_message_line();
    draw_indicator_bars();
    if(rd_s8(UPDATE_ACTIVITY)>=0) wr_u8(UPDATE_ACTIVITY,(uint8_t)(rd_u8(UPDATE_ACTIVITY)-1));
    wr_u16(UPDATE_STAGE_MARKER,0x1d0);
}
