/* C1B77C-C1BB76: view mode, context detail, origin range and zoom commands. */
#include "view_commands.h"
#include "globals.h"
#include <stdlib.h>

#define VIEW_MODE_AUXILIARY 0xc457a8u
#define VIEW_MODE_COMPANION 0xc457a9u
#define VIEW_REFRESH_REQUEST 0xc45891u
#define VIEW_SPAN_OFFSETS 0xc1bad4u

static void observe(const ViewCommandHooks *h,enum ViewCommandPhase phase,
                    uint32_t value,uint32_t limit,gaddr address) {
    if(h->observe) h->observe(h->context,phase,value,limit,address);
}
static uint8_t test(const ViewCommandHooks *h,gaddr address) {
    uint8_t value=rd_u8(address); observe(h,VIEW_BYTE_TEST,value,0,address); return value;
}
static void byte(const ViewCommandHooks *h,gaddr address,uint8_t value) {
    wr_u8(address,value); observe(h,VIEW_BYTE_STORE,value,0,address);
}
static void word(const ViewCommandHooks *h,gaddr address,uint16_t value) {
    wr_u16(address,value); observe(h,VIEW_WORD_STORE,value,0,address);
}
static void request(const ViewCommandHooks *h,gaddr address,unsigned bit) {
    uint8_t old=rd_u8(address); wr_u8(address,old|(1u<<bit));
    observe(h,VIEW_REQUEST_BIT,old,bit,address);
}
static int compare(const ViewCommandHooks *h,uint8_t value,uint8_t limit) {
    observe(h,VIEW_BYTE_COMPARE,value,limit,0); return (int8_t)value-(int8_t)limit;
}
void set_context_view_detail(unsigned value,const ViewCommandHooks *h) {
    observe(h,VIEW_DETAIL_SET,value,0,0);
    byte(h,ORIGIN_DETAIL_INDEX,(uint8_t)value); byte(h,UPDATE_MASK,0xff);
    if(!test(h,FIRE_STATE)) byte(h,FIRE_STATE,0xff);
}
static void origin_level(const ViewCommandHooks *h,uint8_t value) {
    byte(h,ORIGIN_ENABLE,value); byte(h,UPDATE_MASK,0xff);
    if(!test(h,FIRE_STATE)) byte(h,FIRE_STATE,0xff);
}
static void origin_range(const ViewCommandHooks *h,int increase) {
    uint8_t old=rd_u8(ORIGIN_ENABLE),value;
    uint32_t middle;
    observe(h,VIEW_ORIGIN_READ,old,0,0);
    if(!old || test(h,ORIGIN_DETAIL_MODE)) return;
    if(!increase) request(h,PENDING_COMMAND_WORD_B,3);
    if(test(h,ORIGIN_GATE_MODE)) {
        middle=rd_u32(SELECTOR_ORIGIN_MIDDLE); observe(h,VIEW_MIDDLE_READ,middle,0,0);
        observe(h,increase?VIEW_MIDDLE_INCREASE:VIEW_MIDDLE_DECREASE,middle,0,0);
        middle=increase?middle+0x02000000u:middle-0x02000000u;
        observe(h,VIEW_MIDDLE_COMPARE,middle,increase?0x08000000u:0x01000000u,0);
        if(increase?(int32_t)middle>0x08000000:(int32_t)middle<0x01000000) {
            middle=increase?0x08000000u:0x01000000u;
            observe(h,VIEW_MIDDLE_SET,middle,0,0);
        }
        wr_u32(SELECTOR_ORIGIN_MIDDLE,middle);
        observe(h,VIEW_LONG_STORE,middle,0,SELECTOR_ORIGIN_MIDDLE);
        byte(h,UPDATE_MASK,0xff); return;
    }
    if(increase) {
        value=(uint8_t)(old+1); observe(h,VIEW_ORIGIN_INCREMENT,old,0,0);
        if(!value) { value=1; observe(h,VIEW_ORIGIN_SET,value,0,0); }
        else {
            observe(h,VIEW_ORIGIN_COMPARE,value,4,0);
            if((int8_t)value>=4) { value=4; observe(h,VIEW_ORIGIN_SET,value,0,0); }
        }
    } else {
        value=(uint8_t)(old-1); observe(h,VIEW_ORIGIN_DECREMENT,old,0,0);
        if((int8_t)old<=1) { value=0xff; observe(h,VIEW_ORIGIN_SET,(uint32_t)-1,0,0); }
    }
    origin_level(h,value);
}
static uint32_t finish_mode(uint32_t event,const ViewCommandHooks *h) {
    gaddr record;
    uint8_t value,type;
    int16_t span;
    uint16_t row;
    event=h->consume(h->context,VIEW_COMMAND_ZOOM_MAXIMUM);
    word(h,TARGET_MARK,0xffff); byte(h,UPDATE_MASK,0xff);
    byte(h,VIEW_MODE_COMPANION,0); byte(h,VIEW_REFRESH_REQUEST,0xff);
    record=CONTROL_RECORDS+(gaddr)(int32_t)rd_s16(VIEW_RECORD);
    observe(h,VIEW_RECORD_ADDRESS,0,0,record);
    type=rd_u8(record+0x62); observe(h,VIEW_RECORD_TYPE_READ,type,0,0);
    type&=0xf0; observe(h,VIEW_RECORD_TYPE_MASK,type,0,0);
    if(!compare(h,type,0x30)) return event;
    value=rd_u8(VIEW_MODE); observe(h,VIEW_MODE_READ,value,0,0);
    if(compare(h,value,3)>=0) {
        if(compare(h,value,9)<=0 || compare(h,value,12)>=0) {
            row=0xb3; observe(h,VIEW_ROW_SET,row,0,0);
            if(compare(h,value,5)<0 || compare(h,value,7)>0) {
                row=0xa7; observe(h,VIEW_ROW_SET,row,0,0);
            }
            observe(h,VIEW_ROW_COMPARE,row,rd_u16(LINE_LAST_ROW),0);
            if(row==rd_u16(LINE_LAST_ROW)) {
                word(h,SPAN_ORIGIN,0x32); word(h,SPAN_ORIGIN_Y,0x320); return event;
            }
        }
    }
    observe(h,VIEW_SPAN_TABLE,0,0,VIEW_SPAN_OFFSETS);
    value=rd_u8(VIEW_MODE); observe(h,VIEW_SPAN_INDEX,value,0,0);
    span=rd_s8(VIEW_SPAN_OFFSETS+(gaddr)(int32_t)(int8_t)value);
    observe(h,VIEW_SPAN_READ,(uint16_t)span,0,0);
    word(h,SPAN_ORIGIN,(uint16_t)span);
    observe(h,VIEW_SPAN_SCALE,(uint16_t)span,0,0);
    word(h,SPAN_ORIGIN_Y,(uint16_t)((uint16_t)span<<4));
    event=h->consume(h->context,VIEW_COMMAND_REDRAW);
    value=rd_u8(VIEW_MODE); observe(h,VIEW_MODE_READ,value,0,0);
    if(compare(h,value,3)<0) row=0x90;
    else if(compare(h,value,9)<=0) goto extended_row;
    else if(compare(h,value,12)>=0) goto extended_row;
    else row=0x90;
    word(h,LINE_LAST_ROW,row); return event;
extended_row:
    if(compare(h,value,4)<=0 || compare(h,value,8)>=0) row=0xa7;
    else row=0xb3;
    word(h,LINE_LAST_ROW,row); return event;
}
uint32_t select_zero_view_mode(uint32_t event,const ViewCommandHooks *h) {
    observe(h,VIEW_MODE_ZERO,0,0,0); byte(h,VIEW_MODE_AUXILIARY,0);
    byte(h,VIEW_MODE,0); byte(h,REDRAW_FIRST,3); return finish_mode(event,h);
}

int is_view_command(enum CommandAction action) {
    switch(action) {
    case COMMAND_CONTEXT_VIEW_DECREMENT: case COMMAND_CONTEXT_VIEW_INCREMENT:
    case COMMAND_CONTEXT_VIEW_ALTERNATE: case COMMAND_VIEW_ZERO: case COMMAND_VIEW_ONE:
    case COMMAND_VIEW_INCREMENT: case COMMAND_VIEW_DECREMENT: case COMMAND_VIEW_TWELVE:
    case COMMAND_VIEW_THIRTEEN: case COMMAND_VIEW_THREE: case COMMAND_VIEW_NINE:
    case COMMAND_VIEW_TOGGLE: case COMMAND_VIEW_EIGHT: case COMMAND_FIRE_REQUEST:
    case COMMAND_ZOOM_OUT: case COMMAND_ZOOM_IN: return 1;
    default: return 0;
    }
}
uint32_t execute_view_command(const CommandRequest *r,const ViewCommandHooks *h) {
    uint32_t event=r->raw_event;
    uint8_t value,old;
    uint16_t scale;
    unsigned mode=0;
    if(!h || !h->consume || !is_view_command(r->action)) abort();
    switch(r->action) {
    case COMMAND_CONTEXT_VIEW_DECREMENT:
        value=test(h,ORIGIN_ENABLE);
        if(value) { origin_range(h,0); break; }
        /* The source then loads the same byte into its detail working value. */
        value=rd_u8(ORIGIN_ENABLE); observe(h,VIEW_ORIGIN_READ,value,0,0);
        if(value) { request(h,PENDING_COMMAND_WORD_B+1,4); set_context_view_detail(8,h); }
        else { request(h,PENDING_COMMAND_WORD_B,3); byte(h,VIEW_REFRESH_REQUEST,0xff); }
        break;
    case COMMAND_CONTEXT_VIEW_ALTERNATE:
        value=rd_u8(ORIGIN_ENABLE); observe(h,VIEW_ORIGIN_READ,value,0,0);
        if(value) { request(h,PENDING_COMMAND_WORD_B+1,4); set_context_view_detail(8,h); }
        else { request(h,PENDING_COMMAND_WORD_B,3); byte(h,VIEW_REFRESH_REQUEST,0xff); }
        break;
    case COMMAND_CONTEXT_VIEW_INCREMENT:
        request(h,PENDING_COMMAND_WORD_B+1,7); origin_range(h,1); break;
    case COMMAND_VIEW_THREE:
        request(h,PENDING_COMMAND_WORD_B,7); set_context_view_detail(3,h); break;
    case COMMAND_VIEW_NINE:
        request(h,PENDING_COMMAND_WORD_B+1,5); set_context_view_detail(9,h); break;
    case COMMAND_VIEW_EIGHT:
        request(h,PENDING_COMMAND_WORD_B+1,4); set_context_view_detail(8,h); break;
    case COMMAND_VIEW_TOGGLE:
        request(h,PENDING_COMMAND_WORD_B,5);
        if(test(h,ORIGIN_ENABLE)) { set_context_view_detail(1,h); break; }
        goto mode_zero;
    case COMMAND_VIEW_ZERO:
        request(h,PENDING_COMMAND_WORD_B,4);
        observe(h,VIEW_ORIGIN_TEST,r->origin_mode,0,0);
        if(r->origin_mode) { set_context_view_detail(0,h); break; }
    mode_zero:
        return select_zero_view_mode(event,h);
    case COMMAND_VIEW_ONE:
        request(h,PENDING_COMMAND_WORD_B+1,0);
        observe(h,VIEW_ORIGIN_TEST,r->origin_mode,0,0);
        if(r->origin_mode) { set_context_view_detail(4,h); break; }
        mode=6; observe(h,VIEW_MODE_SET,mode,0,0); goto set_mode;
    case COMMAND_VIEW_INCREMENT:
        request(h,PENDING_COMMAND_WORD_B,6);
        observe(h,VIEW_ORIGIN_TEST,r->origin_mode,0,0);
        if(r->origin_mode) { set_context_view_detail(2,h); break; }
        byte(h,VIEW_MODE_AUXILIARY,0);
        old=rd_u8(VIEW_MODE); value=(uint8_t)(old+1); wr_u8(VIEW_MODE,value);
        observe(h,VIEW_MODE_INCREMENT,old,0,0);
        if(compare(h,value,11)<=0) return finish_mode(event,h);
        goto mode_zero;
    case COMMAND_VIEW_DECREMENT:
        request(h,PENDING_COMMAND_WORD_B+1,2);
        observe(h,VIEW_ORIGIN_TEST,r->origin_mode,0,0);
        if(r->origin_mode) { set_context_view_detail(6,h); break; }
        byte(h,VIEW_MODE_AUXILIARY,0);
        old=rd_u8(VIEW_MODE); value=(uint8_t)(old-1); wr_u8(VIEW_MODE,value);
        observe(h,VIEW_MODE_DECREMENT,old,0,0);
        if((int8_t)old<1) { mode=11; observe(h,VIEW_MODE_SET,mode,0,0); goto store_mode; }
        if(!value || compare(h,value,11)>=0) goto mode_zero;
        return finish_mode(event,h);
    case COMMAND_VIEW_TWELVE: case COMMAND_VIEW_THIRTEEN:
        request(h,PENDING_COMMAND_WORD_B+1,r->action==COMMAND_VIEW_TWELVE?3:1);
        observe(h,VIEW_ORIGIN_TEST,r->origin_mode,0,0);
        if(r->origin_mode) { set_context_view_detail(r->action==COMMAND_VIEW_TWELVE?7:5,h); break; }
        mode=r->action==COMMAND_VIEW_TWELVE?12:13;
        observe(h,VIEW_MODE_SET,mode,0,0); byte(h,VIEW_MODE_AUXILIARY,0);
        byte(h,UPDATE_MASK,0xff); goto store_mode;
    set_mode:
        byte(h,VIEW_MODE_AUXILIARY,0);
    store_mode:
        byte(h,VIEW_MODE,(uint8_t)mode); byte(h,REDRAW_FIRST,3);
        return finish_mode(event,h);
    case COMMAND_FIRE_REQUEST:
        request(h,PENDING_COMMAND_WORD_B+1,6); byte(h,VIEW_REFRESH_REQUEST,5); break;
    case COMMAND_ZOOM_OUT:
        observe(h,VIEW_ZOOM_OUT_BEGIN,0x20,0,0);
        observe(h,VIEW_ORIGIN_TEST,r->origin_mode,0,0);
        if(r->origin_mode && test(h,ORIGIN_GATE_B)) { origin_range(h,0); break; }
        scale=rd_u16(ZOOM_SCALE); observe(h,VIEW_ZOOM_OUT_COMPARE,scale,0x20,0);
        if((int16_t)scale>0x20) {
            observe(h,VIEW_ZOOM_DECREASE,scale,0,0);
            wr_u16(ZOOM_SCALE,(uint16_t)((int16_t)scale>>1));
        }
        goto zoom_refresh;
    case COMMAND_ZOOM_IN:
        observe(h,VIEW_ORIGIN_TEST,r->origin_mode,0,0);
        if(r->origin_mode && test(h,ORIGIN_GATE_B)) {
            request(h,PENDING_COMMAND_WORD_B+1,7); origin_range(h,1); break;
        }
        scale=rd_u16(ZOOM_SCALE); observe(h,VIEW_ZOOM_IN_COMPARE,scale,0x80,0);
        if((int16_t)scale<0x80) {
            observe(h,VIEW_ZOOM_INCREASE,scale,0,0); wr_u16(ZOOM_SCALE,(uint16_t)(scale<<1));
        }
    zoom_refresh:
        byte(h,0xc4583du,3); byte(h,VIEW_REFRESH_REQUEST,0xff);
        value=rd_u8(ZOOM_FLAGS); observe(h,VIEW_ZOOM_FLAGS_READ,value,0,0);
        value&=0x7f; observe(h,VIEW_ZOOM_FLAGS_MASK,value,0,0);
        if(!value) byte(h,UPDATE_MASK,0xff);
        scale=rd_u16(ZOOM_SCALE); observe(h,VIEW_ZOOM_IN_COMPARE,scale,0x80,0);
        old=rd_u8(ZOOM_FLAGS);
        wr_u8(ZOOM_FLAGS,scale==0x80?old|0x80:old&0x7f);
        observe(h,scale==0x80?VIEW_ZOOM_FLAGS_SET:VIEW_ZOOM_FLAGS_CLEAR,old,7,ZOOM_FLAGS);
        break;
    default: abort();
    }
    return event;
}
