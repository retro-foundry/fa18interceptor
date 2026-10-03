#include "menu_return.h"
#include "globals.h"
#include "stages.h"
#include <stdlib.h>
/* Complete source-owned menu/context return callbacks. Observer phases carry
 * CPU outputs; actual child calls retain their source entry/return contracts. */
static void observe(const MenuReturnHooks *h,enum MenuReturnPhase phase,uint32_t value,gaddr address) {
    if(h && h->observe) h->observe(h->context,phase,value,address);
}
static void consume(const MenuReturnHooks *h,enum MenuReturnChild child) {
    if(!h || !h->consume) abort();
    h->consume(h->context,child);
}
static void reset(const MenuReturnHooks *h,enum MenuReturnChild child) {
    if(h) consume(h,child); else reset_message_sequence();
}
static void byte(const MenuReturnHooks *h,gaddr address,uint8_t value) {
    wr_u8(address,value); observe(h,MR_BYTE_STORE,value,address);
}
static void word(const MenuReturnHooks *h,gaddr address,uint16_t value) {
    wr_u16(address,value); observe(h,MR_WORD_STORE,value,address);
}
static uint8_t read_byte(const MenuReturnHooks *h,gaddr address) {
    uint8_t value=rd_u8(address); observe(h,MR_BYTE_D0,value,0); return value;
}
static uint8_t subtract(const MenuReturnHooks *h,uint8_t value,uint8_t amount) {
    observe(h,MR_SUBTRACT_BYTE,value,amount); return (uint8_t)(value-amount);
}
static void callback(const MenuReturnHooks *h,gaddr entry) {
    wr_u32(STAGE_CALLBACK,entry); observe(h,MR_CALLBACK,entry,0);
}
static int expired(const MenuReturnHooks *h) {
    uint16_t count=rd_u16(POST_INPUT_COUNTDOWN);
    observe(h,MR_WORD_D0,count,0); return (int16_t)count<0;
}
void finish_menu_context_three(const MenuReturnHooks *h) {
    if(subtract(h,read_byte(h,CONTEXT_STATE),3)) return;
    observe(h,MR_FULL_D0,0,0); byte(h,CONTEXT_SMOOTH,0); byte(h,MENU_TRANSITION_FLAG,0);
    word(h,POST_INPUT_COUNTDOWN,2); callback(h,ROUTINE_MODE_FOUR);
}
void follow_menu_return_message(const MenuReturnHooks *h) {
    if(!subtract(h,read_byte(h,KEY_TAKEN),2)) { consume(h,MR_MESSAGE_CANCEL); return; }
    if((int8_t)read_byte(h,MESSAGE_STATE_C)>=0) return;
    byte(h,POST_INPUT_AUX,1); consume(h,MR_MESSAGE_RESET); byte(h,MENU_TRANSITION_FLAG,1);
    word(h,POST_INPUT_COUNTDOWN,3); callback(h,0xc10942);
}
void follow_menu_return_context(const MenuReturnHooks *h) {
    if(!subtract(h,read_byte(h,KEY_TAKEN),2)) { consume(h,MR_CONTEXT_CANCEL); return; }
    if(subtract(h,read_byte(h,CONTEXT_STATE),4)) return;
    observe(h,MR_FULL_D0,0,0); byte(h,CONTEXT_SMOOTH,0); byte(h,MENU_TRANSITION_FLAG,0);
    word(h,POST_INPUT_COUNTDOWN,2); callback(h,0xc109ac);
}
void begin_menu_context_ready(const MenuReturnHooks *h) {
    observe(h,MR_FULL_D0,1,0); byte(h,POST_INPUT_AUX,1); byte(h,UPDATE_MASK,0xff);
    byte(h,CONTEXT_GATE,1); word(h,POST_INPUT_COUNTDOWN,5); callback(h,0xc10302);
}
void choose_menu_exit_after_countdown(const MenuReturnHooks *h) {
    if(!expired(h)) return;
    if(!read_byte(h,SEQUENCE_FLAG)) {
        byte(h,POST_INPUT_AUX,0); word(h,MESSAGE_QUEUE,0x49); callback(h,ROUTINE_LEAVE_ON_KEY);
    } else {
        byte(h,POST_INPUT_AUX,1); reset(h,MR_CHOOSE_RESET); callback(h,ROUTINE_AFTER_POST_INPUT);
    }
}
void leave_menu_on_key_or_message(const MenuReturnHooks *h) {
    if((int8_t)read_byte(h,MESSAGE_STATE_C)>=0) {
        uint8_t key=rd_u8(KEY_TAKEN); observe(h,MR_BYTE_TEST,key,0); if(!key) return;
    }
    byte(h,POST_INPUT_AUX,1); reset(h,MR_LEAVE_RESET); callback(h,ROUTINE_AFTER_POST_INPUT);
}
void reset_menu_viewport_after_countdown(const MenuReturnHooks *h) {
    if(!expired(h)) return;
    observe(h,MR_FULL_D0,0,0); byte(h,POST_INPUT_AUX,0); byte(h,VIEWPORT_TARGET,15);
    byte(h,VIEWPORT_MODE,0); callback(h,ROUTINE_ENTER_MODE_FOUR);
}
void enter_menu_mode_four(const MenuReturnHooks *h) {
    uint8_t mode=rd_u8(VIEWPORT_MODE),target=rd_u8(VIEWPORT_TARGET);
    observe(h,MR_VIEWPORT_COMPARE,mode,target); if(mode!=target) return;
    word(h,POST_INPUT_COUNTDOWN,2); observe(h,MR_FULL_D0,1,0);
    byte(h,POST_INPUT_AUX,1); byte(h,UPDATE_MASK,0xff); byte(h,CONTEXT_STATE,4);
    byte(h,CONTEXT_GATE,1); word(h,POST_INPUT_COUNTDOWN,5); callback(h,ROUTINE_MODE_FOUR);
}
void start_menu_smoothing(const MenuReturnHooks *h) {
    if(!subtract(h,read_byte(h,KEY_TAKEN),2)) { consume(h,MR_SMOOTH_CANCEL); return; }
    if(!expired(h)) return;
    byte(h,CONTEXT_SMOOTH,1); callback(h,0xc10970);
}
void complete_menu_return_after_countdown(const MenuReturnHooks *h) {
    uint8_t mode;
    if(!subtract(h,read_byte(h,KEY_TAKEN),2)) { consume(h,MR_END_CANCEL); return; }
    if(!expired(h)) return;
    byte(h,POST_INPUT_AUX,0); mode=read_byte(h,MODE_SELECT); observe(h,MR_COMPARE_BYTE,mode,1);
    if((int8_t)mode>1) byte(h,CONTEXT_SMOOTH,1);
    mode=read_byte(h,MODE_SELECT); observe(h,MR_SIGNED_MODE,mode,0);
    observe(h,MR_COMPARE_LONG,(uint32_t)(int32_t)(int8_t)mode,1);
    if(mode==1) { word(h,MESSAGE_QUEUE,7); byte(h,CONTEXT_STATE,5); byte(h,MENU_TRANSITION_FLAG,1); }
    observe(h,MR_FULL_D0,0,0); byte(h,CONTEXT_GATE,0); byte(h,MESSAGE_STATE_B,0); callback(h,0xc10a24);
}
void select_menu_return_message(const MenuReturnHooks *h) {
    uint8_t key=rd_u8(KEY_TAKEN),mode; gaddr cursor=MESSAGE_QUEUE;
    observe(h,MR_CURSOR_INIT,cursor,0); observe(h,MR_BYTE_TEST,key,0);
    if(key) { consume(h,MR_SELECT_KEY); return; }
    if(!expired(h)) return;
    byte(h,POST_INPUT_AUX,0); mode=subtract(h,read_byte(h,MODE_SELECT),2);
    word(h,cursor,mode?10:9); observe(h,MR_QUEUE_CODE,mode?10:9,cursor);
    observe(h,MR_CURSOR_NEXT,cursor+2,0);
    if(mode) byte(h,CONTEXT_GATE,0);
    callback(h,0xc10362);
}
void cancel_menu_return(const MenuReturnHooks *h) {
    byte(h,POST_INPUT_AUX,1); consume(h,MR_CANCEL_REFRESH);
    byte(h,UPDATE_MASK,0xff); byte(h,CONTEXT_STATE,0); consume(h,MR_CANCEL_RESET);
    observe(h,MR_FULL_D0,0,0); byte(h,POST_INPUT_EVENT,0); byte(h,CONTEXT_SMOOTH,0);
    observe(h,MR_FULL_D0,1,0); byte(h,MENU_TRANSITION_FLAG,1); byte(h,CONTEXT_STARTED,1);
    observe(h,MR_FULL_D0,0,0); byte(h,CONTEXT_GATE,0); byte(h,CONTEXT_AUX,0);
    word(h,POST_INPUT_COUNTDOWN,5); callback(h,0xc10c68);
}
void leave_menu_return_on_key(const MenuReturnHooks *h) {
    uint8_t mode;
    if((int8_t)read_byte(h,MESSAGE_STATE_C)>=0) {
        uint8_t key=rd_u8(KEY_TAKEN); observe(h,MR_BYTE_TEST,key,0); if(!key) return;
    }
    byte(h,POST_INPUT_AUX,1); byte(h,UPDATE_MASK,0xff); consume(h,MR_SELECT_RESET);
    observe(h,MR_FULL_D0,1,0); byte(h,CONTEXT_STARTED,1); byte(h,CONTEXT_SMOOTH,1);
    byte(h,CONTEXT_GATE,0); mode=read_byte(h,MODE_SELECT); observe(h,MR_COMPARE_BYTE,mode,0x7d);
    if(mode==0x7d) { byte(h,CONTEXT_STATE,6); callback(h,ROUTINE_AFTER_POST_INPUT); }
    else {
        byte(h,REDRAW_FIRST,3); word(h,MENU_RETURN_WORD,0); byte(h,CONTEXT_AUX,0);
        word(h,POST_INPUT_COUNTDOWN,5); callback(h,0xc10c68);
    }
}
