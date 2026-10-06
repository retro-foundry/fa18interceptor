#include "menu_setup.h"
#include "globals.h"
#include <stdlib.h>

static void observe(const MenuSetupHooks *h,enum MenuSetupPhase phase,uint32_t value,gaddr address) {
    if(h && h->observe) h->observe(h->context,phase,value,address);
}
static void consume(const MenuSetupHooks *h,enum MenuSetupCall call,uint32_t value) {
    if(!h->consume) abort();
    h->consume(h->context,call,value);
}
static void byte(const MenuSetupHooks *h,gaddr address,uint8_t value) {
    wr_u8(address,value); observe(h,MENU_SETUP_BYTE_STORE,value,address);
}
static void word(const MenuSetupHooks *h,gaddr address,uint16_t value) {
    wr_u16(address,value); observe(h,MENU_SETUP_WORD_STORE,value,address);
}
static void longword(const MenuSetupHooks *h,gaddr address,uint32_t value) {
    wr_u32(address,value); observe(h,MENU_SETUP_LONG_STORE,value,address);
}
void queue_top_level_menu_messages(const MenuSetupHooks *h) {
    static const uint16_t codes[]={6,100,101,102,103,104,105,106,107,108,109};
    gaddr cursor=MESSAGE_QUEUE;
    unsigned i;
    longword(h,MESSAGE_TIMER,0x1e0);
    for(i=0;i<sizeof codes/sizeof codes[0];++i) {
        wr_u16(cursor,codes[i]); observe(h,MENU_SETUP_QUEUE_WORD,codes[i],cursor);
        cursor+=2; observe(h,MENU_SETUP_QUEUE_NEXT,cursor,0);
    }
    wr_u16(cursor,0); observe(h,MENU_SETUP_QUEUE_WORD,0,cursor);
}
void begin_top_level_menu(const MenuSetupHooks *h) {
    gaddr cursor=MESSAGE_QUEUE;
    uint8_t flags;
    observe(h,MENU_SETUP_CURSOR,cursor,0);
    longword(h,MASTER_VOLUME_TARGET,0x1f0000);
    flags=rd_u8(SOUND_FLAGS); observe(h,MENU_SETUP_BIT_TEST,flags,7);
    if(flags&0x80u) {
        flags=rd_u8(VOLUME_FADING); observe(h,MENU_SETUP_BYTE_D0,flags,0);
        if(!flags) longword(h,MASTER_VOLUME,0x1f0000);
    }
    consume(h,MENU_SETUP_SOUND,15); byte(h,VOLUME_FADING,1);
    consume(h,MENU_SETUP_CLEAR,0); consume(h,MENU_SETUP_RESET,0);
    consume(h,MENU_SETUP_DELAY,0xc000);
}
void finish_top_level_menu(const MenuSetupHooks *h) {
    byte(h,CONTEXT_GATE,0);
    consume(h,MENU_SETUP_SCRIPT,0xc08490); queue_top_level_menu_messages(h);
    wr_u32(STAGE_CALLBACK,0xc0fcb4); observe(h,MENU_SETUP_CALLBACK,0xc0fcb4,0);
}
void start_top_level_menu(const MenuSetupHooks *h) {
    begin_top_level_menu(h);
    finish_top_level_menu(h);
}
void select_menu_sound_pair(uint32_t volume,const MenuSetupHooks *h) {
    uint8_t value=rd_u8(VOLUME_FADING);
    observe(h,MENU_SETUP_BYTE_D0,value,0);
    if(value) return;
    value=rd_u8(SOUND_FLAGS); observe(h,MENU_SETUP_BIT_TEST,value,7);
    if(value&0x80u) {
        consume(h,MENU_SOUND_FREE_BEFORE,0);
        consume(h,MENU_SOUND_FIXED_FIRST,63); consume(h,MENU_SOUND_FIXED_SECOND,63);
    } else {
        value=rd_u8(SOUND_FLAGS-1); observe(h,MENU_SETUP_BIT_TEST,value,2);
        if(!(value&4u)) { consume(h,MENU_SOUND_FREE_OTHER,0); return; }
        consume(h,MENU_SOUND_ARGUMENT_FIRST,volume); consume(h,MENU_SOUND_ARGUMENT_SECOND,volume);
    }
    byte(h,VOLUME_FADING,2);
}
void begin_menu_countdown(const MenuSetupHooks *h) {
    byte(h,POST_INPUT_AUX,1); word(h,POST_INPUT_COUNTDOWN,3);
    wr_u32(STAGE_CALLBACK,0xc0fb70); observe(h,MENU_SETUP_TAIL_CALLBACK,0xc0fb70,0);
}
uint16_t filter_cockpit_message(uint16_t code,const MenuSetupHooks *h) {
    uint16_t flags=rd_u16(COCKPIT_FLAGS),state;
    observe(h,MENU_SETUP_COCKPIT_READ,flags,0);
    observe(h,MENU_SETUP_BIT_TEST,flags,8);
    if(!(flags&0x100u)) return code;
    observe(h,MENU_SETUP_MESSAGE_READ,code,0); word(h,MESSAGE_CODE,code);
    flags|=1; observe(h,MENU_SETUP_COCKPIT_HELD,flags,0); word(h,COCKPIT_FLAGS,flags);
    code&=0xff00u; observe(h,MENU_SETUP_MESSAGE_MASK,code,0);
    observe(h,MENU_SETUP_MESSAGE_COMPARE,code,0x4000);
    if(code!=0x4000) observe(h,MENU_SETUP_MESSAGE_COMPARE,code,0x4800);
    if(code==0x4000 || code==0x4800) {
        state=rd_u16(MESSAGE_STATE)&0xdfffu;
        observe(h,MENU_SETUP_MESSAGE_STATE,state,0); word(h,MESSAGE_STATE,state);
    }
    return code;
}
void queue_indexed_menu_message(uint32_t mode,uint32_t position,uint32_t row,const MenuSetupHooks *h) {
    uint16_t index=(uint16_t)((uint16_t)((uint16_t)mode-3u)*2u),code;
    gaddr selected,destination;
    observe(h,MENU_SETUP_SELECTOR_BASE,0,MENU_SELECTOR_RECORDS);
    observe(h,MENU_SETUP_SELECTOR_MODE,mode,0);
    selected=MENU_SELECTOR_RECORDS+(gaddr)(int32_t)rd_s16(MENU_SELECTOR_DIRECTORY+(gaddr)(int32_t)(int16_t)index);
    observe(h,MENU_SETUP_SELECTOR_ROW,row,selected);
    code=rd_u16(selected+(gaddr)(int32_t)(int16_t)((uint16_t)row*2u));
    observe(h,MENU_SETUP_SELECTOR_CODE,code,0);
    destination=MESSAGE_QUEUE+(gaddr)(int32_t)(int16_t)((uint16_t)position*2u);
    observe(h,MENU_SETUP_SELECTOR_DESTINATION,position,MESSAGE_QUEUE);
    word(h,destination,code); word(h,destination+2,0);
}
