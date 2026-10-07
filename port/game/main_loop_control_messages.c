/* Complete original main-loop control records and message-sequence owner.
 * Actual sound, record-update, code-check and glyph children remain distinct. */
#include "main_loop_control_messages.h"
#include <stdlib.h>
#define CONTROL_SELECTED_RECORD 0xc18214u
#define CONTROL_RECORDS 0xc45c72u
#define CONTROL_REQUEST 0xc46201u
#define CONTROL_COOLDOWN 0xc461bfu
#define CONTROL_BUDGET 0xc461e4u
#define PLAYER_RECORD 0xc1ab74u
#define MESSAGE_SELECTORS 0xc4574au
#define MESSAGE_CURSOR 0xc457c6u
#define MESSAGE_ACTIVE 0xc457c3u
#define MESSAGE_MODE 0xc457e0u
#define MESSAGE_DELAY 0xc45744u
#define MESSAGE_WAIT 0xc45746u
#define MESSAGE_REPEAT 0xc45748u
#define MESSAGE_FLAGS 0xc457dcu
#define MESSAGE_PACE 0xc457dbu
#define MESSAGE_READY 0xc457deu
#define MESSAGE_SEGMENT_DELAY 0xc457dfu
#define MESSAGE_EVENTS 0xc457e1u
#define MESSAGE_BUFFER 0xc457ebu
#define MESSAGE_BUFFER_SPACE 0xc457f5u
#define MESSAGE_BUFFER_SIZE 0xc457f6u
#define MESSAGE_EVENT_CURSOR 0xc457f8u
#define MESSAGE_EVENT_COUNT 0xc457f9u
#define MESSAGE_RESTART_STRIDE 0xc4573eu
#define MESSAGE_RESTART_CURSOR 0xc4570au
#define MESSAGE_LIVE_CURSOR 0xc456feu
#define MESSAGE_COLOUR 0xc45952u
static void observe(const MainControlHooks *h,enum MainControlPhase p,uint32_t v,uint32_t other) {
    if(h && h->observe) h->observe(h->context,p,v,other);
}
static MessageWorking consume(const MainControlHooks *h,enum MainControlChild child) {
    if(h && h->consume) return h->consume(h->context,child);
    abort();
}
static MessageWorking consume_message(const MainControlHooks *h,enum MainControlChild child,MessageWorking work) {
    if(h && h->consume_values) return h->consume_values(h->context,child,work);
    return consume(h,child);
}
static void byte(const MainControlHooks *h,gaddr a,uint8_t v) { wr_u8(a,v); observe(h,MC_STORE_BYTE,v,0); }
static void word(const MainControlHooks *h,gaddr a,uint16_t v) { wr_u16(a,v); observe(h,MC_STORE_WORD,v,0); }
static void longword(const MainControlHooks *h,gaddr a,uint32_t v) { wr_u32(a,v); observe(h,MC_STORE_LONG,v,0); }
static uint8_t primary_byte(const MainControlHooks *h,gaddr a) { uint8_t v=rd_u8(a); observe(h,MC_PRIMARY_BYTE,v,0); return v; }
static uint16_t primary_word(const MainControlHooks *h,gaddr a) { uint16_t v=rd_u16(a); observe(h,MC_PRIMARY_WORD,v,0); return v; }
static gaddr lookup(const MainControlHooks *h,gaddr a) { observe(h,MC_LOOKUP,a,0); return a; }
static int test_byte(const MainControlHooks *h,gaddr a) { uint8_t v=rd_u8(a); observe(h,MC_TEST_BYTE,v,0); return v!=0; }
static void add_byte(const MainControlHooks *h,gaddr a,unsigned amount) {
    uint8_t old=rd_u8(a); wr_u8(a,(uint8_t)(old+amount)); observe(h,MC_MEMORY_ADD_BYTE,old,amount);
}
static uint8_t subtract_byte(const MainControlHooks *h,gaddr a,unsigned amount) {
    uint8_t old=rd_u8(a); wr_u8(a,(uint8_t)(old-amount)); observe(h,MC_MEMORY_SUB_BYTE,old,amount); return old;
}
static void add_word(const MainControlHooks *h,gaddr a,unsigned amount) {
    uint16_t old=rd_u16(a); wr_u16(a,(uint16_t)(old+amount)); observe(h,MC_MEMORY_ADD_WORD,old,amount);
}
static uint16_t subtract_word(const MainControlHooks *h,gaddr a,unsigned amount) {
    uint16_t old=rd_u16(a); wr_u16(a,(uint16_t)(old-amount)); observe(h,MC_MEMORY_SUB_WORD,old,amount); return old;
}
static int bit(const MainControlHooks *h,uint32_t v,unsigned index) { observe(h,MC_BIT_TEST,v,index); return (v&(1u<<index))!=0; }
void advance_main_loop_control_records(gaddr frame,const MainControlHooks *h) {
    gaddr record,flags,request,cooldown,player; uint8_t packed,remaining,mode; uint16_t index,value;
    observe(h,MC_PRIMARY_LONG,0,0); byte(h,frame-3,0); byte(h,frame-1,0); byte(h,frame-2,0);
    consume(h,MC_RESET_FACE_STATE); longword(h,frame-8,CONTROL_REQUEST); longword(h,frame-12,CONTROL_COOLDOWN);
    cooldown=lookup(h,rd_u32(frame-12)); remaining=primary_byte(h,cooldown); observe(h,MC_PRIMARY_SUB_BYTE,1,0); byte(h,cooldown,(uint8_t)(remaining-1));
    word(h,frame-16,0);
    for(;;) {
        index=primary_word(h,frame-16); observe(h,MC_COMPARE_WORD,index,20); if((int16_t)index>=20) break;
        observe(h,MC_PRIMARY_EXT_LONG,(uint32_t)(int32_t)(int16_t)index,0); observe(h,MC_PRIMARY_SHIFT_LONG,6,0);
        record=lookup(h,(uint32_t)(int32_t)(int16_t)index<<6); record=lookup(h,record+CONTROL_RECORDS); longword(h,CONTROL_SELECTED_RECORD,record);
        if(!test_byte(h,0xc457bdu) && !test_byte(h,0xc457aeu)) {
            value=primary_word(h,record+40); observe(h,MC_PRIMARY_SUB_WORD,1,0);
            record=lookup(h,rd_u32(CONTROL_SELECTED_RECORD)); word(h,record+40,(uint16_t)(value-1));
        }
        flags=lookup(h,rd_u32(CONTROL_SELECTED_RECORD)); flags=lookup(h,flags+38); packed=primary_byte(h,0xc457aeu); longword(h,frame-20,flags);
        observe(h,MC_TEST_BYTE,packed,0);
        if(!packed) { value=primary_word(h,0xc458c6u); if(bit(h,value,3)) goto consider_request; }
        if(!test_byte(h,0xc4588bu)) goto update_record;
consider_request:
        flags=lookup(h,rd_u32(frame-20)); value=primary_word(h,flags); if(bit(h,value,0)) goto update_record;
        if(test_byte(h,frame-3)) goto wait_for_cooldown;
        request=lookup(h,rd_u32(frame-8)); packed=primary_byte(h,request); observe(h,MC_PRIMARY_AND_BYTE,0xf0,0); packed&=0xf0; observe(h,MC_TEST_BYTE,packed,0);
        if(!packed) goto wait_for_cooldown;
        value=primary_word(h,CONTROL_BUDGET); observe(h,MC_COMPARE_WORD,value,1); if((int16_t)value<1) goto wait_for_cooldown;
        add_byte(h,frame-3,1); packed=primary_byte(h,request); byte(h,frame-22,packed);
        observe(h,MC_PRIMARY_AND_BYTE,0xf0,0); observe(h,MC_PRIMARY_SUB_BYTE,0x10,0); remaining=(uint8_t)((packed&0xf0)-0x10);
        packed=rd_u8(frame-22); observe(h,MC_SECONDARY_BYTE,packed,0); observe(h,MC_SECONDARY_AND_BYTE,0x0f,0); observe(h,MC_SECONDARY_OR_BYTE,remaining,0);
        byte(h,request,(uint8_t)((packed&0x0f)|remaining)); add_byte(h,frame-1,1); add_byte(h,frame-2,1);
        player=lookup(h,rd_u32(PLAYER_RECORD)); value=rd_u16(player+58); observe(h,MC_SECONDARY_WORD,value,0); observe(h,MC_SECONDARY_ADD_WORD,3,0);
        player=lookup(h,rd_u32(PLAYER_RECORD)); word(h,player+58,(uint16_t)(value+3)); byte(h,0xc457c5u,1);
        flags=lookup(h,rd_u32(frame-20)); word(h,flags,0x401); record=lookup(h,rd_u32(CONTROL_SELECTED_RECORD)); word(h,record+46,0); byte(h,frame-13,remaining);
        if(test_byte(h,0xc4588bu)) {
            packed=primary_byte(h,0xc457adu); observe(h,MC_TEST_BYTE,packed,0); if(!packed) consume(h,MC_CONTROL_TONE);
            mode=primary_byte(h,0xc4588bu); observe(h,MC_PRIMARY_SUB_BYTE,1,0);
            flags=lookup(h,rd_u32(frame-20)); value=primary_word(h,flags);
            observe(h,MC_PRIMARY_OR_WORD,mode==1?0x2000:0x1000,0); word(h,flags,(uint16_t)(value|(mode==1?0x2000:0x1000)));
            index=primary_word(h,frame-16); observe(h,MC_PRIMARY_EXT_LONG,(uint32_t)(int32_t)(int16_t)index,0); observe(h,MC_PRIMARY_ADD_LONG,1,0);
            byte(h,mode==1?0xc4588du:0xc4588cu,(uint8_t)(index+1)); byte(h,0xc4588bu,0);
        } else {
            packed=primary_byte(h,0xc457adu); observe(h,MC_TEST_BYTE,packed,0);
            if(!packed) {
                if(bit(h,rd_u8(0xc45b5bu),5)) {
                    if(test_byte(h,0xc45785u)) consume(h,MC_CONTROL_ALERT_FIRST); else consume(h,MC_CONTROL_ALERT_SECOND);
                } else consume(h,MC_CONTROL_FALLBACK_TONE);
            }
            value=primary_word(h,CONTROL_BUDGET); observe(h,MC_PRIMARY_SUB_WORD,3,0); word(h,CONTROL_BUDGET,(uint16_t)(value-3)); observe(h,MC_TEST_WORD,(uint16_t)(value-3),0);
            if((int16_t)(value-3)<0) word(h,CONTROL_BUDGET,0);
            byte(h,0xc45843u,3); cooldown=lookup(h,rd_u32(frame-12)); byte(h,cooldown,20);
        }
        index=primary_word(h,frame-16); observe(h,MC_PRIMARY_EXT_LONG,(uint32_t)(int32_t)(int16_t)index,0); consume(h,MC_BEGIN_RECORD); goto update_record;
wait_for_cooldown:
        cooldown=lookup(h,rd_u32(frame-12)); packed=primary_byte(h,cooldown); observe(h,MC_TEST_BYTE,packed,0);
        if((int8_t)packed>0) add_byte(h,frame-1,1);
update_record:
        flags=lookup(h,rd_u32(frame-20)); value=primary_word(h,flags);
        if(bit(h,value,0)) { index=primary_word(h,frame-16); observe(h,MC_PRIMARY_EXT_LONG,(uint32_t)(int32_t)(int16_t)index,0); consume(h,MC_ADVANCE_RECORD); }
        add_word(h,frame-16,2);
    }
    if(!test_byte(h,frame-1)) {
        cooldown=lookup(h,rd_u32(frame-12)); byte(h,cooldown,0); request=lookup(h,rd_u32(frame-8)); packed=primary_byte(h,request);
        observe(h,MC_PRIMARY_AND_BYTE,0x0f,0); observe(h,MC_PRIMARY_OR_BYTE,0x50,0); byte(h,request,(uint8_t)((packed&0x0f)|0x50));
    }
}
static MessageWorking load_cursor(const MainControlHooks *h,MessageWorking w,gaddr a) {
    w.positions=rd_u32(a); w.text=rd_u32(a+4); w.origin=rd_u32(a+8); observe(h,MC_CURSOR_LOAD,a,0); return w;
}
static void store_cursor(const MainControlHooks *h,MessageWorking w,gaddr a) {
    wr_u32(a,w.positions); wr_u32(a+4,w.text); wr_u32(a+8,w.origin); observe(h,MC_CURSOR_STORE,a,0);
}
static uint8_t secondary_byte(const MainControlHooks *h,gaddr a) { uint8_t v=rd_u8(a); observe(h,MC_SECONDARY_BYTE,v,0); return v; }
static void set_lookup(const MainControlHooks *h,MessageWorking *w,gaddr a) { w->lookup=lookup(h,a); }
static void set_positions(const MainControlHooks *h,MessageWorking *w,gaddr a) { w->positions=a; observe(h,MC_POSITIONS,a,0); }
static void set_text(const MainControlHooks *h,MessageWorking *w,gaddr a) { w->text=a; observe(h,MC_TEXT,a,0); }
static void set_origin(const MainControlHooks *h,MessageWorking *w,gaddr a) { w->origin=a; observe(h,MC_ORIGIN,a,0); }
static void set_glyph(const MainControlHooks *h,MessageWorking *w,gaddr a) { w->glyph=a; observe(h,MC_GLYPH,a,0); }
static void refresh_colour(const MainControlHooks *h,MessageWorking *w) {
    w->colour=(w->control&0xf0u)>>4; observe(h,MC_COLOUR_FROM_CONTROL,0,0); word(h,MESSAGE_COLOUR,(uint16_t)w->colour);
}
static MessageSequenceResult message_result(MessageWorking w,int assigned) {
    return (MessageSequenceResult){(uint8_t)w.character,assigned};
}
MessageSequenceResult advance_main_loop_message_sequence(MessageWorking w,const MainControlHooks *h) {
    int assigned=0;
    uint16_t selector,value,relative; uint8_t mode,event,cursor,flags,previous; unsigned i;
    if(test_byte(h,0xc45871u)) return message_result(w,assigned);
    set_lookup(h,&w,MESSAGE_SELECTORS); w.control=secondary_byte(h,MESSAGE_CURSOR);
    observe(h,MC_SECONDARY_EXT_WORD,(uint16_t)(int16_t)(int8_t)w.control,0);
    selector=primary_word(h,w.lookup+(uint32_t)(int32_t)(int8_t)w.control); if(!selector) goto count_delay;
    observe(h,MC_TEST_BYTE,(uint8_t)w.control,0); if(!(uint8_t)w.control) word(h,0xc45772u,selector);
    if(test_byte(h,MESSAGE_ACTIVE)) goto active_sequence;
    byte(h,MESSAGE_BUFFER_SIZE,0); set_positions(h,&w,0xc41066u); set_lookup(h,&w,0xc3ed0au);
    observe(h,MC_SELECT_MESSAGE,selector,0);
    if((int16_t)selector<0) { set_origin(h,&w,rd_u32(MESSAGE_LIVE_CURSOR+8)); set_origin(h,&w,w.origin+440); observe(h,MC_PRIMARY_AND_WORD,0x3fff,0); }
    value=(int16_t)selector<0?(selector&0x3fff):selector;
    observe(h,MC_PRIMARY_SUB_WORD,1,0); --value; observe(h,MC_PRIMARY_DOUBLE,0,0); value=(uint16_t)(value*2);
    relative=primary_word(h,w.lookup+(uint32_t)(int32_t)(int16_t)value); set_text(h,&w,w.lookup+(uint32_t)(int32_t)(int16_t)relative);
    observe(h,MC_TEST_WORD,selector,0);
    if((int16_t)selector<0) set_text(h,&w,w.text+2);
    else {
        observe(h,MC_PRIMARY_LONG,0,0); value=primary_byte(h,w.text); set_text(h,&w,w.text+1);
        observe(h,MC_PRIMARY_SHIFT_WORD,3,0); value=(uint16_t)(value*8); observe(h,MC_SECONDARY_WORD,value,0);
        observe(h,MC_PRIMARY_SHIFT_WORD,2,0); observe(h,MC_PRIMARY_ADD_SECONDARY,0,0); value=(uint16_t)(value*5);
        w.control=secondary_byte(h,w.text); set_text(h,&w,w.text+1); observe(h,MC_SECONDARY_EXT_WORD,(uint16_t)(int16_t)(int8_t)w.control,0);
        observe(h,MC_PRIMARY_ADD_SECONDARY,0,0); value=(uint16_t)(value+(int16_t)(int8_t)w.control);
        observe(h,MC_PRIMARY_EXT_LONG,(uint32_t)(int32_t)(int16_t)value,0); set_origin(h,&w,(uint32_t)(int32_t)(int16_t)value);
    }
    word(h,MESSAGE_DELAY,0); word(h,MESSAGE_WAIT,0); word(h,MESSAGE_REPEAT,0);
    mode=rd_u8(MESSAGE_MODE); observe(h,MC_COMPARE_BYTE,mode,2);
    if((int8_t)mode<2) { byte(h,MESSAGE_MODE,0); byte(h,MESSAGE_READY,1); byte(h,MESSAGE_SEGMENT_DELAY,1); }
    else {
        byte(h,MESSAGE_EVENT_COUNT,0); byte(h,MESSAGE_EVENT_CURSOR,0); byte(h,0xc457f7u,0);
        observe(h,MC_MESSAGE_CLEAR_BEGIN,0,0);
        for(i=0;i<10;++i) { wr_u8(MESSAGE_EVENTS+i,0); wr_u8(MESSAGE_BUFFER+i,0); observe(h,MC_MESSAGE_CLEAR_BYTE,0,0); }
        observe(h,MC_MESSAGE_CLEAR_DONE,0,0); w.character=0; assigned=1; byte(h,MESSAGE_READY,0);
    }
    byte(h,MESSAGE_ACTIVE,1); byte(h,MESSAGE_FLAGS,rd_u8(w.text)); set_text(h,&w,w.text+1);
    w.control=secondary_byte(h,w.text); set_text(h,&w,w.text+1); refresh_colour(h,&w);
    if(bit(h,selector,14)) { byte(h,MESSAGE_PACE,2); goto emit_character; }
    observe(h,MC_SECONDARY_AND_WORD,15,0); w.control&=15; byte(h,MESSAGE_PACE,(uint8_t)w.control); store_cursor(h,w,MESSAGE_RESTART_CURSOR); goto emit_character;
active_sequence:
    value=rd_u16(MESSAGE_DELAY); observe(h,MC_TEST_WORD,value,0); if((int16_t)value>0) goto count_delay;
    mode=rd_u8(MESSAGE_MODE); observe(h,MC_COMPARE_BYTE,mode,2); if((int8_t)mode>=2) goto command_event;
    value=subtract_word(h,MESSAGE_WAIT,1); if((int16_t)value>1) return message_result(w,assigned);
    if(!bit(h,rd_u8(MESSAGE_FLAGS),0)) { w=load_cursor(h,w,MESSAGE_RESTART_CURSOR); goto reset_repeat; }
    value=subtract_word(h,MESSAGE_REPEAT,1); if((int16_t)value<=1) goto reset_repeat;
    goto advance_ready;
command_event:
    flags=rd_u8(MESSAGE_BUFFER_SPACE); observe(h,MC_TEST_BYTE,flags,0); if((int8_t)flags<0) goto finish_sequence;
    set_lookup(h,&w,MESSAGE_EVENTS); cursor=rd_u8(MESSAGE_EVENT_CURSOR); observe(h,MC_INDEX_BYTE,cursor,0); observe(h,MC_INDEX_EXT_WORD,(uint16_t)(int16_t)(int8_t)cursor,0);
    event=rd_u8(w.lookup+(uint32_t)(int32_t)(int8_t)cursor); w.character=event; assigned=1; observe(h,MC_CHARACTER_BYTE,event,0); if(!event) return message_result(w,assigned);
    observe(h,MC_COMPARE_BYTE,event,0x44); if(event==0x44) goto finish_sequence;
    flags=rd_u8(MESSAGE_READY); observe(h,MC_TEST_BYTE,flags,0); if((int8_t)flags<=0) goto reset_repeat;
    subtract_byte(h,MESSAGE_READY,1); subtract_byte(h,MESSAGE_EVENT_COUNT,1);
    byte(h,w.lookup+(uint32_t)(int32_t)(int8_t)cursor,0); observe(h,MC_INDEX_INCREMENT,1,0); cursor=(uint8_t)(cursor+1);
    observe(h,MC_COMPARE_BYTE,cursor,10); if((int8_t)cursor>=10) { cursor=0; observe(h,MC_INDEX_BYTE,0,0); }
    byte(h,MESSAGE_EVENT_CURSOR,cursor); goto restore_live;
finish_sequence:
    mode=rd_u8(MESSAGE_MODE); observe(h,MC_COMPARE_BYTE,mode,3);
    if(mode==3) { w=consume_message(h,MC_ACCEPT_TYPED_CODE,w); byte(h,MESSAGE_MODE,(uint8_t)(rd_u8(MESSAGE_MODE)|0x80)); return message_result(w,assigned); }
    byte(h,MESSAGE_MODE,(uint8_t)(mode&0xfd)); w=consume_message(h,MC_FINISH_SEQUENCE,w); byte(h,0xc457d5u,0xff); return message_result(w,assigned);
advance_ready:
    flags=rd_u8(MESSAGE_READY); observe(h,MC_TEST_BYTE,flags,0); if((int8_t)flags<=0) return message_result(w,assigned);
    subtract_byte(h,MESSAGE_READY,1); goto restore_live;
reset_repeat:
    w.control=secondary_byte(h,MESSAGE_PACE); observe(h,MC_SECONDARY_EXT_WORD,(uint16_t)(int16_t)(int8_t)w.control,0); word(h,MESSAGE_REPEAT,(uint16_t)(int16_t)(int8_t)w.control);
    byte(h,MESSAGE_READY,1); observe(h,MC_COMPARE_BYTE,(uint8_t)w.character,0x41); if((uint8_t)w.character!=0x41) goto restore_live;
    flags=rd_u8(MESSAGE_BUFFER_SIZE); observe(h,MC_TEST_BYTE,flags,0); if((int8_t)flags<=0) goto restore_live;
    subtract_byte(h,MESSAGE_BUFFER_SIZE,1); add_byte(h,MESSAGE_BUFFER_SPACE,1);
    set_lookup(h,&w,rd_u32(PLAYER_RECORD)); set_lookup(h,&w,w.lookup+30); cursor=rd_u8(MESSAGE_BUFFER_SIZE);
    observe(h,MC_INDEX_BYTE,cursor,0); observe(h,MC_INDEX_EXT_WORD,(uint16_t)(int16_t)(int8_t)cursor,0); byte(h,w.lookup+(uint32_t)(int32_t)(int8_t)cursor,0);
    set_lookup(h,&w,MESSAGE_BUFFER); set_lookup(h,&w,w.lookup+(uint32_t)(int32_t)(int8_t)cursor);
    byte(h,w.lookup,0); set_lookup(h,&w,w.lookup+1); byte(h,w.lookup,0);
    w=load_cursor(h,w,MESSAGE_LIVE_CURSOR); set_positions(h,&w,w.positions-4); goto emit_character;
restore_live:
    w=load_cursor(h,w,MESSAGE_LIVE_CURSOR);
emit_character:
    set_glyph(h,&w,0xc3d8fcu); w.planes=rd_u32(0xc456b6u); observe(h,MC_PLANES,w.planes,0); observe(h,MC_STRIDE_WORD,450,0);
    mode=rd_u8(MESSAGE_MODE); observe(h,MC_COMPARE_BYTE,mode,2);
    if((int8_t)mode>=2) {
        observe(h,MC_TEST_BYTE,(uint8_t)w.character,0); if(!(uint8_t)w.character) goto publish_live;
        observe(h,MC_CHARACTER_AND_WORD,255,0); w.character&=255; set_lookup(h,&w,0xc331ceu);
        w.character=rd_u8(w.lookup+w.character); assigned=1; observe(h,MC_CHARACTER_BYTE,w.character,0); if(!w.character) goto publish_live;
        observe(h,MC_COMPARE_BYTE,w.character,32); if((int8_t)w.character<32) goto prepare_character;
        flags=rd_u8(MESSAGE_READY); observe(h,MC_TEST_BYTE,flags,0); if((int8_t)flags<=0) goto prepare_character;
        observe(h,MC_COMPARE_BYTE,rd_u8(MESSAGE_MODE),3);
        if(rd_u8(MESSAGE_MODE)!=3) {
            set_lookup(h,&w,rd_u32(PLAYER_RECORD)); set_lookup(h,&w,w.lookup+30); cursor=rd_u8(MESSAGE_BUFFER_SIZE);
            observe(h,MC_INDEX_BYTE,cursor,0); observe(h,MC_INDEX_EXT_WORD,(uint16_t)(int16_t)(int8_t)cursor,0); byte(h,w.lookup+(uint32_t)(int32_t)(int8_t)cursor,(uint8_t)w.character);
        }
        add_byte(h,MESSAGE_BUFFER_SIZE,1); previous=subtract_byte(h,MESSAGE_BUFFER_SPACE,1);
        if((int8_t)previous<1) { w.character=32; assigned=1; observe(h,MC_CHARACTER_LONG,32,0); }
    } else {
        w.character=rd_u8(w.text); assigned=1; observe(h,MC_CHARACTER_BYTE,w.character,0);
        if((int8_t)w.character<0) goto next_selector;
        if(!w.character) goto next_segment;
    }
prepare_character:
    value=rd_u16(w.positions); observe(h,MC_INDEX_WORD,value,0); observe(h,MC_OFFSET_EXT_LONG,(uint32_t)(int32_t)(int16_t)value,0);
    w.offset=(uint32_t)(int32_t)(int16_t)value+w.origin; observe(h,MC_OFFSET_ADD_LONG,w.origin,0);
    w.style=rd_u16(w.positions+2); observe(h,MC_STYLE_WORD,w.style,0); w.colour=rd_u16(MESSAGE_COLOUR); observe(h,MC_COLOUR_WORD,w.colour,0);
    w.character&=255; observe(h,MC_CHARACTER_AND_WORD,255,0); observe(h,MC_COMPARE_BYTE,(uint8_t)w.character,8);
    if((uint8_t)w.character==8) {
        flags=rd_u8(MESSAGE_READY); observe(h,MC_TEST_BYTE,flags,0); if((int8_t)flags<=0) set_positions(h,&w,w.positions-4);
        w.character=92; assigned=1; observe(h,MC_CHARACTER_LONG,92,0); w.colour=0; observe(h,MC_COLOUR_WORD,0,0);
    }
    observe(h,MC_CHARACTER_SUB_WORD,32,0); w.character=(uint16_t)(w.character-32);
    if(bit(h,rd_u8(MESSAGE_FLAGS),0)) {
        observe(h,MC_TEST_WORD,(uint16_t)w.character,0);
        if(!w.character) {
            observe(h,MC_COMPARE_BYTE,rd_u8(MESSAGE_MODE),2);
            if(rd_u8(MESSAGE_MODE)!=2) {
                flags=primary_byte(h,MESSAGE_FLAGS); observe(h,MC_PRIMARY_AND_BYTE,2,0);
                if(flags&2) { set_positions(h,&w,w.positions+4); set_text(h,&w,w.text+1); store_cursor(h,w,MESSAGE_LIVE_CURSOR); goto reset_repeat; }
                goto draw_character;
            }
        }
        if(test_byte(h,MESSAGE_READY)) {
            w.control=secondary_byte(h,0xc457d7u);
            if(!w.control) { observe(h,MC_SECONDARY_LONG,2,0); w=consume_message(h,MC_SEQUENCE_TONE,w); }
            else { observe(h,MC_SECONDARY_SUB_BYTE,1,0); if((uint8_t)(w.control-1)==0) { observe(h,MC_SECONDARY_LONG,2,0); w=consume_message(h,MC_SEQUENCE_TONE,w); } }
        }
    }
draw_character:
    observe(h,MC_CHARACTER_DOUBLE,0,0); value=(uint16_t)(w.character*2);
    w.glyph+=(uint32_t)(int32_t)rd_s16(w.glyph+(uint32_t)(int32_t)(int16_t)value); observe(h,MC_GLYPH_SELECT,w.glyph,0); w.character=w.glyph;
    for(i=0;i<4;++i) {
        if(i==0) { bit(h,w.colour,3); observe(h,MC_PLANE_STYLE,(w.colour&8)?0xbfa:0xb0a,w.style); observe(h,MC_PLANE_ARGUMENT,rd_u32(w.planes),w.offset); }
        else { observe(h,MC_PLANE_ARGUMENT,rd_u32(w.planes+4*i),w.offset); bit(h,w.colour,3-i); observe(h,MC_PLANE_STYLE,(w.colour&(1u<<(3-i)))?0xbfa:0xb0a,w.style); }
        w=consume_message(h,(enum MainControlChild)(MC_GLYPH_FIRST+i),w);
    }
    if(!bit(h,rd_u8(MESSAGE_FLAGS),0)) { set_positions(h,&w,w.positions+4); set_text(h,&w,w.text+1); store_cursor(h,w,MESSAGE_LIVE_CURSOR); goto reset_repeat; }
    flags=rd_u8(MESSAGE_READY); observe(h,MC_TEST_BYTE,flags,0); if((int8_t)flags<=0) { set_positions(h,&w,w.positions+4); set_text(h,&w,w.text+1); }
publish_live:
    store_cursor(h,w,MESSAGE_LIVE_CURSOR); return message_result(w,assigned);
next_selector:
    set_lookup(h,&w,MESSAGE_SELECTORS); cursor=primary_byte(h,MESSAGE_CURSOR); observe(h,MC_PRIMARY_EXT_WORD,(uint16_t)(int16_t)(int8_t)cursor,0);
    value=rd_u16(w.lookup+2+(uint32_t)(int32_t)(int8_t)cursor); observe(h,MC_TEST_WORD,value,0);
    if(value) { add_byte(h,MESSAGE_CURSOR,2); goto reset_active; }
    byte(h,MESSAGE_MODE,1); word(h,MESSAGE_DELAY,50); byte(h,MESSAGE_CURSOR,0); longword(h,MESSAGE_SELECTORS,0); return message_result(w,assigned);
next_segment:
    if(!bit(h,rd_u8(MESSAGE_FLAGS),0)) {
        previous=subtract_byte(h,MESSAGE_SEGMENT_DELAY,1);
        if((int8_t)previous>=1) {
            w=load_cursor(h,w,MESSAGE_RESTART_CURSOR); store_cursor(h,w,MESSAGE_LIVE_CURSOR); w.control=secondary_byte(h,0xc457d7u);
            if(w.control) { observe(h,MC_SECONDARY_SUB_BYTE,1,0); if((uint8_t)(w.control-1)!=0) return message_result(w,assigned); }
    observe(h,MC_SECONDARY_LONG,2,0); consume_message(h,MC_SEQUENCE_RESTART_TONE,w); return message_result(w,assigned);
        }
    }
    set_text(h,&w,w.text+1); value=primary_byte(h,w.text); set_text(h,&w,w.text+1); observe(h,MC_PRIMARY_EXT_WORD,(uint16_t)(int16_t)(int8_t)value,0);
    observe(h,MC_PRIMARY_SHIFT_WORD,2,0); word(h,MESSAGE_WAIT,(uint16_t)((int16_t)(int8_t)value*4)); flags=rd_u8(w.text); byte(h,MESSAGE_FLAGS,flags);
    if((int8_t)flags>=0) { set_text(h,&w,w.text+1); byte(h,MESSAGE_SEGMENT_DELAY,1); w.control=secondary_byte(h,w.text); set_text(h,&w,w.text+1); refresh_colour(h,&w); }
    word(h,MESSAGE_REPEAT,0xffff); set_origin(h,&w,w.origin+rd_u32(MESSAGE_RESTART_STRIDE)); observe(h,MC_SECONDARY_AND_WORD,15,0); w.control&=15;
    set_lookup(h,&w,MESSAGE_SELECTORS); cursor=primary_byte(h,MESSAGE_CURSOR); observe(h,MC_PRIMARY_EXT_WORD,(uint16_t)(int16_t)(int8_t)cursor,0);
    selector=primary_word(h,w.lookup+(uint32_t)(int32_t)(int8_t)cursor);
    if(bit(h,selector,14)) byte(h,MESSAGE_PACE,2); else byte(h,MESSAGE_PACE,(uint8_t)w.control);
    set_positions(h,&w,0xc41066u); store_cursor(h,w,MESSAGE_RESTART_CURSOR); goto publish_live;
count_delay:
    longword(h,MESSAGE_RESTART_STRIDE,440); value=rd_u16(MESSAGE_DELAY); observe(h,MC_TEST_WORD,value,0);
    if((int16_t)value>0) { value=subtract_word(h,MESSAGE_DELAY,1); if((int16_t)value<=1) byte(h,MESSAGE_MODE,(uint8_t)(rd_u8(MESSAGE_MODE)|0x80)); }
reset_active:
    byte(h,MESSAGE_ACTIVE,0);
    return message_result(w,assigned);
}
