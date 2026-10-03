#include "menu_context_finish.h"
#include "globals.h"
#include "audio.h"
#include "target_heading.h"
#include <stdlib.h>

static void observe(const MenuContextHooks *h,enum MenuContextPhase phase,uint32_t value,gaddr address) {
    if(h && h->observe) h->observe(h->context,phase,value,address);
}
static int32_t consume(const MenuContextHooks *h,enum MenuContextChild child) {
    if(h && h->consume) return h->consume(h->context,child);
    if(child==MC_HEADING) return refresh_post_input_heading();
    if(child==MC_EXPIRY_TONE) { play_tone_2(); return 0; }
    abort();
}
static void byte(const MenuContextHooks *h,gaddr address,uint8_t value) {
    wr_u8(address,value); observe(h,MC_BYTE_STORE,value,address);
}
static void word(const MenuContextHooks *h,gaddr address,uint16_t value) {
    wr_u16(address,value); observe(h,MC_WORD_STORE,value,address);
}
static void longword(const MenuContextHooks *h,gaddr address,uint32_t value) {
    wr_u32(address,value); observe(h,MC_LONG_STORE,value,address);
}
static uint8_t read_byte(const MenuContextHooks *h,gaddr address) {
    uint8_t value=rd_u8(address); observe(h,MC_D0_BYTE,value,0); return value;
}
static uint16_t read_word(const MenuContextHooks *h,gaddr address) {
    uint16_t value=rd_u16(address); observe(h,MC_D0_WORD,value,0); return value;
}
static uint32_t read_long(const MenuContextHooks *h,gaddr address) {
    uint32_t value=rd_u32(address); observe(h,MC_D0_LONG,value,0); return value;
}
static uint8_t subtract(const MenuContextHooks *h,uint8_t value,uint8_t amount) {
    observe(h,MC_SUB_BYTE,value,amount); return (uint8_t)(value-amount);
}
static int test_byte(const MenuContextHooks *h,gaddr address) {
    uint8_t value=rd_u8(address); observe(h,MC_BYTE_TEST,value,0); return value!=0;
}
static int expired(const MenuContextHooks *h) {
    uint16_t value=read_word(h,POST_INPUT_COUNTDOWN);
    observe(h,MC_WORD_TEST,value,0); return (int16_t)value<0;
}
static void callback(const MenuContextHooks *h,gaddr entry) {
    wr_u32(STAGE_CALLBACK,entry); observe(h,MC_CALLBACK,entry,0);
}
static uint16_t mask_word(const MenuContextHooks *h,gaddr address,uint16_t mask,int set) {
    uint16_t value=read_word(h,address);
    observe(h,set?MC_WORD_OR_D0:MC_WORD_AND_D0,mask,0);
    value=set?(uint16_t)(value|mask):(uint16_t)(value&mask);
    word(h,address,value); return value;
}
void follow_menu_smoothing(const MenuContextHooks *h) {
    if(!subtract(h,read_byte(h,KEY_TAKEN),2)) { consume(h,MC_CANCEL); return; }
    if(!test_byte(h,CONTEXT_SMOOTH)) {
        if(!subtract(h,read_byte(h,MESSAGE_STATE_C),1)) byte(h,POST_INPUT_AUX,1);
        return;
    }
    byte(h,CONTEXT_SMOOTH,0); consume(h,MC_SMOOTH_RESET);
    observe(h,MC_D0_LONG,0,0); byte(h,PAUSE_A,0);
    observe(h,MC_D0_LONG,3,0); byte(h,REDRAW_FIRST,3);
    observe(h,MC_D0_LONG,1,0); byte(h,POST_INPUT_AUX,1); word(h,POST_INPUT_COUNTDOWN,2);
    if(subtract(h,read_byte(h,SCENE_POSE_ENTRY),3)) {
        callback(h,0xc10ab2); byte(h,COMMAND_ENABLE_GATE,1);
        byte(h,MENU_TRANSITION_FLAG,0); byte(h,CONTEXT_STATE,6);
    } else callback(h,0xc10b1e);
}
void queue_menu_smoothing_message(const MenuContextHooks *h) {
    observe(h,MC_D0_LONG,0,0); byte(h,CONTEXT_SMOOTH,0);
    if(!expired(h)) return;
    observe(h,MC_D0_LONG,0,0); byte(h,POST_INPUT_AUX,0);
    word(h,MESSAGE_QUEUE,0x4b); byte(h,MESSAGE_STATE_B,0); callback(h,0xc10ae6);
}
void restart_menu_smoothing(const MenuContextHooks *h) {
    uint8_t gate=read_byte(h,COMMAND_ENABLE_GATE); observe(h,MC_BYTE_TEST,gate,0);
    if(gate) return;
    word(h,POST_INPUT_COUNTDOWN,2); byte(h,POST_INPUT_AUX,1); consume(h,MC_RESTART_RESET);
    observe(h,MC_D0_LONG,1,0); byte(h,MENU_TRANSITION_FLAG,1); byte(h,CONTEXT_GATE,1); callback(h,0xc10b1e);
}
void reset_menu_smoothing_view(const MenuContextHooks *h) {
    uint8_t mode,flags;
    if(!expired(h)) return;
    byte(h,CONTEXT_SMOOTH,1); consume(h,MC_REFRESH_VIEW); consume(h,MC_PRESET_POSITION);
    observe(h,MC_D0_LONG,0,0); byte(h,ORIGIN_ADJUSTMENT_MODE,0);
    byte(h,CONTEXT_STATE,6); byte(h,CONTEXT_GATE,0); mode=read_byte(h,MODE_SELECT);
    observe(h,MC_CMP_BYTE,mode,3);
    if((int8_t)mode>=3) {
        observe(h,MC_CMP_BYTE,mode,8);
        if((int8_t)mode<=8) {
            flags=rd_u8(SOUND_FLAGS); observe(h,MC_BIT_BYTE,flags,4);
            observe(h,MC_D0_LONG,8,0); consume(h,(flags&16)?MC_NOISE:MC_ENGINE);
        }
    }
    callback(h,ROUTINE_AFTER_POST_INPUT);
}
void begin_menu_context(const MenuContextHooks *h) {
    uint8_t selected=read_byte(h,CONTEXT_SELECT); observe(h,MC_BYTE_TEST,selected,0);
    if(!selected) {
        byte(h,CONTEXT_STARTED,1); observe(h,MC_D0_LONG,0,0);
        byte(h,CONTEXT_STATE,0); byte(h,CONTEXT_GATE,0); byte(h,CONTEXT_AUX,0);
        word(h,POST_INPUT_COUNTDOWN,5); callback(h,ROUTINE_CONTEXT_STAGE);
    } else {
        uint8_t event=read_byte(h,POST_INPUT_EVENT); observe(h,MC_BYTE_TEST,event,0);
        if((int8_t)event<0) {
            word(h,POST_INPUT_COUNTDOWN,2); byte(h,VIEWPORT_TARGET,10);
            longword(h,STAGE_CALLBACK,0xc11a26);
        }
    }
}
typedef struct { gaddr frame,cursor; uint8_t mode; } QueueLocals;
static gaddr queue_cursor(const QueueLocals *local) { return local->frame?rd_u32(local->frame-4):local->cursor; }
static void queue_word(QueueLocals *local,const MenuContextHooks *h,uint16_t code,int advance) {
    gaddr cursor=queue_cursor(local);
    wr_u16(cursor,code); observe(h,MC_QUEUE_WORD,code,cursor);
    if(advance) {
        observe(h,MC_QUEUE_NEXT,cursor,2); local->cursor=cursor+2;
        if(local->frame) wr_u32(local->frame-4,local->cursor);
    }
}
void queue_menu_context_command(gaddr frame,const MenuContextHooks *h) {
    QueueLocals local={frame,MESSAGE_QUEUE,0}; uint8_t mode,flags; int32_t result;
    if(!expired(h)) return;
    mode=read_byte(h,MODE_SELECT); byte(h,POST_INPUT_AUX,0);
    if(frame) wr_u32(frame-4,local.cursor); observe(h,MC_LOCAL_CURSOR,local.cursor,0);
    local.mode=mode; if(frame) wr_u8(frame-5,mode); observe(h,MC_LOCAL_BYTE,mode,0);
    observe(h,MC_CMP_BYTE,mode,3);
    if((int8_t)mode>=3) {
        observe(h,MC_CMP_BYTE,mode,8);
        if((int8_t)mode<=8) {
            result=consume(h,MC_HEADING); observe(h,MC_LONG_TEST,(uint32_t)result,0);
            if(result>=0) queue_word(&local,h,0x58,1);
        }
    }
    mode=frame?rd_u8(frame-5):local.mode; observe(h,MC_CMP_BYTE,mode,2);
    if(mode==2) byte(h,MESSAGE_STATE_C,0xff);
    else {
        flags=read_byte(h,CONTROL_RECORDS+4); observe(h,MC_BIT_BYTE,flags,3);
        queue_word(&local,h,(flags&8)?0x5a:0x59,1);
    }
    queue_word(&local,h,0,0); callback(h,0xc10cfe);
}
void finish_menu_context_message(const MenuContextHooks *h) {
    uint8_t state=read_byte(h,MESSAGE_STATE_C); observe(h,MC_BYTE_TEST,state,0);
    if((int8_t)state>=0) return;
    observe(h,MC_D0_LONG,1,0); byte(h,POST_INPUT_AUX,1);
    mask_word(h,COCKPIT_FLAGS,0x40,1); mask_word(h,MESSAGE_STATE,0x100,1);
    mask_word(h,UPDATE_DISPLAY_FLAGS,2,1);
    if(!subtract(h,read_byte(h,MODE_SELECT),2)) byte(h,TEXT_ALWAYS,1);
    consume(h,MC_MESSAGE_TIME); longword(h,MENU_TIME_SAVED,read_long(h,READOUT_SAMPLE));
    longword(h,MENU_TIME_OPTIONAL,rd_u32(READOUT_SAMPLE));
    word(h,STREAM_SKIP,0); word(h,POST_INPUT_COUNTDOWN,3); callback(h,0xc10d8a);
}
void expire_menu_context(const MenuContextHooks *h) {
    if(!expired(h)) return;
    byte(h,POST_INPUT_EXPIRED,1); consume(h,MC_EXPIRY_TONE); callback(h,STAGE_AFTER_EXPIRY);
}
void queue_menu_viewport_message(const MenuContextHooks *h) {
    uint8_t mode,target;
    byte(h,POST_INPUT_AUX,0); mode=rd_u8(VIEWPORT_MODE); target=rd_u8(VIEWPORT_TARGET);
    observe(h,MC_VIEWPORT_COMPARE,mode,target); if(mode!=target) return;
    word(h,MESSAGE_QUEUE,0x26); callback(h,0xc11a50);
}
void finish_menu_viewport_message(const MenuContextHooks *h) {
    uint8_t event=read_byte(h,POST_INPUT_EVENT); uint32_t previous,sample,delta,total;
    observe(h,MC_BYTE_TEST,event,0); if((int8_t)event<0) return;
    byte(h,POST_INPUT_AUX,1); consume(h,MC_VIEWPORT_RESET); byte(h,VIEWPORT_TARGET,15);
    if(!subtract(h,read_byte(h,CONTEXT_STATE),6)) { longword(h,STAGE_CALLBACK,ROUTINE_AFTER_POST_INPUT); return; }
    consume(h,MC_VIEWPORT_TIME); previous=read_long(h,MENU_TIME_PENDING);
    sample=rd_u32(READOUT_SAMPLE); observe(h,MC_D1_LONG,sample,0);
    delta=sample-previous; observe(h,MC_SUB_LONG_D1,previous,0);
    longword(h,MENU_TIME_PENDING,delta); total=rd_u32(MENU_TIME_TOTAL)+delta;
    observe(h,MC_ADD_MEMORY_D1,rd_u32(MENU_TIME_TOTAL),0); wr_u32(MENU_TIME_TOTAL,total);
    total=rd_u32(MENU_TIME_OPTIONAL); observe(h,MC_LONG_TEST,total,0);
    if(total) {
        delta=read_long(h,MENU_TIME_PENDING); observe(h,MC_ADD_MEMORY_D0,total,0);
        wr_u32(MENU_TIME_OPTIONAL,total+delta);
    }
    longword(h,MENU_TIME_PENDING,0); longword(h,STAGE_CALLBACK,STAGE_AFTER_EXPIRY);
}
void load_menu_position_preset(const MenuContextHooks *h) {
    uint32_t result[3]; unsigned i;
    observe(h,MC_PRESET_INPUT,0,0); consume(h,MC_POSITION_TRANSFORM);
    if(!h || !h->position_result) abort(); h->position_result(h->context,result);
    for(i=0;i<3;++i) wr_u32(ORIGIN_CANDIDATE_TRIPLE+4*i,result[i]);
}
void read_menu_time_sample(const MenuContextHooks *h) {
    byte(h,MENU_TIME_REQUEST+8,5); byte(h,MENU_TIME_REQUEST+9,0);
    observe(h,MC_TIMER_ZERO,0,0); longword(h,MENU_TIME_REQUEST+10,0);
    longword(h,MENU_TIME_REQUEST+14,0); word(h,MENU_TIME_REQUEST+28,10);
    consume(h,MC_TIMER_REQUEST); longword(h,READOUT_SAMPLE,rd_u32(MENU_TIME_REQUEST+32));
    longword(h,READOUT_SAMPLE+4,rd_u32(MENU_TIME_REQUEST+36));
}
void update_menu_context(gaddr frame,const MenuContextHooks *h) {
    uint8_t phase=rd_u8(PLAYER_PHASE),value,selected; uint16_t status,offset;
    uint8_t saved=phase;
    if(frame) wr_u8(frame-1,phase); observe(h,MC_LOCAL_BYTE,phase,0);
    status=read_word(h,CONTROL_RECORDS); observe(h,MC_BIT_WORD,status,9);
    if(status&0x200) {
        value=read_byte(h,COMMAND_BLOCK_FLAGS); value&=15; observe(h,MC_D0_BYTE,value,0);
        observe(h,MC_BYTE_TEST,value,0);
        if(!value) {
            if(test_byte(h,MENU_CONTEXT_FLAG)) {
                gaddr table=rd_u32(MODE_TABLE); observe(h,MC_TABLE_POINTER,table,0);
                offset=rd_u16(table+0x10); observe(h,MC_D0_WORD,offset,0);
                observe(h,MC_ADD_WORD_D0,1,0); offset=(uint16_t)(offset+1);
                table=rd_u32(MODE_TABLE); observe(h,MC_TABLE_POINTER,table,0); word(h,table+0x10,offset);
                observe(h,MC_D0_LONG,1,0); byte(h,0xc457c5,1);
                mask_word(h,STATUS_CA,0xfeff,0); mask_word(h,COCKPIT_FLAGS,0xffbf,0);
                mask_word(h,CONTROL_RECORDS,0xfdff,0);
                selected=read_byte(h,CONTEXT_SELECT); observe(h,MC_BYTE_TEST,selected,0);
                if(!selected) {
                    byte(h,PLAYER_FLAGS_B,1); longword(h,0xc461c6,0); byte(h,PLAYER_FLAGS_A,0x28);
                    consume(h,MC_STAGE_SETUP); observe(h,MC_D0_LONG,0x78,0); consume(h,MC_STAGE_SOUND);
                }
                longword(h,STAGE_CALLBACK,0xc11788); return;
            }
            mask_word(h,CONTROL_RECORDS,0xfdff,0); consume(h,MC_STAGE_COMMAND); return;
        }
    }
    value=read_byte(h,POST_INPUT_EVENT); observe(h,MC_BYTE_TEST,value,0);
    if((int8_t)value<0) {
        word(h,POST_INPUT_COUNTDOWN,2); byte(h,VIEWPORT_TARGET,10); consume(h,MC_STAGE_TIME);
        longword(h,MENU_TIME_PENDING,rd_u32(READOUT_SAMPLE)); longword(h,STAGE_CALLBACK,0xc11a26); return;
    }
    phase=frame?rd_u8(frame-1):saved; observe(h,MC_D0_BYTE,phase,0); observe(h,MC_CMP_BYTE,phase,0xff);
    if((int8_t)phase<=-1) {
        observe(h,MC_CMP_BYTE,phase,0xfc);
        if((int8_t)phase>=-4 && test_byte(h,MESSAGE_STATE_B) && test_byte(h,MESSAGE_STATE_C)) {
            observe(h,MC_CMP_BYTE,phase,0xff);
            if(phase==0xff) byte(h,PLAYER_PHASE,1);
            else {
                observe(h,MC_CMP_BYTE,phase,0xfe);
                if(phase==0xfe) byte(h,PLAYER_PHASE,2);
                else { observe(h,MC_CMP_BYTE,phase,0xfc); if(phase==0xfc) byte(h,PLAYER_PHASE,4); }
            }
            byte(h,POST_INPUT_AUX,1); consume(h,MC_STAGE_RESET);
            observe(h,MC_D0_LONG,0,0); byte(h,CONTEXT_GATE,0); byte(h,FIRE_STATE,0xfe);
            byte(h,UPDATE_MASK,0xff); byte(h,POST_INPUT_EVENT,0);
            offset=rd_u16(TARGET_RECORD); observe(h,MC_WORD_TEST,offset,0); if(!offset) return;
            observe(h,MC_D0_LONG,0,0); word(h,TARGET_RECORD,0); word(h,VIEW_RECORD,0);
            selected=read_byte(h,MENU_CONTEXT_SAVED_SELECT); byte(h,CONTEXT_SELECT,selected);
            observe(h,MC_BYTE_TEST,selected,0);
            if(selected) { word(h,SPAN_ORIGIN,0x32); word(h,LINE_LAST_ROW,0xa7); }
            else { word(h,SPAN_ORIGIN,0); word(h,LINE_LAST_ROW,0x90); }
            byte(h,VIEW_MODE,0); offset=read_word(h,SPAN_ORIGIN);
            observe(h,MC_SHIFT_WORD_D0,4,0); word(h,SPAN_ORIGIN_Y,(uint16_t)(offset<<4));
            consume(h,MC_STAGE_FINISH); value=read_byte(h,SCHEDULE_SAVED_VIEW); value&=0x7f;
            observe(h,MC_D0_BYTE,value,0); byte(h,VIEW_SIDE,value); return;
        }
    }
    value=read_byte(h,SEQUENCE_PHASE); observe(h,MC_CMP_BYTE,value,0xff);
    if(value==0xff) {
        byte(h,SEQUENCE_PHASE,0); byte(h,MESSAGE_STATE_C,0xff); longword(h,STAGE_CALLBACK,0xc118fc); return;
    }
    phase=frame?rd_u8(frame-1):saved; observe(h,MC_CMP_BYTE,phase,0xf0);
    if(phase==0xf0) { longword(h,STAGE_CALLBACK,0xc118fc); return; }
    observe(h,MC_CMP_BYTE,phase,0xef);
    if(phase==0xef) { longword(h,STAGE_CALLBACK,0xc11958); return; }
    offset=read_word(h,VIEW_RECORD); observe(h,MC_WORD_TEST,offset,0);
    if(!offset) {
        selected=read_byte(h,CONTEXT_SELECT); observe(h,MC_BYTE_TEST,selected,0);
        if(!selected) {
            value=read_byte(h,PLAYER_FLAGS_E); observe(h,MC_CMP_BYTE,value,5);
            if((int8_t)value>5) { byte(h,VIEWPORT_TARGET,2); return; }
        }
    }
    byte(h,VIEWPORT_TARGET,15);
}
