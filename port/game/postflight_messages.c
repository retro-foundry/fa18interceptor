/* Complete postflight text, outcome and return owners. Source guards and
 * saved-frame reloads preserve the original callback and child boundaries. */
#include "postflight_messages.h"
#include "globals.h"
#include <stdlib.h>
static void observe(const PostflightMessageHooks *h,enum PostflightMessagePhase p,uint32_t v,uint32_t other) {
    if(h && h->observe) h->observe(h->context,p,v,other);
}
static int32_t consume(const PostflightMessageHooks *h,enum PostflightMessageChild child) {
    if(h && h->consume) return h->consume(h->context,child);
    abort(); /* These children require the platform's actual service boundary. */
}
static uint8_t read_byte(const PostflightMessageHooks *h,gaddr a) {
    uint8_t v=rd_u8(a); observe(h,PM_D0_BYTE,v,0); return v;
}
static uint16_t read_word(const PostflightMessageHooks *h,gaddr a) {
    uint16_t v=rd_u16(a); observe(h,PM_D0_WORD,v,0); return v;
}
static void byte(const PostflightMessageHooks *h,gaddr a,uint8_t v) { wr_u8(a,v); observe(h,PM_STORE_BYTE,v,0); }
static void word(const PostflightMessageHooks *h,gaddr a,uint16_t v) { wr_u16(a,v); observe(h,PM_STORE_WORD,v,0); }
static void longword(const PostflightMessageHooks *h,gaddr a,uint32_t v) { wr_u32(a,v); observe(h,PM_STORE_LONG,v,0); }
static gaddr address(const PostflightMessageHooks *h,unsigned reg,gaddr a) { observe(h,reg?PM_A1:PM_A0,a,0); return a; }
static void callback(const PostflightMessageHooks *h,gaddr a,int lea) {
    if(lea) address(h,0,a);
    longword(h,STAGE_CALLBACK,a);
}
static void add_local(const PostflightMessageHooks *h,gaddr a,uint32_t amount,unsigned width) {
    uint32_t v=width==2?rd_u16(a):rd_u32(a);
    observe(h,width==2?PM_ADD_MEMORY_WORD:PM_ADD_MEMORY_LONG,v,amount);
    if(width==2) wr_u16(a,(uint16_t)(v+amount)); else wr_u32(a,v+amount);
}
static int expired(const PostflightMessageHooks *h) {
    uint16_t v=read_word(h,POST_INPUT_COUNTDOWN); observe(h,PM_TEST_WORD,v,0); return (int16_t)v<0;
}
static int message_finished(const PostflightMessageHooks *h) {
    uint8_t v=read_byte(h,MESSAGE_STATE_C); observe(h,PM_TEST_BYTE,v,0); return (int8_t)v<0;
}
static int32_t signed_mode(const PostflightMessageHooks *h,int full) {
    int16_t v=(int8_t)read_byte(h,MODE_SELECT); observe(h,PM_EXT_WORD,(uint16_t)v,0);
    if(full) observe(h,PM_EXT_LONG,(uint32_t)(int32_t)v,0);
    return full?v:(uint16_t)v;
}
static void compare(const PostflightMessageHooks *h,uint32_t v,uint32_t limit,unsigned width) {
    observe(h,width==1?PM_CMP_BYTE:width==2?PM_CMP_WORD:PM_CMP_LONG,v,limit);
}
static void append(const PostflightMessageHooks *h,gaddr local,uint16_t code) {
    gaddr cursor=address(h,0,rd_u32(local)); word(h,cursor,code); add_local(h,local,2,4);
}
void initialise_postflight_text(const PostflightMessageHooks *h) {
    consume(h,PM_BOOT_INPUT); consume(h,PM_BOOT_DISPLAY); consume(h,PM_OPEN_TEXT);
    consume(h,PM_SELECT_TEXT); consume(h,PM_BOOT_TIMER); consume(h,PM_BOOT_SCENE);
    consume(h,PM_BOOT_VIEW); consume(h,PM_SET_BOUNDS); consume(h,PM_SET_CENTRE);
    byte(h,0xc4584b,0); consume(h,PM_BOOT_MENU); consume(h,PM_BOOT_RECORDS); callback(h,0xc0f812,1);
}
void copy_postflight_text(gaddr frame,const PostflightMessageHooks *h) {
    uint16_t count,value; gaddr src,dst; uint32_t selected; uint8_t old;
    consume(h,PM_INIT_TEXT); longword(h,frame-4,rd_u32(LONG_TABLE)); word(h,frame-10,0);
    for(;;) {
        count=read_word(h,frame-10); compare(h,count,32,2); if((int16_t)count>=32) break;
        observe(h,PM_EXT_LONG,(uint32_t)(int32_t)(int16_t)count,0); observe(h,PM_SHIFT_LONG,1,0);
        src=address(h,0,(uint32_t)(int32_t)(int16_t)count*2u); src=address(h,0,src+0xc08510);
        dst=address(h,1,rd_u32(frame-4)); value=rd_u16(src); word(h,dst,value);
        add_local(h,frame-4,2,4); add_local(h,frame-10,1,2);
    }
    byte(h,MODE_SELECT,0); observe(h,PM_D0_LONG,0xffffffffu,0);
    byte(h,ORIGIN_GATE_MODE,0xff); byte(h,MENU_TRANSITION_FLAG,0xff);
    word(h,POST_INPUT_COUNTDOWN,3); callback(h,0xc11446,0);
    dst=address(h,0,0xc3f040); longword(h,frame-14,dst); dst=address(h,0,dst+21);
    longword(h,frame-8,0); observe(h,PM_D0_LONG,0,0); selected=read_word(h,0xc4564c);
    longword(h,frame-14,dst); compare(h,selected,0xc560,4);
    if(selected!=0xc560) { byte(h,dst,0x31); longword(h,frame-8,selected); }
    observe(h,PM_D0_LONG,0,0); selected=read_word(h,0xc45650); compare(h,selected,0x7e70,4);
    if(selected!=0x7e70) {
        dst=address(h,0,rd_u32(frame-14)); old=rd_u8(dst); observe(h,PM_D1_BYTE,old,0);
        observe(h,PM_OR_D1,0x32,0); byte(h,dst,old|0x32); longword(h,frame-8,selected);
    }
    observe(h,PM_D0_LONG,0,0); selected=read_word(h,0xc45654); compare(h,selected,0x4de8,4);
    if(selected!=0x4de8) {
        dst=address(h,0,rd_u32(frame-14)); old=rd_u8(dst); observe(h,PM_D1_BYTE,old,0);
        observe(h,PM_OR_D1,0x34,0); byte(h,dst,old|0x34); longword(h,frame-8,selected);
    }
    selected=rd_u32(frame-8); observe(h,PM_TEST_LONG,selected,0);
    if(selected) {
        dst=address(h,0,rd_u32(frame-14)); address(h,0,dst+1);
        consume(h,PM_FORMAT_TEXT); callback(h,0xc113e4,0);
    } else consume(h,PM_DELAY_TEXT);
}
void raise_postflight_message_event(const PostflightMessageHooks *h) {
    if(!expired(h)) return;
    observe(h,PM_D0_LONG,1,0); byte(h,POST_INPUT_EVENT,1); word(h,POST_INPUT_COUNTDOWN,2);
    callback(h,0xc110a4,1); byte(h,CONTEXT_GATE,1);
}
typedef struct {
    gaddr frame,cursor;
    uint16_t mode;
} ResultMessageLocals;
static gaddr result_cursor(const ResultMessageLocals *locals) {
    return locals->frame?rd_u32(locals->frame-6):locals->cursor;
}
static uint16_t result_mode(const ResultMessageLocals *locals) {
    return locals->frame?rd_u16(locals->frame-2):locals->mode;
}
static void result_store_cursor(ResultMessageLocals *locals,gaddr cursor,const PostflightMessageHooks *h) {
    locals->cursor=cursor;
    if(locals->frame) wr_u32(locals->frame-6,cursor);
    observe(h,PM_STORE_LONG,cursor,0);
}
static void result_advance(ResultMessageLocals *locals,const PostflightMessageHooks *h) {
    gaddr cursor=result_cursor(locals);
    observe(h,PM_ADD_MEMORY_LONG,cursor,2);
    locals->cursor=cursor+2;
    if(locals->frame) wr_u32(locals->frame-6,locals->cursor);
}
static void result_append(ResultMessageLocals *locals,const PostflightMessageHooks *h,uint16_t code) {
    gaddr cursor=address(h,0,result_cursor(locals)); word(h,cursor,code); result_advance(locals,h);
}
static void prepare_result_messages(ResultMessageLocals *locals,const PostflightMessageHooks *h) {
    uint16_t mode,phase_word; uint8_t phase; int32_t selected; gaddr cursor;
    if(!expired(h)) return;
    mode=(uint16_t)signed_mode(h,0); result_store_cursor(locals,MESSAGE_QUEUE,h); byte(h,POST_INPUT_AUX,0);
    callback(h,0xc10dae,1); locals->mode=mode;
    if(locals->frame) wr_u16(locals->frame-2,mode);
    observe(h,PM_STORE_WORD,mode,0); consume(h,PM_RESET_SEQUENCE);
    phase=read_byte(h,PLAYER_PHASE); compare(h,phase,0xff,1);
    if(phase==0xff) {
        mode=result_mode(locals); observe(h,PM_D0_WORD,mode,0); compare(h,mode,3,2);
        if(mode>=3) { compare(h,mode,8,2); }
        if(mode>=3 && mode<=8) {
            consume(h,PM_INDEXED_MESSAGE);
            cursor=address(h,0,result_cursor(locals)); cursor=address(h,0,cursor+2); result_store_cursor(locals,cursor,h);
            mode=result_mode(locals); compare(h,mode,3,2);
            if(mode==3) { word(h,cursor,0x8055); result_advance(locals,h); }
            else result_append(locals,h,0x8056);
        } else {
            selected=signed_mode(h,1); compare(h,(uint32_t)selected,9,4);
            if(selected==9) {
                cursor=address(h,0,rd_u32(MODE_TABLE)); word(h,cursor,1); consume(h,PM_LOAD_MODE);
                cursor=address(h,0,result_cursor(locals)); word(h,cursor,0x4a); cursor=address(h,0,cursor+2);
                word(h,cursor,0x8053); cursor=address(h,0,cursor+2);
                byte(h,SEQUENCE_FLAG,1); byte(h,PLAYER_PHASE,0xef); result_store_cursor(locals,cursor,h);
            } else {
                compare(h,(uint32_t)selected,125,4);
                if(selected==125) { result_append(locals,h,0x48); byte(h,PLAYER_PHASE,0xf0); }
                else result_append(locals,h,0x41);
            }
        }
    } else {
        phase=read_byte(h,PLAYER_PHASE); compare(h,phase,0xfe,1);
        if(phase==0xfe) {
            selected=signed_mode(h,1); observe(h,PM_SUB_LONG,4,0); selected=(int32_t)((uint32_t)selected-4u);
            if(selected>=0) compare(h,(uint32_t)selected,4,4);
            if(selected>=0 && selected<4) {
                observe(h,PM_SHIFT_LONG,1,0);
                switch(selected) {
                case 0:
                    phase_word=read_word(h,0xc458da); observe(h,PM_BIT_D0,phase_word,0);
                    result_append(locals,h,(phase_word&1)?0x24:0x25); break;
                case 1: result_append(locals,h,0x2c); byte(h,PLAYER_PHASE,0xf0); break;
                case 2: result_append(locals,h,0x33); break;
                case 3:
                    cursor=address(h,0,result_cursor(locals)); word(h,cursor,0x39); cursor=address(h,0,cursor+2);
                    phase_word=read_word(h,0xc458da); result_store_cursor(locals,cursor,h); observe(h,PM_BIT_D0,phase_word,0);
                    if(phase_word&1) { word(h,cursor,0x803a); result_advance(locals,h); byte(h,PLAYER_PHASE,0xf0); }
                    else result_append(locals,h,0x805c);
                    break;
                }
            } else result_append(locals,h,0x42);
            phase=read_byte(h,PLAYER_PHASE); compare(h,phase,0xf0,1);
            if(phase!=0xf0) {
                cursor=address(h,0,result_cursor(locals)); cursor=address(h,0,cursor+2);
                word(h,cursor,0x8056); cursor=address(h,0,cursor+2);
                word(h,cursor,0x8053); cursor=address(h,0,cursor+2); result_store_cursor(locals,cursor,h);
            }
        } else {
            phase=read_byte(h,PLAYER_PHASE); compare(h,phase,0xfd,1);
            if(phase==0xfd) {
                selected=signed_mode(h,1); compare(h,(uint32_t)selected,4,4);
                if(selected==4) { result_append(locals,h,0x4c); byte(h,PLAYER_PHASE,0xf0); }
            }
        }
    }
    phase=read_byte(h,PLAYER_PHASE); compare(h,phase,0xfc,1);
    if(phase==0xfc) {
        cursor=address(h,0,result_cursor(locals)); word(h,cursor,0x54); cursor=address(h,0,cursor+2);
        word(h,cursor,0x8053); cursor=address(h,0,cursor+2); result_store_cursor(locals,cursor,h);
        consume(h,PM_RECORD_OUTCOME); consume(h,PM_LOAD_OUTCOME);
    }
    cursor=address(h,0,result_cursor(locals)); word(h,cursor,0);
}
void prepare_postflight_messages(gaddr frame,const PostflightMessageHooks *h) {
    ResultMessageLocals locals={.frame=frame}; prepare_result_messages(&locals,h);
}
void prepare_postflight_result(const PostflightMessageHooks *h) {
    ResultMessageLocals locals={0}; prepare_result_messages(&locals,h);
}
static void record_result(gaddr frame,const PostflightMessageHooks *h) {
    int32_t mode=signed_mode(h,1); gaddr a,b,entry; uint16_t count; uint8_t attempts;
    a=address(h,0,rd_u32(MODE_TABLE)); a=address(h,0,a+18); a=address(h,0,a+(uint32_t)mode);
    entry=a;
    if(frame) longword(h,frame-4,a); else observe(h,PM_STORE_LONG,a,0);
    consume(h,PM_REFRESH_OUTCOME);
    a=address(h,0,rd_u32(MODE_TABLE)); byte(h,a+6,rd_u8(MODE_SELECT));
    a=address(h,0,frame?rd_u32(frame-4):entry); b=address(h,1,rd_u32(MODE_TABLE)); byte(h,b+7,rd_u8(a));
    a=address(h,0,rd_u32(MODE_TABLE)); count=read_word(h,a+0x38); observe(h,PM_ADD_WORD,1,0); count++;
    a=address(h,0,rd_u32(MODE_TABLE)); word(h,a+0x38,count);
    attempts=read_byte(h,SCENE_DISPATCH_LIMIT); observe(h,PM_ADD_BYTE,1,0); attempts++;
    byte(h,SCENE_DISPATCH_LIMIT,attempts); compare(h,attempts,3,1); if((int8_t)attempts>3) byte(h,SCENE_DISPATCH_LIMIT,3);
    a=address(h,0,frame?rd_u32(frame-4):entry); attempts=read_byte(h,a); observe(h,PM_ADD_BYTE,1,0); attempts++;
    byte(h,a,attempts); compare(h,attempts,3,1); if(attempts>3) byte(h,a,3);
    byte(h,SCENE_DISPATCH_LIMIT_PREVIOUS,rd_u8(SCENE_DISPATCH_LIMIT));
}
void record_postflight_outcome(gaddr frame,const PostflightMessageHooks *h) { record_result(frame,h); }
void record_postflight_result(const PostflightMessageHooks *h) { record_result(0,h); }
void refresh_postflight_grade(void) {
    wr_s16(rd_u32(MODE_TABLE)+2,(int16_t)(int8_t)rd_u8(SCENE_DISPATCH_LIMIT));
}
void queue_postflight_text_error(const PostflightMessageHooks *h) {
    if(!expired(h)) { consume(h,PM_CLEAR_ERROR); return; }
    consume(h,PM_LOAD_ERROR_TABLE); word(h,MESSAGE_QUEUE,0x61); word(h,POST_INPUT_COUNTDOWN,500); callback(h,0xc1141e,1);
}
void wait_postflight_text_error(const PostflightMessageHooks *h) {
    if(!message_finished(h) || !expired(h)) return;
    word(h,POST_INPUT_COUNTDOWN,3); callback(h,0xc1141e,1);
}
void queue_postflight_intro(const PostflightMessageHooks *h) {
    if(!expired(h)) { consume(h,PM_CLEAR_INTRO); return; }
    consume(h,PM_LOAD_INTRO_TABLE); word(h,MESSAGE_QUEUE,15); callback(h,0xc11478,1);
}
void accept_postflight_return(const PostflightMessageHooks *h) {
    uint8_t key=rd_u8(KEY_TAKEN); observe(h,PM_TEST_BYTE,key,0); if(!key) return;
    longword(h,MASTER_VOLUME_TARGET,0x1f0000); byte(h,0xc457d7,1);
    consume(h,PM_LOAD_RETURN_TABLE); consume(h,PM_RESET_RETURN); consume(h,PM_CLEAR_RETURN);
    byte(h,COMMAND_RETURN_STATE,1); word(h,POST_INPUT_COUNTDOWN,5); byte(h,MODE_SELECT,0);
    consume(h,PM_REFRESH_RETURN); callback(h,0xc114d2,1);
}
void prepare_postflight_status(gaddr frame,const PostflightMessageHooks *h) {
    uint8_t state; uint16_t status; gaddr cursor;
    consume(h,PM_CLEAR_STATUS); word(h,POST_INPUT_COUNTDOWN,5);
    state=read_byte(h,COMMAND_RETURN_STATE); observe(h,PM_TEST_BYTE,state,0);
    if(!state) { consume(h,PM_REFRESH_STATUS); byte(h,COMMAND_EVENT_COUNTER,0xff); callback(h,0xc115ba,1); return; }
    status=read_word(h,MENU_TABLE_STATUS); observe(h,PM_TEST_WORD,status,0);
    if(!status) { byte(h,COMMAND_RETURN_STATE,0); callback(h,0xc115ba,1); return; }
    consume(h,PM_LOAD_STATUS_TABLE); cursor=address(h,0,MESSAGE_QUEUE); status=read_word(h,MENU_TABLE_STATUS);
    longword(h,frame-4,cursor); observe(h,PM_SUB_WORD,1,0);
    if(status==1) { word(h,cursor,0x44); add_local(h,frame-4,2,4); }
    else {
        status=read_word(h,MENU_TABLE_STATUS); observe(h,PM_SUB_WORD,2,0);
        if(status==2) append(h,frame-4,0x45);
        else { status=read_word(h,MENU_TABLE_STATUS); observe(h,PM_SUB_WORD,3,0); if(status==3) append(h,frame-4,0x46); }
    }
    append(h,frame-4,0x5e); cursor=address(h,0,rd_u32(frame-4)); word(h,cursor,0); callback(h,0xc1159e,1);
}
void wait_postflight_status(const PostflightMessageHooks *h) {
    uint8_t state=read_byte(h,COMMAND_RETURN_STATE); observe(h,PM_TEST_BYTE,state,0); if(state) return;
    consume(h,PM_CLEAR_STATUS_WAIT); callback(h,0xc114d2,1);
}
void prepare_postflight_retry(gaddr frame,const PostflightMessageHooks *h) {
    uint16_t status,count; uint8_t c; gaddr a,b;
    status=read_word(h,MENU_TABLE_STATUS); observe(h,PM_TEST_WORD,status,0);
    if(status) { consume(h,PM_RESET_RETRY); callback(h,0xc0fbe0,0); return; }
    if(!expired(h)) { consume(h,PM_CLEAR_RETRY); return; }
    byte(h,CONTEXT_GATE,1); consume(h,PM_LOAD_RETRY_TABLE);
    observe(h,PM_D0_LONG,0,0); byte(h,MESSAGE_STATE_C,0); byte(h,0xc457c3,0);
    a=address(h,0,rd_u32(MODE_TABLE)); count=read_word(h,a+4); compare(h,count,0,2);
    if(!count) { word(h,MESSAGE_QUEUE,1); callback(h,0xc1169a,1); return; }
    word(h,MESSAGE_QUEUE,2); a=address(h,0,0xc3f1e3); longword(h,frame-6,a); a=address(h,0,a+10);
    b=address(h,1,rd_u32(MODE_TABLE)); b=address(h,1,b+30); word(h,frame-2,0);
    longword(h,frame-6,a); longword(h,frame-10,b);
    for(;;) {
        count=rd_u16(frame-2); compare(h,count,24,2); if((int16_t)count>=24) break;
        a=address(h,0,rd_u32(frame-10)); c=read_byte(h,a); observe(h,PM_TEST_BYTE,c,0); if(!c) break;
        a=address(h,0,rd_u32(frame-10)); b=address(h,1,rd_u32(frame-6)); byte(h,b,rd_u8(a));
        add_local(h,frame-10,1,4); add_local(h,frame-6,1,4); add_local(h,frame-2,1,2);
    }
    callback(h,0xc116b0,1);
}
void wait_postflight_retry_message(const PostflightMessageHooks *h) { if(message_finished(h)) callback(h,0xc116ce,1); }
void wait_postflight_retry_input(const PostflightMessageHooks *h) {
    uint8_t key; if(message_finished(h)) { callback(h,0xc116ce,1); return; }
    key=rd_u8(KEY_TAKEN); observe(h,PM_TEST_BYTE,key,0); if(key) callback(h,0xc116ce,1);
}
void advance_postflight_retry(const PostflightMessageHooks *h) {
    gaddr a; uint16_t count;
    consume(h,PM_RESET_COMPLETE); word(h,POST_INPUT_COUNTDOWN,5);
    a=address(h,0,rd_u32(MODE_TABLE)); count=read_word(h,a+4); compare(h,count,0,2);
    if(!count) { word(h,MESSAGE_QUEUE,3); byte(h,0xc457c3,0); byte(h,MESSAGE_STATE_C,2); byte(h,0xc457f5,20); callback(h,0xc11738,1); }
    else callback(h,0xc0fbe0,0);
    a=address(h,0,rd_u32(MODE_TABLE)); count=read_word(h,a+4); observe(h,PM_ADD_WORD,1,0); count++;
    a=address(h,0,rd_u32(MODE_TABLE)); word(h,a+4,count);
}
void finish_postflight_retry(const PostflightMessageHooks *h) {
    if(!message_finished(h)) return;
    consume(h,PM_RESET_FINISH); word(h,POST_INPUT_COUNTDOWN,5); callback(h,0xc0fbe0,0);
}
