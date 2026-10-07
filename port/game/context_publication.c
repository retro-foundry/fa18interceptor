#include "context_publication.h"
#include "globals.h"
#include <stdlib.h>

static void observe(const ContextPublicationHooks *h,enum ContextPublicationPhase phase,
                    uint32_t value,uint32_t limit,gaddr address) {
    if(h->observe) h->observe(h->context,phase,value,limit,address);
}
static ContextPublicationResult consume(const ContextPublicationHooks *h,enum ContextPublicationChild child) {
    if(!h->consume) abort();
    return h->consume(h->context,child);
}
static uint8_t test(const ContextPublicationHooks *h,gaddr address) {
    uint8_t value=rd_u8(address);
    observe(h,CONTEXT_PUBLISH_BYTE_TEST,value,0,address); return value;
}
static void byte(const ContextPublicationHooks *h,gaddr address,uint8_t value) {
    wr_u8(address,value); observe(h,CONTEXT_PUBLISH_BYTE_STORE,value,0,address);
}
static void word(const ContextPublicationHooks *h,gaddr address,uint16_t value) {
    wr_u16(address,value); observe(h,CONTEXT_PUBLISH_WORD_STORE,value,0,address);
}
void publish_context_detail_command(uint8_t event,const ContextPublicationHooks *h) {
    set_context_view_detail(4,h->view); publish_command_event(event,h->publication);
}
uint8_t publish_context_toggle_command(uint8_t event,gaddr flag,const ContextPublicationHooks *h) {
    byte(h,flag,test(h,flag)?0:1); return publish_command_event(event,h->publication);
}
void publish_context_record_command(uint32_t event,int16_t index,const ContextPublicationHooks *h) {
    ContextPublicationResult child;
    gaddr record;
    uint8_t type;
    byte(h,UPDATE_MASK,0xff); word(h,SELECTION_MARKER,0xffff); byte(h,FIRE_STATE,0xfe);
    word(h,TARGET_RECORD,(uint16_t)index);
    observe(h,CONTEXT_PUBLISH_INDEX_SCALE,(uint16_t)index,0,0);
    word(h,VIEW_RECORD,(uint16_t)((uint16_t)index<<9));
    child=consume(h,CONTEXT_PUBLISH_ZOOM); event=child.event;
    if(test(h,CONTEXT_SELECT)) {
        byte(h,CONTEXT_SELECT,rd_u8(CONTEXT_PUBLISH_RETURN_MODE)); byte(h,REDRAW_FIRST,3);
    } else {
        observe(h,CONTEXT_PUBLISH_RECORD_BASE,0,0,CONTROL_RECORDS);
        record=CONTROL_RECORDS+(gaddr)(int32_t)child.record_offset;
        word(h,ORIGIN_ANGLE_HISTORY,rd_u16(record+0x68));
        type=rd_u8(record+0x62); observe(h,CONTEXT_PUBLISH_RECORD_KIND,type,0,0);
        type&=0xf0; observe(h,CONTEXT_PUBLISH_RECORD_MASK,type,0,0);
        observe(h,CONTEXT_PUBLISH_RECORD_COMPARE,type,0x30,0);
        if(type!=0x30) event=select_zero_view_mode(event,h->view);
        else {
            word(h,SPAN_ORIGIN,0x32); word(h,SPAN_ORIGIN_Y,0x320);
            wr_u8(VIEW_MODE,0); observe(h,CONTEXT_PUBLISH_BYTE_ZERO,0,0,VIEW_MODE);
            event=consume(h,CONTEXT_PUBLISH_VIEW).event;
            /* The called C1BA86 already published once. The source still
             * reaches the queue tail again; KEY_TAKEN decides any second write. */
            word(h,LINE_LAST_ROW,0xa7);
        }
    }
    publish_command_event((uint8_t)event,h->publication);
}
void set_selected_record_request(uint8_t level,const ContextPublicationHooks *h) {
    gaddr record=CONTROL_RECORDS+(gaddr)(int32_t)rd_s16(CHOSEN_RECORD);
    uint8_t value;
    observe(h,CONTEXT_PUBLISH_SELECTED_RECORD,0,0,record);
    value=rd_u8(record+3); observe(h,CONTEXT_PUBLISH_BIT_TEST,value,7,record+3);
    if(value&0x80u) return;
    observe(h,CONTEXT_PUBLISH_BYTE_TEST,level,0,0);
    value=rd_u8(record+0x7c); wr_u8(record+0x7c,level?value|0x80u:value&0x7fu);
    observe(h,level?CONTEXT_PUBLISH_BIT_SET:CONTEXT_PUBLISH_BIT_CLEAR,value,7,record+0x7c);
}
void clear_matching_record_selection(const ContextPublicationHooks *h) {
    uint16_t selected=rd_u16(SELECTED_RECORD),chosen;
    uint32_t causes;
    observe(h,CONTEXT_PUBLISH_SELECTED_READ,selected,0,0);
    observe(h,CONTEXT_PUBLISH_SELECTED_COMPARE,selected,0xffff,0);
    if(selected==0xffff) return;
    chosen=rd_u16(CHOSEN_RECORD); observe(h,CONTEXT_PUBLISH_CHOSEN_COMPARE,selected,chosen,0);
    if(selected!=chosen) return;
    observe(h,CONTEXT_PUBLISH_SELECTION_TONE_ID,0x4016,0,0);
    consume(h,CONTEXT_PUBLISH_SELECTION_TONE);
    word(h,SELECTED_RECORD,0xffff);
    causes=rd_u32(WARNING_CAUSES)&0xffffbdffu;
    wr_u32(WARNING_CAUSES,causes); observe(h,CONTEXT_PUBLISH_WARNING_MASK,causes,0,WARNING_CAUSES);
}
