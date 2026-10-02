#include "update_sequence.h"
#include "globals.h"
#include <stdlib.h>

static void observe(const UpdateSequenceHooks *h,enum UpdateSequencePhase phase,
                    uint32_t value,uint32_t mask,uint32_t limit,gaddr record) {
    UpdateSequenceEvent event={phase,value,mask,limit,record};
    if(h->observe) h->observe(h->context,&event);
}
static void marker(const UpdateSequenceHooks *h,uint16_t value) {
    wr_u16(UPDATE_STAGE_MARKER,value);
    observe(h,UPDATE_SEQUENCE_MARKER,value,0,0,0);
}
static uint8_t byte(const UpdateSequenceHooks *h,gaddr address,int load) {
    uint8_t value=rd_u8(address);
    observe(h,load?UPDATE_SEQUENCE_BYTE_LOAD:UPDATE_SEQUENCE_BYTE_TEST,value,0,0,0);
    return value;
}
static int32_t signed_value(const UpdateSequenceHooks *h,int32_t limit) {
    uint32_t value=rd_u32(POSITION_BIAS);
    observe(h,UPDATE_SEQUENCE_LONG_COMPARE,value,0,(uint32_t)limit,0);
    return (int32_t)value;
}
static uint16_t tick_key(const UpdateSequenceHooks *h,uint16_t tick,
                         uint16_t mask,uint16_t limit,int subtract) {
    observe(h,subtract?UPDATE_SEQUENCE_TICK_SUBTRACT:UPDATE_SEQUENCE_TICK_COMPARE,
            tick,mask,limit,0);
    return tick&mask;
}
static int activity(const UpdateSequenceHooks *h,uint16_t tick,uint16_t mask,
                    uint16_t limit,int greater) {
    uint16_t key;
    if((int8_t)byte(h,UPDATE_ACTIVITY,1)>0) return 1;
    key=tick_key(h,tick,mask,limit,0);
    return greater?key>limit:key<limit;
}
static void zero_argument(const UpdateSequenceHooks *h,enum UpdateSequenceChild child) {
    observe(h,UPDATE_SEQUENCE_ZERO_ARGUMENT,0,0,0,0);
    h->consume(h->context,child);
    observe(h,UPDATE_SEQUENCE_DROP_ARGUMENT,0,0,0,0);
}
static void normal_readouts(const UpdateSequenceHooks *h,uint16_t tick) {
    uint8_t value;
    h->consume(h->context,UPDATE_PANEL_FRAME); h->consume(h->context,UPDATE_PANEL_IMAGE);
    h->consume(h->context,UPDATE_POSTFLIGHT); h->consume(h->context,UPDATE_HUD);
    h->consume(h->context,UPDATE_THREAT_LIGHTS); h->consume(h->context,UPDATE_COMPASS);
    if(activity(h,tick,3,1,1)) {
        h->consume(h->context,UPDATE_HEADING); h->consume(h->context,UPDATE_SPEED);
    }
    if(activity(h,tick,3,2,0)) h->consume(h->context,UPDATE_RECORD_2B);
    if(activity(h,tick,3,2,0) || signed_value(h,-0x8000)>-0x8000)
        h->consume(h->context,UPDATE_ALTITUDE);
    if(activity(h,tick,7,2,0)) h->consume(h->context,UPDATE_RECORD_72);
    if(activity(h,tick,15,2,0)) h->consume(h->context,UPDATE_GAUGE);
    marker(h,0x1a0);
    h->consume(h->context,UPDATE_PANEL_MARK); h->consume(h->context,UPDATE_WEAPON);
    h->consume(h->context,UPDATE_STORES); h->consume(h->context,UPDATE_GRID_Z);
    h->consume(h->context,UPDATE_GRID_X); h->consume(h->context,UPDATE_ZOOM);
    h->consume(h->context,UPDATE_MODE_BAR); h->consume(h->context,UPDATE_SCALE);
    h->consume(h->context,UPDATE_MESSAGE_LINE); h->consume(h->context,UPDATE_INDICATOR_BARS);
    value=byte(h,UPDATE_ACTIVITY,1);
    if((int8_t)value>=0) {
        wr_u8(UPDATE_ACTIVITY,(uint8_t)(value-1u));
        observe(h,UPDATE_SEQUENCE_ACTIVITY_DECREMENT,value,0,0,0);
    }
    marker(h,0x1d0);
}
static void context_readouts(const UpdateSequenceHooks *h) {
    uint8_t mode;
    if(!byte(h,ORIGIN_ENABLE,0)) return;
    wr_u8(CONTEXT_READOUTS,1); observe(h,UPDATE_SEQUENCE_CONTEXT_LATCH,1,0,0,0);
    h->consume(h->context,UPDATE_CONTEXT_SPEED); h->consume(h->context,UPDATE_CONTEXT_ALTITUDE);
    h->consume(h->context,UPDATE_CONTEXT_HEADING);
    mode=rd_u8(MODE_SELECT);
    observe(h,UPDATE_SEQUENCE_CONTEXT_MODE,mode,0,0,0);
    if((uint8_t)(mode-2u)==0) h->consume(h->context,UPDATE_CONTEXT_MESSAGE);
}
void run_game_update_sequence(const UpdateSequenceHooks *h) {
    uint16_t saved_tick;
    if(!h || !h->consume) abort();
    saved_tick=rd_u16(UPDATE_TICK);
    observe(h,UPDATE_SEQUENCE_BEGIN,saved_tick,0,0,0);
    h->consume(h->context,UPDATE_KEYS); h->consume(h->context,UPDATE_POST_INPUT);
    h->consume(h->context,UPDATE_NOTIFY);
    if(byte(h,UPDATE_ACTIVE,0)) {
        gaddr record; uint16_t index; uint8_t type;
        marker(h,8); h->consume(h->context,UPDATE_VIEW_CONTROLS);
        marker(h,0x10); h->consume(h->context,UPDATE_INDEXED_EMPTY);
        h->consume(h->context,UPDATE_RECORDS);
        marker(h,0x20); h->consume(h->context,UPDATE_POST_RECORD_EMPTY);
        h->consume(h->context,UPDATE_MATRIX); h->consume(h->context,UPDATE_PROJECTION);
        h->consume(h->context,UPDATE_OCTANT); h->consume(h->context,UPDATE_ATTITUDE);
        h->consume(h->context,UPDATE_CONTEXT);
        marker(h,0x60); h->consume(h->context,UPDATE_COCKPIT_SLIDE);
        h->consume(h->context,UPDATE_LIST_RESET);
        /* C0DA38's alternate buffer path unlinks this owner's frame. */
        if(h->consume(h->context,UPDATE_BUFFERS).owner_finished) return;
        marker(h,0x68);
        if(!byte(h,UPDATE_MAP_FLAGS,1) || byte(h,UPDATE_MAP_OVERRIDE,0)) h->consume(h->context,UPDATE_MAP);
        marker(h,0x70); h->consume(h->context,UPDATE_MATRIX_MARK);
        if(signed_value(h,-0x8000000)>-0x8000000) {
            uint32_t decision;
            marker(h,0x78); h->consume(h->context,UPDATE_PRIMARY_SCENE);
            marker(h,0x80); h->consume(h->context,UPDATE_ALTERNATE_SCENE);
            marker(h,0x90); h->consume(h->context,UPDATE_GRID);
            marker(h,0xa0);
            if(byte(h,ORIGIN_GATE_MODE,0)) h->consume(h->context,UPDATE_FLAGGED_SCENE);
            else {
                decision=h->consume(h->context,UPDATE_RANGE_DECISION).value;
                observe(h,UPDATE_SEQUENCE_DECISION,decision,0,0,0);
                if(decision) {
                    marker(h,0xa4); h->consume(h->context,UPDATE_TRUE_SCENE);
                    marker(h,0xa8); h->consume(h->context,UPDATE_TRUE_FOLLOWUP);
                } else {
                    marker(h,0xac); h->consume(h->context,UPDATE_FALSE_FOLLOWUP);
                    marker(h,0xb0); h->consume(h->context,UPDATE_FALSE_SCENE);
                }
            }
        }
        marker(h,0xc0); h->consume(h->context,UPDATE_MESSAGE);
        h->consume(h->context,UPDATE_RECORD_STATUS); marker(h,0xd0);
        if(byte(h,ORIGIN_ENABLE,1) && !byte(h,UPDATE_HUD_MODE,0)) context_readouts(h);
        else {
            index=rd_u16(TARGET_RECORD);
            record=CONTROL_RECORDS+((uint32_t)(int32_t)(int16_t)index<<9);
            type=rd_u8(record+0x62u);
            observe(h,UPDATE_SEQUENCE_RECORD,index,0,type,record);
            if(type!=0x30 || byte(h,UPDATE_HUD_MODE,0)) normal_readouts(h,saved_tick);
        }
        marker(h,0x1d4); h->consume(h->context,UPDATE_LOST_SELECTION);
        h->consume(h->context,UPDATE_READOUT_EMPTY); h->consume(h->context,UPDATE_RANGE_STAGE);
        h->consume(h->context,UPDATE_SELECTION_STAGE);
        if(tick_key(h,saved_tick,7,7,1)==7) h->consume(h->context,UPDATE_PERIODIC_READOUT);
        if(tick_key(h,saved_tick,15,4,1)==4) zero_argument(h,UPDATE_PERIODIC_WAIT);
        if(tick_key(h,saved_tick,31,8,1)==8) h->consume(h->context,UPDATE_PERIODIC_PAGE);
        else if(!byte(h,ORIGIN_DETAIL_MODE,1) && tick_key(h,saved_tick,31,16,0)==16)
            h->consume(h->context,UPDATE_PERIODIC_REDRAW);
        if(!byte(h,ORIGIN_GATE_A,1)) {
            uint16_t tick=rd_u16(UPDATE_TICK);
            wr_u16(UPDATE_TICK,(uint16_t)(tick+1u));
            observe(h,UPDATE_SEQUENCE_INCREMENT,tick,0,0,0);
        }
    } else {
        h->consume(h->context,UPDATE_IDLE_STATUS);
        zero_argument(h,UPDATE_IDLE_WAIT);
    }
    h->consume(h->context,UPDATE_AFTER_TICK);
    if(byte(h,UPDATE_TAIL_CONDITION,0) && byte(h,UPDATE_ACTIVE,0)) {
        marker(h,0x210); h->consume(h->context,UPDATE_TAIL_DRAW);
        marker(h,0x218); h->consume(h->context,UPDATE_TAIL_READOUT);
    }
    marker(h,0x220); h->consume(h->context,UPDATE_FINAL);
    observe(h,UPDATE_SEQUENCE_END,0,0,0,0);
}
void submit_update_display_buffers(const UpdateSequenceHooks *h) {
    uint16_t flags=rd_u16(UPDATE_DISPLAY_FLAGS);
    observe(h,UPDATE_SEQUENCE_DISPLAY_FLAGS,flags,0,0,0);
    h->consume(h->context,(flags&0x2000u)?UPDATE_DISPLAY_END:UPDATE_DISPLAY_PLANES);
}
