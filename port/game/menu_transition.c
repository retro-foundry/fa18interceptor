#include "menu_transition.h"
#include "globals.h"
#include <stdlib.h>

static void observe(const MenuTransitionHooks *h,enum MenuTransitionPhase phase,
                    uint32_t value,uint32_t extra,gaddr address) {
    if(h->observe) h->observe(h->context,phase,value,extra,address);
}
static MenuTransitionResult consume(const MenuTransitionHooks *h,enum MenuTransitionCall call,uint32_t value) {
    if(!h->consume) abort();
    return h->consume(h->context,call,value);
}
static uint8_t test(const MenuTransitionHooks *h,gaddr address) {
    uint8_t value=rd_u8(address); observe(h,MENU_BYTE_TEST,value,0,address); return value;
}
static uint8_t byte_d0(const MenuTransitionHooks *h,gaddr address) {
    uint8_t value=rd_u8(address); observe(h,MENU_BYTE_D0,value,0,address); return value;
}
static void byte(const MenuTransitionHooks *h,gaddr address,uint8_t value) {
    wr_u8(address,value); observe(h,MENU_BYTE_STORE,value,0,address);
}
static void word(const MenuTransitionHooks *h,gaddr address,uint16_t value) {
    wr_u16(address,value); observe(h,MENU_WORD_STORE,value,0,address);
}
static void longword(const MenuTransitionHooks *h,gaddr address,uint32_t value) {
    wr_u32(address,value); observe(h,MENU_LONG_STORE,value,0,address);
}
static void callback(const MenuTransitionHooks *h,gaddr value) {
    wr_u32(STAGE_CALLBACK,value); observe(h,MENU_CALLBACK,value,0,STAGE_CALLBACK);
}
static void queue(const MenuTransitionHooks *h,gaddr *cursor,uint16_t value,int advance) {
    wr_u16(*cursor,value); observe(h,MENU_QUEUE_WORD,value,0,*cursor);
    if(advance) { *cursor+=2; observe(h,MENU_QUEUE_ADVANCE,*cursor,0,0); }
}
void follow_top_level_menu(const MenuTransitionHooks *h) {
    uint8_t mode=rd_u8(MODE_SELECT),current;
    gaddr cursor=MESSAGE_QUEUE;
    uint16_t code=0;
    observe(h,MENU_FOLLOW_LOCALS,mode,0,cursor);
    if(test(h,SEQUENCE_PHASE)) {
        byte(h,SEQUENCE_PHASE,0);
        observe(h,MENU_PHASE_SUBTRACT,rd_u16(0xc45772u),0,0);
        if(rd_u16(0xc45772u)==6) return;
        consume(h,MENU_PHASE_RESET,0); callback(h,0xc0fbe0u); return;
    }
    current=byte_d0(h,MODE_SELECT);
    if((int8_t)current>0) {
        longword(h,MASTER_VOLUME_TARGET,0); consume(h,MENU_POSITIVE_RESET,0);
        consume(h,MENU_POSITIVE_CLEAR,0);
        observe(h,MENU_COMPARE_BYTE,mode,0x7f,0);
        if(mode==0x7f) code=0x65;
        else {
            observe(h,MENU_COMPARE_BYTE,mode,1,0);
            if(mode==1) code=0x66;
            else {
                observe(h,MENU_COMPARE_BYTE,mode,2,0);
                if(mode==2) code=0x67;
                else {
                    observe(h,MENU_COMPARE_BYTE,mode,0x7d,0);
                    if(mode==0x7d) code=0x68;
                    else {
                        observe(h,MENU_COMPARE_BYTE,mode,9,0);
                        if(mode==9) code=0x69;
                        else if(test(h,0xc45792u)) code=0x6b;
                    }
                }
            }
        }
        if(code) { queue(h,&cursor,code,1); if(code==0x6b) byte(h,0xc45792u,0); }
        queue(h,&cursor,0,0);
        current=byte_d0(h,SEQUENCE_FLAG);
        if(!current) {
            current=rd_u8(SOUND_FLAGS); observe(h,MENU_BIT_TEST,current,7,0);
            word(h,POST_INPUT_COUNTDOWN,(current&0x80u)?0xd2:0x96);
        } else word(h,POST_INPUT_COUNTDOWN,0x96);
        callback(h,0xc0feceu); return;
    }
    current=byte_d0(h,MODE_SELECT);
    if((int8_t)current<0) {
        consume(h,MENU_NEGATIVE_RESET,0); consume(h,MENU_NEGATIVE_CLEAR,0);
        current=byte_d0(h,MODE_SELECT); observe(h,MENU_COMPARE_BYTE,current,0xff,0);
        if(current==0xff) callback(h,0xc1017eu);
        else {
            consume(h,MENU_OTHER_CLEAR,0); consume(h,MENU_SUMMARY,0);
            queue(h,&cursor,0x57,1); queue(h,&cursor,0,0); callback(h,0xc0fe36u);
        }
        byte(h,MODE_SELECT,0);
    } else byte(h,SEQUENCE_PHASE,0);
}
static void refresh(const MenuTransitionHooks *h) {
    uint8_t pose=byte_d0(h,SCENE_POSE_ENTRY);
    observe(h,MENU_POSE_SUBTRACT,pose,0,0);
    if(pose==3) byte(h,POSTFLIGHT_FAILURE_INPUT,0x11);
    consume(h,MENU_REFRESH,0);
}
static void mode_nine(const MenuTransitionHooks *h) {
    byte(h,SCENE_POSE_ENTRY,3); observe(h,MENU_FULL_D0,0,0,0);
    byte(h,CONTEXT_STATE,0); byte(h,PAUSE_A,0);
    consume(h,MENU_MODE_NINE_ROOT,0); consume(h,MENU_MODE_NINE_POSITION,0);
    consume(h,MENU_MODE_NINE_RESET,0); observe(h,MENU_FULL_D0,1,0,0);
    byte(h,CONTEXT_GATE,1); byte(h,POST_INPUT_AUX,1);
    observe(h,MENU_FULL_D0,0,0,0); word(h,SPAN_ORIGIN,0); word(h,SPAN_ORIGIN_Y,0);
    consume(h,MENU_MODE_NINE_VIEW,0); byte(h,UPDATE_MASK,0xff); byte(h,CONTEXT_SELECT,0);
    callback(h,0xc101fcu);
}
static void demonstration(const MenuTransitionHooks *h) {
    byte(h,POST_INPUT_EVENT,1); callback(h,0xc0fa04u);
}
void enter_menu_mode_nine(const MenuTransitionHooks *h) { mode_nine(h); refresh(h); }
void enter_menu_demonstration(const MenuTransitionHooks *h) { demonstration(h); refresh(h); }
void advance_delayed_menu(const MenuTransitionHooks *h) {
    int16_t mode=(int8_t)rd_u8(MODE_SELECT),delay=rd_s16(POST_INPUT_COUNTDOWN);
    int offset;
    uint16_t request;
    observe(h,MENU_DELAY_LOCALS,(uint16_t)mode,(uint16_t)delay,0);
    if(delay>=0) return;
    byte(h,VOLUME_FADING,0); consume(h,MENU_STOP_ZERO,0); consume(h,MENU_STOP_ONE,1);
    longword(h,MASTER_VOLUME,0x3f0000);
    observe(h,MENU_WORD_D0,(uint16_t)mode,0,0);
    observe(h,MENU_COMPARE_WORD,(uint16_t)mode,3,0);
    if((uint16_t)mode>=3) observe(h,MENU_COMPARE_WORD,(uint16_t)mode,8,0);
    if((uint16_t)mode>=3 && (uint16_t)mode<=8) consume(h,MENU_SOUND_PAIR,40);
    else {
        uint8_t sound=rd_u8(SOUND_FLAGS); observe(h,MENU_BIT_TEST,sound,4,0);
        consume(h,(sound&0x10u)?MENU_NOISE:MENU_SCRIPTED_NOISE,8);
    }
    word(h,LINE_LAST_ROW,0xb3); consume(h,MENU_DELAY_RESET,0);
    word(h,POST_INPUT_COUNTDOWN,4); byte(h,POSTFLIGHT_RESET_REMAINING,3); byte(h,ATTEMPTS_LEFT,2);
    consume(h,MENU_DELAY_ROOT,0); byte(h,POST_INPUT_EVENT,0); byte(h,POST_INPUT_AUX,1);
    byte(h,REDRAW_FIRST,3); byte(h,VIEWPORT_TARGET,rd_u8(VIEWPORT_MODE));
    consume(h,MENU_DELAY_VIEWPORT,0); callback(h,0xc103e4u);
    byte(h,SCENE_POSE_ENTRY,3); byte(h,POSTFLIGHT_FAILURE_INPUT,0x11);
    observe(h,MENU_TABLE_BEGIN,(uint16_t)mode,0,0);
    /* Read all six keys from the original eight-byte key/BRA table, in the
     * source's reverse scan order. The static audit seals every arm. */
    for(offset=40;offset>=0;offset-=8) {
        uint32_t key=rd_u32(0xc0ffdeu+(gaddr)offset);
        observe(h,MENU_TABLE_NEXT,(uint32_t)offset,0,0);
        observe(h,MENU_TABLE_COMPARE,(uint16_t)mode,key,0);
        if(key==(uint16_t)mode) break;
    }
    if(offset<0) observe(h,MENU_TABLE_NEXT,(uint32_t)offset,0,0);
    switch(offset) {
    case 0: mode_nine(h); break;
    case 8:
        byte(h,POSTFLIGHT_RESET_REMAINING,1); word(h,LINE_LAST_ROW,0xa7);
        byte(h,CONTEXT_STARTED,0xff); byte(h,0xc457b6u,3); byte(h,SCENE_POSE_ENTRY,2);
        consume(h,MENU_MODE_RESTORE,0); consume(h,MENU_MODE_RESTORE_STATE,0);
        consume(h,MENU_MODE_RESTORE_POSITION,0); consume(h,MENU_MODE_RESTORE_ROOT,0);
        byte(h,PAUSE_A,0); callback(h,0xc10272u); break;
    case 16: byte(h,SCENE_POSE_ENTRY,0); break;
    case 24:
        word(h,LINE_LAST_ROW,0xa7); byte(h,CONTEXT_STARTED,0xff); consume(h,MENU_MODE_TWO_ROOT,0);
        observe(h,MENU_FULL_D0,0,0,0); byte(h,PAUSE_A,0);
        observe(h,MENU_FULL_D0,1,0,0); byte(h,CONTEXT_SELECT,1); byte(h,CONTEXT_STATE,0);
        byte(h,0xc457b5u,1); word(h,0xc458dcu,4); word(h,VIEW_RECORD,0x800); byte(h,VIEW_SIDE,3);
        request=rd_u16(SECONDARY_REQUEST_FLAGS)|0x100u;
        observe(h,MENU_REQUEST_WORD,rd_u16(SECONDARY_REQUEST_FLAGS),0,0);
        word(h,SECONDARY_REQUEST_FLAGS,request); callback(h,0xc10272u); break;
    case 32:
        consume(h,MENU_MODE_ONE_ROOT,0); byte(h,POSTFLIGHT_RESET_REMAINING,5); callback(h,0xc101fcu); break;
    case 40: demonstration(h); break;
    default: break;
    }
    refresh(h);
}
void start_menu_alert_pair(uint32_t argument,const MenuTransitionHooks *h) {
    uint8_t enabled=rd_u8(0xc45b5au);
    observe(h,MENU_BIT_TEST,enabled,2,0);
    if(!(enabled&4u)) return;
    consume(h,MENU_PAIR_FIRST,argument); consume(h,MENU_PAIR_SECOND,argument);
}
static uint32_t divide_word(uint32_t value,uint16_t divisor) {
    uint32_t quotient=value/divisor;
    return quotient>0xffffu?value:((value%divisor)<<16)|quotient;
}
static MenuTransitionResult field(const MenuTransitionHooks *h,enum MenuTransitionCall call,
                                  uint32_t value,gaddr text,unsigned offset,unsigned width) {
    observe(h,MENU_FORMAT_FIELD,value,(offset<<8)|width,text); return consume(h,call,value);
}
void format_menu_summary(const MenuTransitionHooks *h) {
    static const struct { unsigned record_offset; gaddr text; unsigned offset,width; } fields[]={
        {0x36,0xc3f558u,0x25,3},{0x38,0xc3f583u,0x25,3},
        {0x3a,0xc3f5d9u,0x17,5},{0x3c,0xc3f5d9u,0x20,4},
        {0x3e,0xc3f601u,0x17,5},{0x40,0xc3f601u,0x20,4},
        {0x42,0xc3f629u,0x17,5},{0x44,0xc3f629u,0x20,4}
    };
    MenuTransitionResult result={0,rd_u32(MODE_TABLE)};
    uint32_t packed;
    unsigned i;
    observe(h,MENU_FORMAT_BEGIN,0,0,result.record);
    for(i=0;i<8;++i) {
        result.value=(result.value&0xffff0000u)|rd_u16(result.record+fields[i].record_offset);
        observe(h,MENU_WORD_D0,result.value,0,0);
        result=field(h,(enum MenuTransitionCall)(MENU_FIELD_FIRST+i),result.value,
                     fields[i].text,fields[i].offset,fields[i].width);
    }
    result.value=rd_u32(result.record+8); observe(h,MENU_LONG_D0,result.value,0,0);
    observe(h,MENU_DIVIDE,3600,0,0); packed=divide_word(result.value,3600);
    observe(h,MENU_SAVE_TIME,packed,0,0); result.value=(uint32_t)(int32_t)(int16_t)packed;
    observe(h,MENU_EXTEND_TIME,result.value,0,0);
    result=field(h,MENU_FIELD_HOURS,result.value,0xc3f655u,0x17,4);
    observe(h,MENU_RESTORE_TIME,packed,0,0);
    packed=(packed<<16)|(packed>>16); observe(h,MENU_SWAP_TIME,packed,0,0);
    packed=(uint32_t)(int32_t)(int16_t)packed; observe(h,MENU_EXTEND_TIME,packed,0,0);
    observe(h,MENU_DIVIDE,60,0,0); result.value=(uint32_t)(int32_t)(int16_t)divide_word(packed,60);
    observe(h,MENU_EXTEND_TIME,result.value,0,0);
    result=field(h,MENU_FIELD_MINUTES,result.value,0xc3f655u,0x21,2);
    result.value=(result.value&0xffff0000u)|rd_u16(result.record+0x46);
    observe(h,MENU_WORD_D0,result.value,0,0);
    result=field(h,MENU_FIELD_NINTH,result.value,0xc3f67fu,0x17,4);
    result.value=(result.value&0xffff0000u)|rd_u16(result.record+0x48);
    observe(h,MENU_WORD_D0,result.value,0,0);
    field(h,MENU_FIELD_LAST,result.value,0xc3f69du,0x17,4);
}
