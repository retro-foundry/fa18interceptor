/* Source C0F138-C0F29E gates and cadence for HUD and panel owners.
 * Aircraft stores use the source normal-view contract in stores.c. */
#include "hud.h"
#include "stores.h"
#include "../globals.h"
#include "../postflight_hud.h"
#include "../hud_readouts.h"
#include "../view_marks.h"
#include "../hud_bars.h"
#include "../postflight_variants.h"
#include "../message_line.h"

static NativeInputReturn text_return(NativeInputReturn prior,TextDrawResult text) {
    if(text.kind==TEXT_DRAW_NONE) return prior;
    if(text.kind==TEXT_DRAW_CHARACTER)
        return (NativeInputReturn){text.character,NATIVE_INPUT_RETURN_HUD_TEXT};
    if(text.kind==TEXT_DRAW_GLYPH)
        return (NativeInputReturn){(uint8_t)text.glyph,NATIVE_INPUT_RETURN_HUD_TEXT};
    return (NativeInputReturn){0};
}
static NativeInputReturn bar_return(NativeInputReturn prior,BarDrawResult bar) {
    if(bar.kind==BAR_DRAW_NONE) return prior;
    if(bar.kind==BAR_DRAW_DESTINATION)
        return (NativeInputReturn){(uint8_t)bar.destination,NATIVE_INPUT_RETURN_HUD_BAR};
    if(bar.kind==BAR_DRAW_MARKER_LINE && bar.line.kind==LINE_DRAW_SIZE)
        return (NativeInputReturn){(uint8_t)bar.line.size,NATIVE_INPUT_RETURN_HUD_LINE};
    if(bar.kind==BAR_DRAW_MARKER_LINE && bar.line.kind==LINE_DRAW_X_DELTA)
        return (NativeInputReturn){(uint8_t)bar.line.x_delta,NATIVE_INPUT_RETURN_HUD_LINE};
    return (NativeInputReturn){0};
}
NativeInputReturn native_hud_draw(uint16_t saved_tick) {
    NativeInputReturn result={0};
    wr_u16(UPDATE_STAGE_MARKER,0xd0);
    if(rd_u8(ORIGIN_ENABLE) && !rd_u8(UPDATE_HUD_MODE)) {
        wr_u8(CONTEXT_READOUTS,1);
        result=text_return(result,draw_speed_readout());
        result=text_return(result,draw_altitude_readout());
        result=text_return(result,draw_heading_readout());
        if(rd_u8(MODE_SELECT)==2) result=text_return(result,draw_message_line());
        return result;
    }
    const gaddr record=CONTROL_RECORDS+((uint32_t)(int32_t)rd_s16(TARGET_RECORD)<<9);
    if(rd_u8(record+0x62)==0x30 && !rd_u8(UPDATE_HUD_MODE)) return result;
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
    native_hud_draw_stores(); /* C30A00 between weapon status and grid readouts. */
    draw_grid_z_readout();draw_grid_x_readout();draw_zoom_readout();
    result=bar_return(result,draw_mode_bar());
    result=text_return(result,draw_scale_readout());
    result=text_return(result,draw_message_line());
    result=bar_return(result,draw_indicator_bars());
    if(rd_s8(UPDATE_ACTIVITY)>=0) wr_u8(UPDATE_ACTIVITY,(uint8_t)(rd_u8(UPDATE_ACTIVITY)-1));
    wr_u16(UPDATE_STAGE_MARKER,0x1d0);
    return result;
}
