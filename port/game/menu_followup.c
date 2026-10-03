#include "menu_followup.h"
#include "globals.h"
#include <stdlib.h>
/* Original viewport/key callbacks, mode-message owner and table-file owner.
 * Hooks expose actual child calls and the CPU outputs needed by the adapters;
 * the domain decisions below follow the sealed original owner flow. */
static void observe(const MenuFollowupHooks *h,enum MenuFollowupPhase phase,uint32_t value,gaddr address) {
    if(h && h->observe) h->observe(h->context,phase,value,address);
}
static uint32_t consume(const MenuFollowupHooks *h,enum MenuFollowupChild child,uint32_t value,gaddr address) {
    if(!h || !h->consume) abort();
    return h->consume(h->context,child,value,address);
}
static void byte(const MenuFollowupHooks *h,gaddr address,uint8_t value) {
    wr_u8(address,value); observe(h,MF_BYTE_STORE,value,address);
}
static void word(const MenuFollowupHooks *h,gaddr address,uint16_t value) {
    wr_u16(address,value); observe(h,MF_WORD_STORE,value,address);
}
static void callback(const MenuFollowupHooks *h,gaddr value) {
    wr_u32(STAGE_CALLBACK,value); observe(h,MF_CALLBACK,value,0);
}
void poll_menu_viewport(const MenuFollowupHooks *h,int alternate) {
    uint8_t mode=rd_u8(VIEWPORT_MODE),target=rd_u8(VIEWPORT_TARGET);
    observe(h,MF_VIEWPORT_COMPARE,mode,target);
    if(mode!=target) return;
    observe(h,MF_ONE_D0,1,0); byte(h,POST_INPUT_AUX,1); byte(h,UPDATE_MASK,0xff);
    if(alternate) { byte(h,PAUSE_A,1); byte(h,MENU_TRANSITION_FLAG,1); }
    else byte(h,CONTEXT_GATE,1);
    word(h,POST_INPUT_COUNTDOWN,alternate?9:5);
    callback(h,alternate?0xc10458:0xc10678);
}
void follow_menu_key_or_countdown(const MenuFollowupHooks *h) {
    uint8_t off=rd_u8(MODE_MESSAGES_OFF),key=rd_u8(KEY_TAKEN),sequence;
    off|=key; wr_u8(MODE_MESSAGES_OFF,off); observe(h,MF_LATCH,off,key);
    sequence=rd_u8(SEQUENCE_FLAG); observe(h,MF_SEQUENCE_D0,sequence,0);
    if(!sequence) {
        key=rd_u8(KEY_TAKEN); observe(h,MF_BYTE_TEST,key,0);
        if(key) {
            byte(h,POST_INPUT_AUX,1); consume(h,MF_RESET,0,0);
            word(h,POST_INPUT_COUNTDOWN,0xffff); callback(h,0xc105f4); return;
        }
    }
    { uint16_t countdown=rd_u16(POST_INPUT_COUNTDOWN);
      observe(h,MF_WORD_D0,countdown,0);
      if((int16_t)countdown<0) {
          byte(h,MENU_TRANSITION_FLAG,0); word(h,POST_INPUT_COUNTDOWN,1); callback(h,0xc104c2);
      }
    }
}
void advance_menu_mode_messages(const MenuFollowupHooks *h) {
    uint16_t countdown=rd_u16(POST_INPUT_COUNTDOWN),mode;
    uint8_t sequence,off;
    gaddr cursor=MESSAGE_QUEUE;
    observe(h,MF_WORD_D0,countdown,0);
    if((int16_t)countdown>=0) return;
    byte(h,POST_INPUT_AUX,0); sequence=rd_u8(SEQUENCE_FLAG); observe(h,MF_BYTE_TEST,sequence,0);
    if(sequence) {
        byte(h,CONTEXT_REQUEST,1); byte(h,CONTEXT_GATE,2);
        word(h,POST_INPUT_COUNTDOWN,3); callback(h,ROUTINE_OUTCOME); return;
    }
    mode=(uint16_t)(int16_t)(int8_t)rd_u8(MODE_SELECT);
    observe(h,MF_QUEUE_START,mode,cursor);
    off=rd_u8(MODE_MESSAGES_OFF); observe(h,MF_QUEUE_OFF,off,mode);
    if(!off) {
        observe(h,MF_MODE_COMPARE,mode,3);
        if(mode>=3) {
            observe(h,MF_MODE_COMPARE,mode,8);
            if(mode<=8) {
                observe(h,MF_MODE_SUBTRACT,mode,3);
                word(h,cursor,mode==3?0x5f:0x60); observe(h,MF_QUEUE_CODE,0,cursor);
                cursor+=2; observe(h,MF_QUEUE_NEXT,cursor,0);
            }
        }
    }
    byte(h,MODE_MESSAGES_OFF,0);
    word(h,cursor,0x47); observe(h,MF_QUEUE_CODE,0,cursor);
    cursor+=2; observe(h,MF_QUEUE_NEXT,cursor,0);
    word(h,cursor,0); observe(h,MF_QUEUE_CODE,0,cursor);
    callback(h,ROUTINE_QUEUE_MESSAGE_FOUR);
}
void load_menu_mode_file(const MenuFollowupHooks *h) {
    uint16_t status=rd_u16(MENU_TABLE_STATUS),ready;
    uint32_t handle,received;
    observe(h,MF_WORD_D0,status,0);
    if(status) return;
    consume(h,MF_FILE_RELEASE,0,0); byte(h,MODE_TABLE_CHANGED,0);
    status=(uint16_t)consume(h,MF_FILE_CHECK,0,0);
    word(h,MENU_TABLE_STATUS,status); observe(h,MF_FILE_STATUS,status,0);
    if(status) { consume(h,MF_FILE_OWN,0,0); return; }
    consume(h,MF_FILE_YIELD_BEFORE_OPEN,0,0);
    ready=rd_u16(MENU_FILE_READY); observe(h,MF_WORD_D0,ready,0);
    if(!ready) { observe(h,MF_FILE_ZERO,0,0); return; }
    handle=consume(h,MF_FILE_OPEN,0x3ed,MENU_FILE_NAME);
    /* Both values live in the original frame across later Delay calls. */
    observe(h,MF_FILE_HANDLE,handle,0);
    consume(h,MF_FILE_YIELD_AFTER_OPEN,0,0); observe(h,MF_FILE_HANDLE_TEST,handle,0);
    if((int32_t)handle<=0) {
        word(h,MENU_FILE_READY,0); observe(h,MF_FILE_ZERO,0,0); return;
    }
    received=consume(h,MF_FILE_READ,handle,rd_u32(MODE_TABLE));
    observe(h,MF_FILE_READ_RESULT,received,0);
    consume(h,MF_FILE_YIELD_AFTER_READ,0,0);
    consume(h,MF_FILE_CLOSE,handle,0); consume(h,MF_FILE_YIELD_AFTER_CLOSE,0,0);
    observe(h,MF_FILE_COMPARE,received,0);
    /* The original rejects only -1. Zero and short reads still take OwnBlitter. */
    if(received==0xffffffffu) { observe(h,MF_FILE_ZERO,0,0); return; }
    consume(h,MF_FILE_OWN,0,0);
}
