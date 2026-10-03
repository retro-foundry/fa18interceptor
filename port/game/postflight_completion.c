/* Complete C11788-C119D4, C1104C and C0F946/C0F974 callback owners. */
#include "postflight_completion.h"
#include "globals.h"
#include "stages.h"
#include "scene_setup.h"
#include "render_buffers.h"
#include <stdlib.h>
static void observe(const PostflightCompletionHooks *h,enum PostflightCompletionPhase p,uint32_t v,uint32_t other) {
    if(h && h->observe) h->observe(h->context,p,v,other);
}
static void consume(const PostflightCompletionHooks *h,enum PostflightCompletionChild child) {
    if(h && h->consume) { h->consume(h->context,child); return; }
    switch(child) {
    case PFC_RESET_SCENE: case PFC_RESTART_SCENE: place_scene_root(); return;
    case PFC_CLEAR_RENDER: clear_render_buffers(); return;
    case PFC_VIEW_ZERO: start_view_mode_zero(0); return;
    case PFC_LOAD_TABLE: load_long_table(0xc08490); return;
    }
    abort();
}
static uint8_t read_byte(const PostflightCompletionHooks *h,gaddr a) {
    uint8_t v=rd_u8(a); observe(h,PFC_D0_BYTE,v,0); return v;
}
static uint16_t read_word(const PostflightCompletionHooks *h,gaddr a) {
    uint16_t v=rd_u16(a); observe(h,PFC_D0_WORD,v,0); return v;
}
static void byte(const PostflightCompletionHooks *h,gaddr a,uint8_t v) { wr_u8(a,v); observe(h,PFC_BYTE_STORE,v,0); }
static void word(const PostflightCompletionHooks *h,gaddr a,uint16_t v) { wr_u16(a,v); observe(h,PFC_WORD_STORE,v,0); }
static void callback(const PostflightCompletionHooks *h,gaddr entry,int lea) {
    wr_u32(STAGE_CALLBACK,entry); observe(h,lea?PFC_CALLBACK_LEA:PFC_CALLBACK_DIRECT,entry,0);
}
static int expired(const PostflightCompletionHooks *h) {
    uint16_t v=read_word(h,POST_INPUT_COUNTDOWN); observe(h,PFC_TEST_WORD,v,0); return (int16_t)v<0;
}
static int message_finished(const PostflightCompletionHooks *h) {
    uint8_t v=read_byte(h,MESSAGE_STATE_C); observe(h,PFC_TEST_BYTE,v,0); return (int8_t)v<0;
}
static uint16_t mask(const PostflightCompletionHooks *h,gaddr address,uint16_t v,uint16_t bits,int set) {
    v=set?(uint16_t)(v|bits):(uint16_t)(v&bits); observe(h,set?PFC_OR_WORD:PFC_AND_WORD,bits,0);
    word(h,address,v); return v;
}
static void reset_message_flags(const PostflightCompletionHooks *h,gaddr next) {
    uint16_t v=read_word(h,COCKPIT_FLAGS);
    v=mask(h,COCKPIT_FLAGS,v,0xffbf,0); mask(h,COCKPIT_FLAGS,v,0x9fff,0);
    word(h,POST_INPUT_COUNTDOWN,0x64); callback(h,next,1);
}
void advance_postflight_completion(const PostflightCompletionHooks *h) {
    uint8_t selected=read_byte(h,CONTEXT_SELECT),v; uint16_t flags;
    observe(h,PFC_TEST_BYTE,selected,0);
    if(!selected) {
        v=read_byte(h,PLAYER_FLAGS_A); observe(h,PFC_TEST_BYTE,v,0);
        if(v) { selected=rd_u8(CONTEXT_SELECT); observe(h,PFC_TEST_BYTE,selected,0); if(!selected) return; }
    } else { selected=rd_u8(CONTEXT_SELECT); observe(h,PFC_TEST_BYTE,selected,0); if(!selected) return; }
    if(selected) { flags=read_word(h,CONTROL_RECORDS); observe(h,PFC_BIT_WORD,flags,10); if(flags&0x400) return; }
    flags=read_word(h,COCKPIT_FLAGS); flags=mask(h,COCKPIT_FLAGS,flags,0x9fff,0);
    mask(h,COCKPIT_FLAGS,flags,0xfffe,0); consume(h,PFC_RESET_SCENE);
    byte(h,VIEWPORT_TARGET,15); observe(h,PFC_D0_ZERO,0,0); byte(h,PLAYER_FLAGS_E,0);
    flags=read_word(h,PLAYER_STATUS_D4); mask(h,PLAYER_STATUS_D4,flags,0xfbff,0);
    v=read_byte(h,POSTFLIGHT_RESET_REMAINING); observe(h,PFC_SUB_BYTE,1,0); v=(uint8_t)(v-1);
    byte(h,POSTFLIGHT_RESET_REMAINING,v); observe(h,PFC_TEST_BYTE,v,0);
    if((int8_t)v>0) callback(h,0xc11830,1);
    else {
        consume(h,PFC_CLEAR_RENDER); word(h,POST_INPUT_COUNTDOWN,5); byte(h,POST_INPUT_AUX,0);
        callback(h,0xc118a0,1);
    }
}
void restart_postflight_completion(const PostflightCompletionHooks *h) {
    uint16_t flags; uint8_t selected;
    observe(h,PFC_D0_ZERO,0,0); byte(h,PLAYER_FLAGS_B,0);
    flags=read_word(h,CONTROL_RECORDS+2); mask(h,CONTROL_RECORDS+2,flags,2,1);
    selected=read_byte(h,CONTEXT_SELECT); observe(h,PFC_TEST_BYTE,selected,0);
    if(!selected) consume(h,PFC_VIEW_ZERO);
    consume(h,PFC_RESTART_SCENE); word(h,POST_INPUT_COUNTDOWN,5); callback(h,0xc11872,1);
}
void expire_postflight_completion(const PostflightCompletionHooks *h) {
    uint16_t flags;
    if(!expired(h)) return;
    flags=read_word(h,COCKPIT_FLAGS); mask(h,COCKPIT_FLAGS,flags,0x40,1);
    byte(h,FIRE_STATE,0xfe); callback(h,STAGE_AFTER_EXPIRY,0);
}
void queue_postflight_failure(const PostflightCompletionHooks *h) {
    uint8_t input;
    if(!expired(h)) return;
    consume(h,PFC_LOAD_TABLE); input=read_byte(h,POSTFLIGHT_FAILURE_INPUT);
    observe(h,PFC_COMPARE_BYTE,input,0x10); word(h,MESSAGE_QUEUE,input==0x10?0x62:0x63);
    byte(h,SEQUENCE_FLAG,0); callback(h,0xc118e6,1);
}
void end_postflight_message(const PostflightCompletionHooks *h) {
    if(message_finished(h)) callback(h,0xc0f920,0);
}
void follow_postflight_message(const PostflightCompletionHooks *h) {
    if(message_finished(h)) reset_message_flags(h,0xc11934);
}
void clear_postflight_phase(const PostflightCompletionHooks *h) {
    if(!expired(h)) return;
    observe(h,PFC_D0_ZERO,0,0); byte(h,SEQUENCE_PHASE,0); byte(h,PLAYER_PHASE,0); callback(h,0xc0f920,0);
}
void follow_postflight_message_or_phase(const PostflightCompletionHooks *h) {
    uint8_t phase;
    if(message_finished(h)) { reset_message_flags(h,0xc119d4); return; }
    phase=read_byte(h,SEQUENCE_PHASE); observe(h,PFC_COMPARE_BYTE,phase,0xff);
    if(phase==0xff) {
        observe(h,PFC_D0_ZERO,0,0); byte(h,SEQUENCE_PHASE,0); byte(h,SEQUENCE_FLAG,0); callback(h,0xc0f920,0); return;
    }
    phase=read_byte(h,SEQUENCE_PHASE); observe(h,PFC_SUB_BYTE,1,0);
    if((uint8_t)(phase-1)) return;
    word(h,POST_INPUT_COUNTDOWN,0xffff); callback(h,0xc119d4,1);
}
void restart_postflight_after_countdown(const PostflightCompletionHooks *h) {
    uint8_t requested;
    if(!expired(h)) return;
    observe(h,PFC_D0_ZERO,0,0); byte(h,SEQUENCE_PHASE,0); byte(h,PLAYER_PHASE,0);
    requested=rd_u8(CONTEXT_REQUEST); observe(h,PFC_TEST_BYTE,requested,0);
    if(requested) byte(h,SEQUENCE_FLAG,1);
    byte(h,POST_INPUT_EVENT,1); word(h,POST_INPUT_COUNTDOWN,3);
    observe(h,PFC_D0_ZERO,0,0); byte(h,POST_INPUT_AUX,0); byte(h,VIEWPORT_TARGET,0); callback(h,0xc0f946,0);
}
void queue_postflight_end(const PostflightCompletionHooks *h) {
    if(!expired(h)) return;
    observe(h,PFC_D0_ZERO,0,0); byte(h,POST_INPUT_AUX,0); word(h,MESSAGE_QUEUE,14);
    byte(h,MESSAGE_STATE_C,0); callback(h,0xc118fc,0);
}
void await_postflight_viewport(const PostflightCompletionHooks *h) {
    uint8_t mode,target;
    if(!expired(h)) return;
    mode=read_byte(h,VIEWPORT_MODE); target=rd_u8(VIEWPORT_TARGET); observe(h,PFC_D1_BYTE,target,0);
    observe(h,PFC_VIEWPORT_COMPARE,mode,target); if(mode!=target) return;
    word(h,POST_INPUT_COUNTDOWN,2); callback(h,0xc0f974,1);
}
void mark_postflight_viewport_ready(const PostflightCompletionHooks *h) {
    if(!expired(h)) return;
    byte(h,POST_INPUT_AUX,1); callback(h,0xc0f992,1);
}
