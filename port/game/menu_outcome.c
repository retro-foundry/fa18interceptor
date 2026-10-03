#include "menu_outcome.h"
#include "menu_setup.h"
#include "globals.h"
#include "stages.h"
#include <stdlib.h>
/* Source-owned delayed menu selection and outcome callbacks. CPU adaptation
 * and resumable instruction timing are separate from these game decisions. */
static void observe(const MenuOutcomeHooks *h,enum MenuOutcomePhase phase,uint32_t value,gaddr address) {
    if(h && h->observe) h->observe(h->context,phase,value,address);
}
static void consume(const MenuOutcomeHooks *h,enum MenuOutcomeChild child,uint32_t value) {
    if(!h || !h->consume) abort();
    h->consume(h->context,child,value);
}
static void byte(const MenuOutcomeHooks *h,gaddr address,uint8_t value) {
    wr_u8(address,value); observe(h,MO_BYTE_STORE,value,address);
}
static void word(const MenuOutcomeHooks *h,gaddr address,uint16_t value) {
    wr_u16(address,value); observe(h,MO_WORD_STORE,value,address);
}
static void callback(const MenuOutcomeHooks *h,gaddr value) {
    wr_u32(STAGE_CALLBACK,value); observe(h,MO_CALLBACK,value,0);
}
static int expired(const MenuOutcomeHooks *h) {
    uint16_t count=rd_u16(POST_INPUT_COUNTDOWN);
    observe(h,MO_WORD_D0,count,0); return (int16_t)count<0;
}
static void latch(const MenuOutcomeHooks *h) {
    uint8_t off=rd_u8(MODE_MESSAGES_OFF),key=rd_u8(KEY_TAKEN);
    off|=key; wr_u8(MODE_MESSAGES_OFF,off); observe(h,MO_LATCH,off,key);
}
static void countdown_outputs(void *context,enum MenuSetupPhase phase,uint32_t value,gaddr address) {
    const MenuOutcomeHooks *h=context;
    if(phase==MENU_SETUP_BYTE_STORE) observe(h,MO_BYTE_STORE,value,address);
    else if(phase==MENU_SETUP_WORD_STORE) observe(h,MO_WORD_STORE,value,address);
    else if(phase==MENU_SETUP_TAIL_CALLBACK) observe(h,MO_CALLBACK,value,address);
}
void select_delayed_menu_message(const MenuOutcomeHooks *h) {
    uint8_t sequence,key,enabled; uint16_t mode;
    observe(h,MO_FULL_D0,0,0); byte(h,POST_INPUT_EVENT,0); latch(h);
    sequence=rd_u8(SEQUENCE_FLAG); observe(h,MO_BYTE_D0,sequence,0);
    if(!sequence) {
        key=rd_u8(KEY_TAKEN); observe(h,MO_BYTE_TEST,key,0);
        if(key) {
            byte(h,POST_INPUT_AUX,1); consume(h,MO_DELAYED_RESET,0);
            word(h,POST_INPUT_COUNTDOWN,0xffff); callback(h,0xc105f4); return;
        }
    }
    if(!expired(h)) return;
    byte(h,POST_INPUT_AUX,0);
    mode=(uint16_t)(int16_t)(int8_t)rd_u8(MODE_SELECT); observe(h,MO_MODE_LOCAL,mode,0);
    observe(h,MO_MODE_COMPARE,mode,3);
    if(mode>=3) {
        observe(h,MO_MODE_COMPARE,mode,8);
        if(mode<=8) {
            word(h,MESSAGE_QUEUE,0x5b);
            enabled=rd_u8(rd_u32(MODE_TABLE)+0x12+mode);
            observe(h,MO_SELECTED_MODE,enabled,mode);
            consume(h,enabled?MO_DELAYED_MESSAGE_ENABLED:MO_DELAYED_MESSAGE_DISABLED,mode);
        }
    }
    callback(h,0xc105a6);
}
void pause_menu_after_countdown(const MenuOutcomeHooks *h) {
    if(!expired(h)) return;
    observe(h,MO_FULL_D0,1,0); byte(h,PAUSE_A,1); byte(h,MENU_TRANSITION_FLAG,1);
    word(h,POST_INPUT_COUNTDOWN,1); consume(h,MO_PAUSE_SCENE,0); callback(h,0xc10626);
}
void queue_menu_message_four(const MenuOutcomeHooks *h) {
    uint8_t state=rd_u8(MESSAGE_STATE_C);
    observe(h,MO_BYTE_D0,state,0); observe(h,MO_MESSAGE_SUBTRACT,state,0);
    if((uint8_t)(state-1u)) return;
    word(h,MESSAGE_QUEUE,4); byte(h,MESSAGE_STATE_B,0); byte(h,MESSAGE_STATE_C,3);
    callback(h,ROUTINE_START_OUTCOME);
}
void finish_menu_outcome(const MenuOutcomeHooks *h) {
    uint16_t mode; uint8_t request;
    if(!expired(h)) return;
    mode=(uint16_t)(int16_t)(int8_t)rd_u8(MODE_SELECT);
    request=rd_u8(CONTEXT_REQUEST); observe(h,MO_CONTEXT_MODE,mode,request);
    if((int8_t)request>0) {
        callback(h,0xc10900); observe(h,MO_MODE_COMPARE,mode,3);
        if(mode>=3) {
            observe(h,MO_MODE_COMPARE,mode,8);
            if(mode<=8) {
                uint16_t count; gaddr table;
                word(h,MESSAGE_QUEUE,0x5d); consume(h,MO_OUTCOME_MESSAGE,mode);
                /* The actual message child may replace the mode-table pointer. */
                table=rd_u32(MODE_TABLE); count=rd_u16(table+0x36);
                observe(h,MO_TABLE_READ,count,table); observe(h,MO_TABLE_ADD,1,0);
                table=rd_u32(MODE_TABLE); observe(h,MO_TABLE_STORE,0,table);
                word(h,table+0x36,(uint16_t)(count+1u)); byte(h,MODE_TABLE_CHANGED,1); return;
            }
        }
        /* Read and search the original keys in reverse, as the source does.
         * The sealed branch instruction for each record determines its arm. */
        observe(h,MO_SEARCH_BEGIN,mode,0);
        for(int index=24;;index-=8) {
            uint32_t key;
            observe(h,MO_SEARCH_SUBTRACT,8,0); if(index<0) return;
            key=rd_u32(MENU_OUTCOME_CASE_TABLE+(gaddr)index);
            observe(h,MO_SEARCH_COMPARE,key,mode); if(key!=mode) continue;
            if(index==24) callback(h,0xc10970);
            else if(index) callback(h,0xc102d8);
            else {
                MenuSetupHooks countdown={NULL,countdown_outputs,(void *)h};
                begin_menu_countdown(&countdown);
            }
            return;
        }
    }
    request=rd_u8(CONTEXT_REQUEST); observe(h,MO_BYTE_D0,request,0);
    if((int8_t)request>=0) return;
    byte(h,CONTEXT_REQUEST,0); byte(h,POST_INPUT_AUX,1); consume(h,MO_OUTCOME_RESET,0);
    word(h,POST_INPUT_COUNTDOWN,3);
    request=rd_u8(ATTEMPTS_LEFT); observe(h,MO_BYTE_D0,request,0);
    observe(h,MO_ATTEMPT_SUBTRACT,request,0); request=(uint8_t)(request-1u);
    byte(h,ATTEMPTS_LEFT,request); observe(h,MO_BYTE_TEST,request,0);
    callback(h,(int8_t)request<0?0xc108da:ROUTINE_MODE_FOUR);
}
void leave_delayed_menu_message(const MenuOutcomeHooks *h) {
    uint8_t state=rd_u8(MESSAGE_STATE_C),key;
    observe(h,MO_BYTE_D0,state,0);
    if((int8_t)state>=0) {
        key=rd_u8(KEY_TAKEN); observe(h,MO_BYTE_TEST,key,0); if(!key) return;
    }
    latch(h); byte(h,POST_INPUT_AUX,1); consume(h,MO_MESSAGE_RESET,0);
    word(h,POST_INPUT_COUNTDOWN,0xffff); byte(h,REDRAW_FIRST,3); callback(h,0xc105f4);
}
void start_menu_context_after_countdown(const MenuOutcomeHooks *h) {
    if(!expired(h)) return;
    byte(h,CONTEXT_STATE,2); byte(h,CONTEXT_SMOOTH,1); callback(h,0xc1064c);
}
void start_menu_outcome(const MenuOutcomeHooks *h) {
    uint8_t request=rd_u8(CONTEXT_REQUEST),selected;
    observe(h,MO_BYTE_TEST,request,0); if(!request) return;
    selected=rd_u8(CONTEXT_SELECT); observe(h,MO_BYTE_TEST,selected,0);
    if(selected) byte(h,CONTEXT_GATE,2);
    if(h) consume(h,MO_COUNTDOWN_RESET,0); else reset_message_sequence();
    word(h,POST_INPUT_COUNTDOWN,3); callback(h,ROUTINE_OUTCOME);
}
void queue_menu_attempts_exhausted(const MenuOutcomeHooks *h) {
    if(!expired(h)) return;
    byte(h,POST_INPUT_AUX,0); word(h,MESSAGE_QUEUE,0x43); callback(h,0xc108fe);
}
