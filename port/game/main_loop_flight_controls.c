/* Complete original main-loop control/flight owners and their real consumers.
 * Source-width arithmetic and actual child outputs remain part of behavior. */
#include "main_loop_flight_controls.h"
#include <stdlib.h>
static void observe(const FlightHooks *h,enum FlightPhase p,uint32_t v,uint32_t other) {
    if(h && h->observe) h->observe(h->context,p,v,other);
}
static FlightWorking consume(const FlightHooks *h,enum FlightChild child) {
    if(h && h->consume) return h->consume(h->context,child);
    abort();
}
static FlightWorking consume_input(const FlightHooks *h,enum FlightChild child,FlightWorking w) {
    if(h && h->consume_values) return h->consume_values(h->context,child,w);
    return consume(h,child);
}
static uint32_t narrow_word(uint32_t old,uint16_t v) { return (old&0xffff0000u)|v; }
static uint32_t narrow_byte(uint32_t old,uint8_t v) { return (old&0xffffff00u)|v; }
static uint32_t sign_word(uint32_t v) { return (uint32_t)(int32_t)(int16_t)v; }
static uint32_t shift_right(uint32_t v,unsigned count) { count&=63u; return (uint32_t)((int32_t)v>>(count<32?count:31)); }
static uint32_t swap_words(uint32_t v) { return (v<<16)|(v>>16); }
static gaddr lookup(const FlightHooks *h,gaddr a) { observe(h,FC_CURRENT,a,0); return a; }
static uint16_t value_word(const FlightHooks *h,gaddr a) { uint16_t v=rd_u16(a); observe(h,FC_VALUE_WORD,v,0); return v; }
static uint8_t value_byte(const FlightHooks *h,gaddr a) { uint8_t v=rd_u8(a); observe(h,FC_VALUE_BYTE,v,0); return v; }
static uint32_t value_long(const FlightHooks *h,gaddr a) { uint32_t v=rd_u32(a); observe(h,FC_VALUE_LONG,v,0); return v; }
static void byte(const FlightHooks *h,gaddr a,uint8_t v) { wr_u8(a,v); observe(h,FC_STORE_BYTE,v,0); }
static void word(const FlightHooks *h,gaddr a,uint16_t v) { wr_u16(a,v); observe(h,FC_STORE_WORD,v,0); }
static void longword(const FlightHooks *h,gaddr a,uint32_t v) { wr_u32(a,v); observe(h,FC_STORE_LONG,v,0); }
static int test_byte(const FlightHooks *h,gaddr a) { uint8_t v=rd_u8(a); observe(h,FC_TEST_BYTE,v,0); return v!=0; }
static int test_word(const FlightHooks *h,gaddr a) { uint16_t v=rd_u16(a); observe(h,FC_TEST_WORD,v,0); return v!=0; }
static int bit(const FlightHooks *h,uint32_t v,unsigned index) { observe(h,FC_BIT_TEST,v,index); return (v&(1u<<index))!=0; }
static uint8_t memory_bit(const FlightHooks *h,gaddr a,unsigned index,enum FlightPhase phase) {
    uint8_t old=rd_u8(a),v=phase==FC_MEMORY_BIT_SET?(uint8_t)(old|(1u<<index)):phase==FC_MEMORY_BIT_CLEAR?(uint8_t)(old&~(1u<<index)):(uint8_t)(old^(1u<<index));
    wr_u8(a,v); observe(h,phase,old,index); return old;
}
static void add_word(const FlightHooks *h,gaddr a,uint16_t n) { uint16_t v=rd_u16(a); wr_u16(a,(uint16_t)(v+n)); observe(h,FC_MEMORY_ADD_WORD,v,n); }
static void add_long(const FlightHooks *h,gaddr a,uint32_t n) { uint32_t v=rd_u32(a); wr_u32(a,v+n); observe(h,FC_MEMORY_ADD_LONG,v,n); }
static void subtract_long(const FlightHooks *h,gaddr a,uint32_t n) { uint32_t v=rd_u32(a); wr_u32(a,v-n); observe(h,FC_MEMORY_SUB_LONG,v,n); }
static void and_byte(const FlightHooks *h,gaddr a,uint8_t mask) { byte(h,a,rd_u8(a)&mask); }
static void or_byte(const FlightHooks *h,gaddr a,uint8_t mask) { byte(h,a,rd_u8(a)|mask); }
void advance_main_loop_flight_controls(gaddr frame,const FlightHooks *h) {
    FlightWorking w={0}; gaddr record,pointer; uint16_t index,v; uint8_t code,status; uint32_t n,m,threshold,base; unsigned i,scale;
    index=rd_u16(0xc459b4u); base=0xc46184u+((uint32_t)(int32_t)(int16_t)index<<9);
    observe(h,FC_CONTROL_SETUP,frame,0);
    wr_u16(frame-36,index); wr_u32(0xc18210u,base); wr_u32(frame-40,base);
    wr_u16(0xc45aa0u,rd_u16(base+150)); wr_u16(0xc45aa0u,rd_u16(base+150)); wr_u16(0xc45aa2u,rd_u16(base+156)); wr_u16(0xc45aa4u,rd_u16(base+162));
    wr_u16(frame-34,(uint16_t)(0u-rd_u16(base+110))); wr_u32(frame-44,base+2); wr_u32(frame-48,base+4); wr_u32(frame-52,base+32);
    w.record=base;
    w.value=(uint32_t)(int32_t)(int16_t)(0u-rd_u16(base+110));
    w.z=(uint32_t)(int32_t)rd_s16(base+150);
    w.rate=(uint32_t)(int32_t)rd_s16(base+156);
    w.depth=(uint32_t)(int32_t)rd_s16(base+162);
    consume_input(h,FC_NORMALISE_CONTROL,w);
    word(h,frame-2,rd_u16(0xc45a4cu)); word(h,frame-4,rd_u16(0xc45a4eu)); word(h,frame-6,rd_u16(0xc45a50u));
    pointer=lookup(h,rd_u32(frame-44)); v=value_word(h,pointer);
    if(!bit(h,v,7)) {
        record=lookup(h,rd_u32(0xc18210u)); n=value_long(h,record+62); observe(h,FC_VALUE_ASR_LONG,2,0); n=shift_right(n,2);
        m=rd_u32(record+70); observe(h,FC_SPEED_LONG,m,0); observe(h,FC_SPEED_ASR_LONG,2,0); m=shift_right(m,2);
        word(h,frame-8,(uint16_t)n); word(h,frame-12,(uint16_t)m);
        if(test_byte(h,0xc457a0u) && !test_word(h,frame-36)) {
            n=value_long(h,record+62); observe(h,FC_VALUE_ASR_LONG,6,0); n=shift_right(n,6);
            m=rd_u32(record+70); observe(h,FC_SPEED_LONG,m,0); observe(h,FC_SPEED_ASR_LONG,6,0); m=shift_right(m,6);
            word(h,frame-8,(uint16_t)n); word(h,frame-12,(uint16_t)m);
        }
        v=value_word(h,frame-2); observe(h,FC_VALUE_EXT_LONG,sign_word(v),0); m=rd_u16(frame-8); observe(h,FC_SPEED_WORD,m,0); observe(h,FC_SPEED_EXT_LONG,sign_word(m),0);
        w.value=sign_word(v);w.speed=sign_word(m);
        w=consume_input(h,FC_ATTENUATE_X,w);
        m=rd_u16(frame-6); observe(h,FC_SPEED_WORD,m,0); observe(h,FC_SPEED_EXT_LONG,sign_word(m),0);
        n=rd_u16(frame-12); observe(h,FC_TURN_WORD,n,0); observe(h,FC_TURN_EXT_LONG,sign_word(n),0);
        word(h,frame-2,(uint16_t)w.value);
        w.value=sign_word((uint16_t)m);w.speed=sign_word((uint16_t)n);
        w=consume_input(h,FC_ATTENUATE_Z,w); word(h,frame-6,(uint16_t)w.value);
    }
    record=lookup(h,rd_u32(0xc18210u)); n=value_long(h,record+66); observe(h,FC_VALUE_ASR_LONG,2,0); n=shift_right(n,2); word(h,frame-10,(uint16_t)n);
    if(test_byte(h,0xc457a0u) && !test_word(h,frame-36)) { observe(h,FC_VALUE_ASR_WORD,4,0); word(h,frame-10,(uint16_t)((int16_t)n>>4)); }
    v=value_word(h,frame-4); observe(h,FC_VALUE_EXT_LONG,sign_word(v),0); m=rd_u16(frame-10); observe(h,FC_SPEED_WORD,m,0); observe(h,FC_SPEED_EXT_LONG,sign_word(m),0);
    w.value=sign_word(v);w.speed=sign_word(m);
    w=consume_input(h,FC_ATTENUATE_Y,w); code=rd_u8(0xc4579fu); observe(h,FC_SPEED_BYTE,code,0); word(h,frame-4,(uint16_t)w.value);
    if(!bit(h,code,0)) { record=lookup(h,rd_u32(0xc18210u)); v=rd_u16(record+118); observe(h,FC_SPEED_WORD,v,0); add_word(h,frame-4,v); }
    scale=2;
publish_motion:
    n=sign_word(value_word(h,frame-2)); observe(h,FC_VALUE_EXT_LONG,n,0); observe(h,FC_VALUE_ASL_LONG,scale,0); n<<=scale;
    m=sign_word(rd_u16(frame-4)); observe(h,FC_SPEED_WORD,(uint16_t)m,0); observe(h,FC_SPEED_EXT_LONG,m,0); observe(h,FC_SPEED_ASL_LONG,scale,0); m<<=scale;
    threshold=sign_word(rd_u16(frame-6)); observe(h,FC_TURN_WORD,(uint16_t)threshold,0); observe(h,FC_TURN_EXT_LONG,threshold,0); observe(h,FC_TURN_ASL_LONG,scale,0); threshold<<=scale;
    longword(h,frame-28,scale==2?0x6000:0x60000); longword(h,frame-16,n); longword(h,frame-20,m); longword(h,frame-24,threshold);
    if(scale==2 && test_byte(h,0xc457a0u) && !test_word(h,frame-36)) { scale=6; goto publish_motion; }
    for(i=0;i<3;++i) { n=value_long(h,frame-16-4*i); observe(h,FC_VALUE_NEG_LONG,0,0); n=0u-n; record=lookup(h,rd_u32(0xc18210u)); longword(h,record+62+4*i,n); }
    if(test_byte(h,0xc457aeu)) { observe(h,FC_VALUE_LONG,0,0); longword(h,frame-28,0); longword(h,frame-20,0); }
    if(!test_word(h,frame-36)) {
        code=value_byte(h,0xc4579fu); observe(h,FC_VALUE_SUB_BYTE,3,0);
        if(code==3) { n=value_long(h,frame-28); subtract_long(h,frame-20,n); }
        else { code=value_byte(h,0xc4579fu); observe(h,FC_VALUE_SUB_BYTE,1,0); if(code==1) { n=value_long(h,frame-28); add_long(h,frame-20,n); } }
    }
    pointer=lookup(h,rd_u32(frame-48)); status=value_byte(h,pointer); observe(h,FC_VALUE_AND_BYTE,0xc0,0); status&=0xc0; observe(h,FC_TEST_BYTE,status,0);
    if(status) {
        record=lookup(h,rd_u32(0xc18210u)); code=value_byte(h,record+124); observe(h,FC_VALUE_AND_BYTE,0x70,0); code&=0x70; observe(h,FC_TEST_BYTE,code,0);
        if(code) { threshold=750; record=lookup(h,rd_u32(0xc18210u)); }
        else { code=value_byte(h,record+98); observe(h,FC_COMPARE_BYTE,code,20); threshold=code==20?2700:1800; if(code!=20) record=lookup(h,rd_u32(0xc18210u)); }
        v=value_word(h,record+78); n=sign_word(v); observe(h,FC_VALUE_EXT_LONG,n,0); observe(h,FC_VALUE_ASL_LONG,8,0); n<<=8;
        observe(h,FC_VALUE_ADD_LONG,threshold,0); n+=threshold; longword(h,frame-32,n); observe(h,FC_COMPARE_LONG,n,threshold);
        if((int32_t)n<(int32_t)threshold) longword(h,frame-32,threshold);
    } else {
        record=lookup(h,rd_u32(0xc18210u)); code=value_byte(h,record+98); observe(h,FC_VALUE_AND_BYTE,0xf0,0); code&=0xf0; observe(h,FC_TEST_BYTE,code,0);
        if(!code) longword(h,frame-32,0xfffffe00);
        else {
            record=lookup(h,rd_u32(0xc18210u)); code=value_byte(h,record+124); observe(h,FC_VALUE_AND_BYTE,0x70,0); code&=0x70; observe(h,FC_TEST_BYTE,code,0);
            if(code) longword(h,frame-32,750);
            else { code=value_byte(h,record+98); observe(h,FC_COMPARE_BYTE,code,20); longword(h,frame-32,code==20?2700:1800); }
        }
    }
    record=lookup(h,rd_u32(0xc18210u)); n=value_long(h,record+24); observe(h,FC_VALUE_SUB_LONG,rd_u32(frame-20),0); n-=rd_u32(frame-20);
    record=lookup(h,rd_u32(0xc18210u)); longword(h,record+24,n); threshold=rd_u32(frame-32); observe(h,FC_SPEED_LONG,threshold,0); observe(h,FC_COMPARE_LONG,n,threshold);
    if((int32_t)n>(int32_t)threshold) goto airborne;
    pointer=lookup(h,rd_u32(frame-52)); code=value_byte(h,pointer); observe(h,FC_VALUE_AND_BYTE,0xfb,0); byte(h,pointer,code&0xfb);
    record=lookup(h,rd_u32(0xc18210u)); code=value_byte(h,record+98); observe(h,FC_VALUE_AND_BYTE,0xf0,0); code&=0xf0; observe(h,FC_TEST_BYTE,code,0);
    if(code) { record=lookup(h,rd_u32(0xc18210u)); longword(h,record+24,threshold); }
    pointer=lookup(h,rd_u32(frame-48)); code=value_byte(h,pointer); observe(h,FC_VALUE_AND_BYTE,0xc0,0); code&=0xc0; observe(h,FC_TEST_BYTE,code,0);
    if(!code) {
        pointer=lookup(h,rd_u32(frame-44)); v=value_word(h,pointer);
        if(bit(h,v,7)) { record=lookup(h,rd_u32(0xc18210u)); v=value_word(h,record+76); observe(h,FC_VALUE_AND_WORD,7,0); v&=7; observe(h,FC_TEST_WORD,v,0); if(v) goto grounded; }
        consume_input(h,FC_PROBE_CONTROL,w); pointer=lookup(h,rd_u32(frame-48)); code=value_byte(h,pointer);
        if(bit(h,code,1)) {
            record=lookup(h,rd_u32(0xc18210u)); code=value_byte(h,record+124); byte(h,frame-54,code); observe(h,FC_VALUE_AND_BYTE,0x70,0); observe(h,FC_TEST_BYTE,code&0x70,0);
            if(!(code&0x70)) {
                code=value_byte(h,frame-54); observe(h,FC_VALUE_OR_BYTE,0x80,0); record=lookup(h,rd_u32(0xc18210u)); byte(h,record+124,code|0x80);
                if(!test_word(h,frame-36)) { pointer=lookup(h,rd_u32(frame-52)); code=value_byte(h,pointer); observe(h,FC_VALUE_OR_BYTE,1,0); byte(h,pointer,code|1); }
            }
        }
    }
grounded:
    if(test_word(h,frame-36)) { pointer=lookup(h,rd_u32(frame-44)); v=value_word(h,pointer); observe(h,FC_VALUE_OR_WORD,0x80,0); word(h,pointer,v|0x80); goto check_ground_speed; }
    pointer=lookup(h,rd_u32(frame-40)); v=value_word(h,pointer); if(bit(h,v,9)) goto advance_contact_timer;
    pointer=lookup(h,rd_u32(frame-44)); v=value_word(h,pointer); if(bit(h,v,7)) goto continuing_contact;
    observe(h,FC_VALUE_OR_WORD,0x80,0); word(h,pointer,v|0x80); record=lookup(h,rd_u32(0xc18210u)); word(h,record+38,0);
    consume_input(h,FC_SAMPLE_TOUCHDOWN,w); longword(h,0xc4590cu,rd_u32(0xc45af2u)); observe(h,FC_VALUE_LONG,0,0); byte(h,0xc4579fu,0); observe(h,FC_VALUE_LONG,1,0); byte(h,0xc457c0u,1);
    v=rd_u16(frame-4); observe(h,FC_COMPARE_WORD,v,0x240);
    if((int16_t)v>=0x240) {
        code=value_byte(h,0xc4589au); observe(h,FC_TEST_BYTE,code,0); if(code) goto clear_inherited_height;
        pointer=lookup(h,rd_u32(frame-40)); v=value_word(h,pointer); observe(h,FC_VALUE_OR_WORD,0x200,0); word(h,pointer,v|0x200);
        record=lookup(h,rd_u32(0xc1ab74u)); v=value_word(h,record+70); observe(h,FC_VALUE_ADD_WORD,1,0); ++v;
        record=lookup(h,rd_u32(0xc1ab74u)); word(h,record+70,v); byte(h,0xc457c5u,1); byte(h,0xc45798u,4); goto clear_inherited_height;
    }
    if(!test_word(h,0xc461f2u)) goto clear_inherited_height;
    word(h,0xc461f0u,rd_u16(0xc461f2u)); byte(h,0xc458b0u,0xfe); pointer=lookup(h,rd_u32(frame-48)); code=value_byte(h,pointer);
    if(!bit(h,code,1)) { record=lookup(h,rd_u32(0xc18210u)); code=value_byte(h,record+124); observe(h,FC_VALUE_AND_BYTE,0x70,0); code&=0x70; observe(h,FC_TEST_BYTE,code,0); if(!code) goto touchdown_tone; }
    record=lookup(h,rd_u32(0xc18210u)); v=value_word(h,record+110); observe(h,FC_COMPARE_WORD,v,0x3c0);
    if((int16_t)v>=0x3c0) { pointer=lookup(h,rd_u32(frame-44)); v=value_word(h,pointer); observe(h,FC_VALUE_OR_WORD,0x1000,0); word(h,pointer,v|0x1000); word(h,0xc4fdd2u,rd_u16(0xc459b6u)); }
touchdown_tone:
    v=rd_u16(frame-4); observe(h,FC_COMPARE_WORD,v,0x180); consume_input(h,(int16_t)v>0x180?FC_TOUCHDOWN_FAST_TONE:FC_TOUCHDOWN_SLOW_TONE,w); goto clear_inherited_height;
continuing_contact:
    v=value_word(h,0xc461f2u); observe(h,FC_TEST_WORD,v,0);
    if(v) {
        v=value_word(h,0xc458d2u); if(!bit(h,v,1)) goto clear_inherited_height;
        byte(h,0xc457c0u,3); memory_bit(h,0xc45b56u,1,FC_MEMORY_BIT_SET); observe(h,FC_VALUE_AND_WORD,0xfffd,0); word(h,0xc458d2u,v&0xfffd); goto clear_inherited_height;
    }
    v=value_word(h,0xc458d2u);
    if(!bit(h,v,1)) {
        observe(h,FC_VALUE_LONG,4,0); byte(h,0xc457c0u,4); v=value_word(h,0xc458cau);
        if(bit(h,v,8)) {
            observe(h,FC_VALUE_AND_WORD,0xfeff,0); word(h,0xc458cau,v&0xfeff); pointer=lookup(h,rd_u32(frame-48)); code=value_byte(h,pointer);
            if(bit(h,code,2)) consume_input(h,FC_RESET_CONTROL,w);
            else { pointer=lookup(h,rd_u32(frame-44)); v=value_word(h,pointer); observe(h,FC_VALUE_AND_WORD,0xc000,0); v&=0xc000; observe(h,FC_COMPARE_WORD,v,0xc000); if(v==0xc000) consume_input(h,FC_RESET_CONTROL,w); }
        }
        v=value_word(h,0xc458d2u); observe(h,FC_VALUE_OR_WORD,2,0); word(h,0xc458d2u,v|2);
    }
    n=rd_u32(0xc461f6u); observe(h,FC_TEST_LONG,n,0); if(n) goto clear_inherited_height;
    v=value_word(h,0xc458ceu); observe(h,FC_VALUE_OR_WORD,0x4000,0); word(h,0xc458ceu,v|0x4000); goto clear_inherited_height;
advance_contact_timer:
    pointer=lookup(h,rd_u32(frame-40)); v=value_word(h,pointer); if(bit(h,v,9)) goto clear_inherited_height;
    v=value_word(h,0xc458ceu); if(!bit(h,v,14)) goto clear_inherited_height;
    n=value_long(h,0xc461f6u); observe(h,FC_VALUE_ADD_LONG,400,0); n+=400; longword(h,0xc461f6u,n); observe(h,FC_COMPARE_LONG,n,0x61a80000);
    if((int32_t)n<0x61a80000) { v=value_word(h,0xc458d0u); if(!bit(h,v,4)) { observe(h,FC_VALUE_OR_WORD,16,0); word(h,0xc458d0u,v|16); } }
    else {
        v=value_word(h,0xc458ceu); observe(h,FC_VALUE_AND_WORD,0xbfff,0); word(h,0xc458ceu,v&0xbfff);
        v=value_word(h,0xc458d0u); observe(h,FC_VALUE_AND_WORD,0xffef,0); word(h,0xc458d0u,v&0xffef); longword(h,0xc461f6u,0x61a80000);
    }
clear_inherited_height:
    longword(h,0xc461c6u,0);
check_ground_speed:
    record=lookup(h,rd_u32(0xc18210u)); v=value_word(h,record+110); observe(h,FC_COMPARE_WORD,v,0x3c0); if((int16_t)v>=0x3c0) return;
    pointer=lookup(h,rd_u32(frame-44)); v=value_word(h,pointer); observe(h,FC_VALUE_AND_WORD,0xefff,0); word(h,pointer,v&0xefff); return;
airborne:
    pointer=lookup(h,rd_u32(frame-44)); v=value_word(h,pointer);
    if(!bit(h,v,7)) {
        n=value_long(h,0xc456fau); record=lookup(h,rd_u32(0xc18210u)); m=rd_u32(record+24); observe(h,FC_SPEED_LONG,m,0); observe(h,FC_COMPARE_LONG,m,n);
        if((int32_t)m>(int32_t)n) { record=lookup(h,rd_u32(0xc18210u)); longword(h,record+24,n); } return;
    }
    observe(h,FC_VALUE_AND_WORD,0xff7f,0); word(h,pointer,v&0xff7f); pointer=lookup(h,rd_u32(frame-48)); code=value_byte(h,pointer);
    if(bit(h,code,3)) { observe(h,FC_VALUE_AND_BYTE,0xf7,0); byte(h,pointer,code&0xf7); byte(h,0xc457c0u,4); }
    if(!test_word(h,frame-36)) {
        consume_input(h,FC_SAMPLE_TAKEOFF,w); n=value_long(h,0xc4590cu); m=rd_u32(0xc45af2u); observe(h,FC_SPEED_LONG,m,0); observe(h,FC_SPEED_SUB_LONG,n,0); m-=n;
        longword(h,0xc4590cu,m); add_long(h,0xc45910u,m); longword(h,0xc4590cu,0);
        record=lookup(h,rd_u32(0xc18210u)); code=value_byte(h,record+33); observe(h,FC_VALUE_OR_BYTE,1,0); record=lookup(h,rd_u32(0xc18210u)); byte(h,record+33,code|1);
        pointer=lookup(h,rd_u32(frame-44)); v=value_word(h,pointer); observe(h,FC_VALUE_AND_WORD,0xefff,0); word(h,pointer,v&0xefff);
        v=value_word(h,0xc458d2u); observe(h,FC_VALUE_AND_WORD,0xfffd,0); word(h,0xc458d2u,v&0xfffd);
        v=value_word(h,0xc458cau); observe(h,FC_VALUE_OR_WORD,0x100,0); word(h,0xc458cau,v|0x100);
    } else consume_input(h,FC_REQUEST_CONTROL,w);
    pointer=lookup(h,rd_u32(frame-48)); code=value_byte(h,pointer); observe(h,FC_VALUE_AND_BYTE,0xfd,0); byte(h,pointer,code&0xfd);
}
void begin_main_loop_mission_reset(const FlightHooks *h) {
    FlightWorking w; uint8_t attempts; unsigned slot,i;
    observe(h,FC_VALUE_SWAP,0,0); observe(h,FC_VALUE_WORD,0x4005,0); w=consume(h,FC_RESET_MESSAGE); (void)w; observe(h,FC_VALUE_SWAP,0,0);
    observe(h,FC_COMPARE_BYTE,rd_u8(0xc458a6u),6);
    if(rd_u8(0xc458a6u)==6) {
        attempts=rd_u8(0xc45798u); observe(h,FC_X_BYTE,attempts,0); observe(h,FC_X_SUB_BYTE,1,0); attempts=(uint8_t)(attempts-1);
        if(attempts) { observe(h,FC_X_ADD_BYTE,2,0); attempts=(uint8_t)(attempts+2); if(attempts) byte(h,0xc45798u,0); }
    }
    longword(h,0xc461f6u,0x61a800); byte(h,0xc461e3u,0x24); word(h,0xc461e4u,500);
    byte(h,0xc45843u,3); byte(h,0xc45844u,3); byte(h,0xc4584cu,16); byte(h,0xc4584du,16);
    byte(h,0xc4588bu,0); byte(h,0xc4588du,0); byte(h,0xc4588cu,0); word(h,0xc458c2u,0);
    for(slot=0;slot<3;++slot) for(i=0;i<41;++i) wr_u32(0xc46384u+512*slot+4*i,0);
    observe(h,FC_RESET_SLOTS,0,0);
}
void normalise_main_loop_control_vector(gaddr frame,const FlightHooks *h) {
    FlightWorking w={0}; uint32_t factor,length,quotient; uint16_t sign; unsigned shift;
    observe(h,FC_NORMALISE_ARGUMENTS,frame,0);
    w.value=rd_u32(frame+8); w.z=rd_u32(frame+12); w.rate=rd_u32(frame+16); w.depth=rd_u32(frame+20);
    sign=rd_u16(frame+8); word(h,frame-2,sign); observe(h,FC_TEST_WORD,w.value,0);
    if(!(uint16_t)w.value) goto zero_vector;
    if((int16_t)w.value<0) { observe(h,FC_VALUE_NEG_WORD,0,0); w.value=narrow_word(w.value,(uint16_t)(0u-w.value)); }
    observe(h,FC_TURN_WORD,w.z,0); if((int16_t)w.z<0) observe(h,FC_TURN_NEG_WORD,0,0);
    observe(h,FC_X_WORD,w.rate,0); if((int16_t)w.rate<0) observe(h,FC_X_NEG_WORD,0,0);
    observe(h,FC_Y_WORD,w.depth,0); if((int16_t)w.depth<0) observe(h,FC_Y_NEG_WORD,0,0);
    w=consume_input(h,FC_NORMALISE_LENGTH,w); if(w.zero) goto zero_vector;
    observe(h,FC_TURN_LONG,8,0); shift=8; factor=sign_word(w.value); observe(h,FC_VALUE_EXT_LONG,factor,0); length=w.speed;
    for(;;) {
        observe(h,FC_COMPARE_LONG,factor,length); if((int32_t)factor>(int32_t)length) break;
        observe(h,FC_VALUE_ASL_LONG,2,0); factor<<=2; observe(h,FC_TURN_ADD_WORD,2,0); shift=(uint16_t)(shift+2);
    }
    for(;;) {
        observe(h,FC_VALUE_ASR_LONG,2,0); factor=shift_right(factor,2); observe(h,FC_TURN_SUB_WORD,2,0); shift=(uint16_t)(shift-2);
        observe(h,FC_COMPARE_WORD,shift,1); if((int16_t)shift<=1) break;
        observe(h,FC_COMPARE_LONG,factor,length); if((int32_t)factor<=(int32_t)length) break;
    }
    observe(h,FC_VALUE_ASL_LONG,2,0); factor<<=2; observe(h,FC_TURN_ADD_WORD,2,0); shift=(uint16_t)(shift+2);
    observe(h,FC_VALUE_ASL_LONG,8,0); factor<<=8;
    if(!(uint16_t)length) {
        if(!h || !h->divide_exception) abort(); w=h->divide_exception(h->context); quotient=w.value; shift=(uint16_t)w.turn;
    } else {
        quotient=factor/(uint16_t)length; quotient=quotient>0xffff?factor:((factor%(uint16_t)length)<<16)|quotient;
        observe(h,FC_NORMALISE_DIVIDE,(uint16_t)length,0);
    }
    w.z=shift_right((uint32_t)((int32_t)(int16_t)w.z*(int32_t)(int16_t)quotient),shift);
    w.rate=shift_right((uint32_t)((int32_t)(int16_t)w.rate*(int32_t)(int16_t)quotient),shift);
    w.depth=shift_right((uint32_t)((int32_t)(int16_t)w.depth*(int32_t)(int16_t)quotient),shift);
    observe(h,FC_NORMALISE_PRODUCTS,0,0); observe(h,FC_TEST_WORD,rd_u16(frame-2),0);
    if(rd_s16(frame-2)<0) { w.z=narrow_word(w.z,(uint16_t)(0u-w.z)); w.rate=narrow_word(w.rate,(uint16_t)(0u-w.rate)); w.depth=narrow_word(w.depth,(uint16_t)(0u-w.depth)); observe(h,FC_Z_NEG_WORD,0,0); observe(h,FC_RATE_NEG_WORD,0,0); observe(h,FC_DEPTH_NEG_WORD,0,0); }
    goto publish;
zero_vector:
    w.z=narrow_word(w.z,0); w.rate=narrow_word(w.rate,0); w.depth=narrow_word(w.depth,0);
    observe(h,FC_Z_WORD,0,0); observe(h,FC_RATE_WORD,0,0); observe(h,FC_DEPTH_WORD,0,0);
publish:
    wr_u16(0xc45a4cu,(uint16_t)w.z); wr_u16(0xc45a4eu,(uint16_t)w.rate); wr_u16(0xc45a50u,(uint16_t)w.depth); observe(h,FC_NORMALISE_PUBLISH,0,0);
}
static uint32_t range_axis(gaddr record,unsigned band,unsigned fine,unsigned origin_band,unsigned origin_fine) {
    uint32_t cell=sign_word(rd_u16(record+band)); uint16_t delta=(uint16_t)(cell-rd_u16(record+origin_band));
    cell=narrow_word(cell,delta); cell=shift_right(swap_words(cell),2);
    uint32_t fraction=sign_word((uint16_t)(rd_u16(record+fine)-rd_u16(record+origin_fine))),sum=cell+fraction;
    return (int64_t)(int32_t)cell+(int32_t)fraction<0?0u-sum:sum;
}
void classify_main_loop_record_range(FlightWorking w,const FlightHooks *h) {
    gaddr record=w.record; uint8_t counter; uint16_t range; uint32_t dx,dy,dz; int8_t period;
    counter=rd_u8(record+57); wr_u8(record+57,(uint8_t)(counter+16)); observe(h,FC_MEMORY_ADD_BYTE,counter,16);
    range=rd_u16(record+74); observe(h,FC_COMPARE_WORD,range,0x480);
    if((int16_t)range>=0x480) {
        period=32; observe(h,FC_SPEED_LONG,32,0); observe(h,FC_COMPARE_WORD,rd_u16(record+74),0x900);
        if(rd_s16(record+74)>=0x900) { period=80; observe(h,FC_SPEED_LONG,80,0); }
        counter=value_byte(h,record+57); observe(h,FC_VALUE_AND_BYTE,0xf0,0); counter&=0xf0; observe(h,FC_COMPARE_BYTE,(uint8_t)period,counter);
        if(period>=(int8_t)counter) return;
    }
    and_byte(h,record+57,15); range=rd_u16(record+44); observe(h,FC_TURN_WORD,range,0); if((int16_t)range<0) return;
    observe(h,FC_RANGE_COMPONENTS,record,0); dx=range_axis(record,44,48,6,12); dz=range_axis(record,46,50,8,14);
    dy=rd_u32(record+52)-rd_u32(record+16); if((int64_t)rd_s32(record+52)-rd_s32(record+16)<0) dy=0u-dy;
    observe(h,FC_VALUE_LONG,0x7f00,0); observe(h,FC_COMPARE_LONG,dx,0x7f00); if((int32_t)dx>0x7f00) goto distant;
    observe(h,FC_COMPARE_LONG,dy,0x7f00); if((int32_t)dy>0x7f00) goto distant;
    observe(h,FC_COMPARE_LONG,dz,0x7f00); if((int32_t)dz>0x7f00) goto distant;
    w=consume(h,FC_CLASSIFY_LENGTH); record=w.record; range=(uint16_t)w.speed; word(h,record+74,range); memory_bit(h,record+4,0,FC_MEMORY_BIT_SET);
    observe(h,FC_COMPARE_WORD,range,0x36c0); if((int16_t)range>0x36c0) goto distant_flags;
    observe(h,FC_COMPARE_WORD,range,0x1e00);
    if((int16_t)range<=0x1e00) { observe(h,FC_COMPARE_BYTE,rd_u8(record+122),3); if(rd_u8(record+122)==3) byte(h,record+122,4); }
    else { observe(h,FC_COMPARE_BYTE,rd_u8(record+122),4); if(rd_u8(record+122)==4) byte(h,record+122,3); }
    if(!test_word(h,0xc459b6u)) return;
    observe(h,FC_COMPARE_WORD,range,0x1800); if((int16_t)range>0x1800) goto distant;
    observe(h,FC_COMPARE_WORD,range,0x300);
    if((int16_t)range<=0x300) { and_byte(h,record+99,15); or_byte(h,record+99,16); return; }
    and_byte(h,record+99,15); observe(h,FC_COMPARE_WORD,range,0xc00); or_byte(h,record+99,(int16_t)range>0xc00?48:32); return;
distant:
    word(h,record+74,0x7fff);
distant_flags:
    if(test_word(h,0xc459b6u)) or_byte(h,record+99,0xf0);
}
void update_main_loop_record_sight(FlightWorking w,const FlightHooks *h) {
    uint8_t counter; uint32_t a,b,c,sum; gaddr record=w.record,viewer=w.viewer;
    counter=value_byte(h,record+57); observe(h,FC_VALUE_AND_BYTE,0xf0,0); counter&=0xf0; observe(h,FC_COMPARE_BYTE,counter,16); if(counter!=16) return;
    observe(h,FC_COMPARE_WORD,rd_u16(viewer+74),0x3000); if(rd_s16(viewer+74)>0x3000) goto clear_sight;
    observe(h,FC_SIGHT_DIFFERENCE,record,viewer); w=consume(h,FC_SIGHT_VECTOR); record=w.record; viewer=w.viewer;
    a=(uint32_t)((int32_t)(int16_t)w.z*rd_s16(viewer+150)); b=(uint32_t)((int32_t)(int16_t)w.rate*rd_s16(viewer+156)); c=(uint32_t)((int32_t)(int16_t)w.depth*rd_s16(viewer+162));
    sum=c+a; observe(h,FC_SIGHT_FACING,viewer,0); if((int64_t)(int32_t)sum+(int32_t)b>=0) goto clear_sight;
    sum+=b; observe(h,FC_COMPARE_LONG,sum,0xffd40000); if((int32_t)sum>-0x2c0000) goto clear_sight;
    a=(uint32_t)((int32_t)rd_s16(viewer+150)*rd_s16(record+150)); b=(uint32_t)((int32_t)rd_s16(viewer+156)*rd_s16(record+156)); c=(uint32_t)((int32_t)rd_s16(viewer+162)*rd_s16(record+162));
    sum=c+a; observe(h,FC_SIGHT_ALIGNMENT,record,viewer); if((int64_t)(int32_t)sum+(int32_t)b<0) goto clear_sight;
    sum+=b; observe(h,FC_COMPARE_LONG,sum,0xd000000); if((int32_t)sum<0xd000000) goto clear_sight;
    memory_bit(h,record+4,5,FC_MEMORY_BIT_SET); return;
clear_sight:
    memory_bit(h,record+4,5,FC_MEMORY_BIT_CLEAR);
}
static void working_byte(uint32_t *field,const FlightHooks *h,enum FlightPhase p,uint8_t v) { *field=narrow_byte(*field,v); observe(h,p,v,0); }
static void working_word(uint32_t *field,const FlightHooks *h,enum FlightPhase p,uint16_t v) { *field=narrow_word(*field,v); observe(h,p,v,0); }
static void working_long(uint32_t *field,const FlightHooks *h,enum FlightPhase p,uint32_t v) { *field=v; observe(h,p,v,0); }
static void working_add_word(uint32_t *field,const FlightHooks *h,enum FlightPhase p,uint16_t v) { *field=narrow_word(*field,(uint16_t)(*field+v)); observe(h,p,v,0); }
static void working_sub_word(uint32_t *field,const FlightHooks *h,enum FlightPhase p,uint16_t v) { *field=narrow_word(*field,(uint16_t)(*field-v)); observe(h,p,v,0); }
static int byte_bit(const FlightHooks *h,gaddr a,unsigned n) { return bit(h,rd_u8(a),n&7u); }
/* C23A7E includes its earlier return arms and the complete C243F2 target tail.
 * Working values have named roles; arithmetic remains source-width and every
 * actual child reloads the values and record cursors it really returns. */
FlightWorking advance_main_loop_flight_record(FlightWorking w,const FlightHooks *h) {
    uint32_t saved_value,saved_speed,saved_turn,old,operand; uint16_t value; uint8_t code; int negative; gaddr p;
#define B(field,phase,v) working_byte(&w.field,h,phase,(uint8_t)(v))
#define W(field,phase,v) working_word(&w.field,h,phase,(uint16_t)(v))
#define L(field,phase,v) working_long(&w.field,h,phase,(uint32_t)(v))
#define AW(field,phase,v) working_add_word(&w.field,h,phase,(uint16_t)(v))
#define SW(field,phase,v) working_sub_word(&w.field,h,phase,(uint16_t)(v))
    if(test_byte(h,0xc457aeu)) goto keep_record;
    B(value,FC_VALUE_BYTE,rd_u8(w.record+98)); w.value=narrow_byte(w.value,w.value&0xf0); observe(h,FC_VALUE_AND_BYTE,0xf0,0); observe(h,FC_COMPARE_BYTE,w.value,0);
    if(!(uint8_t)w.value) goto unclassified_record;
    observe(h,FC_COMPARE_BYTE,w.value,0x30); if((uint8_t)w.value==0x30) goto autonomous_motion;
    observe(h,FC_COMPARE_BYTE,w.value,0x20); if((uint8_t)w.value==0x20) goto keep_record;
    value=rd_u16(w.record+76); wr_u16(w.record+76,(uint16_t)(value-1)); observe(h,FC_MEMORY_SUB_WORD,value,1);
    if(!byte_bit(h,w.record,4) || !test_byte(h,0xc4578eu)) goto keep_record;
    if(byte_bit(h,w.record+32,1)) {
        longword(h,w.record+52,0); word(h,w.record+44,rd_u16(w.record+6)); word(h,w.record+46,rd_u16(w.record+8)); word(h,w.record+48,rd_u16(w.record+12)); word(h,w.record+50,rd_u16(w.record+14));
        W(value,FC_VALUE_WORD,rd_u16(w.record+108)); observe(h,FC_COMPARE_WORD,w.value,0x1200);
        if((int16_t)w.value<=0x1200) { AW(value,FC_VALUE_ADD_WORD,0x90); observe(h,FC_COMPARE_WORD,w.value,0x1200); if((int16_t)w.value>0x1200) W(value,FC_VALUE_WORD,0x1200); }
        else { SW(value,FC_VALUE_SUB_WORD,0x90); observe(h,FC_COMPARE_WORD,w.value,0x1200); if((int16_t)w.value<0x1200) W(value,FC_VALUE_WORD,0x1200); }
        word(h,w.record+108,(uint16_t)w.value); word(h,w.record+110,(uint16_t)w.value); word(h,w.record+2,rd_u16(w.record+2)&0xdff7); goto keep_record;
    }
    if(byte_bit(h,w.record+2,0)) goto route_motion;
    value=rd_u16(w.record+44); observe(h,FC_TEST_WORD,value,0); if((int16_t)value<0) goto route_motion;
    w=consume(h,FC_CLASSIFY_RECORD); W(turn,FC_TURN_WORD,rd_u16(w.record+108)); observe(h,FC_COMPARE_BYTE,rd_u8(w.record+56),255);
    if(rd_u8(w.record+56)==255) {
        observe(h,FC_COMPARE_BYTE,rd_u8(w.record+98),20); if(rd_u8(w.record+98)!=20) goto default_heading;
        W(speed,FC_SPEED_WORD,rd_u16(w.record+2)); w.speed=narrow_word(w.speed,w.speed&0x80); observe(h,FC_SPEED_AND_WORD,0x80,0);
        if((uint16_t)w.speed) { W(speed,FC_SPEED_WORD,rd_u16(w.record+108)); observe(h,FC_COMPARE_WORD,w.speed,0x100); if((int16_t)w.speed<0x100) goto no_heading; observe(h,FC_SPEED_ASR_WORD,1,0); w.speed=narrow_word(w.speed,(uint16_t)((int16_t)w.speed>>1)); goto blend_heading; }
        W(speed,FC_SPEED_WORD,0x1680); observe(h,FC_COMPARE_LONG,rd_u32(w.record+16),0x6c0); if(rd_s32(w.record+16)>0x6c0) goto minimum_heading;
        code=rd_u8(w.record+124); observe(h,FC_TEST_BYTE,code,0); if((int8_t)code<0) memory_bit(h,w.record+124,7,FC_MEMORY_BIT_CHANGE);
        W(speed,FC_SPEED_WORD,0x600); goto minimum_heading;
    }
    w.auxiliary=0xc46184u; observe(h,FC_AUXILIARY,w.auxiliary,0); B(speed,FC_SPEED_BYTE,rd_u8(w.record+56)); observe(h,FC_COMPARE_BYTE,w.speed,255);
    if((uint8_t)w.speed!=255) {
        w.speed=narrow_word(w.speed,w.speed&0x7f); observe(h,FC_SPEED_AND_WORD,0x7f,0); w.speed=narrow_word(w.speed,(uint16_t)(w.speed<<8)); observe(h,FC_SPEED_ASL_WORD,8,0);
        AW(speed,FC_SPEED_ADD_WORD,w.speed); w.auxiliary+=(uint32_t)(int32_t)(int16_t)w.speed; observe(h,FC_AUXILIARY,w.auxiliary,0);
        if(!byte_bit(h,w.record+1,6)) { byte(h,w.record+56,0x80); w.auxiliary=0xc46184u; observe(h,FC_AUXILIARY,w.auxiliary,0); }
    }
    W(speed,FC_SPEED_WORD,rd_u16(w.auxiliary+108)); W(value,FC_VALUE_WORD,w.speed);
    if(byte_bit(h,w.record+4,5)) {
        observe(h,FC_COMPARE_WORD,rd_u16(w.record+74),0x240); if(rd_s16(w.record+74)<=0x240) goto double_heading;
        observe(h,FC_COMPARE_WORD,rd_u16(w.record+74),0x600); if(rd_s16(w.record+74)>=0x600) goto reduce_heading; goto minimum_heading;
    }
    if(!byte_bit(h,w.record+2,0)) {
        B(x,FC_X_BYTE,rd_u8(w.record+5)); if((uint8_t)w.x) { observe(h,FC_COMPARE_BYTE,w.x,1); if((uint8_t)w.x!=1) {
            observe(h,FC_COMPARE_WORD,rd_u16(w.record+74),0x360); if(rd_s16(w.record+74)<=0x360) goto minimum_heading;
            observe(h,FC_COMPARE_BYTE,w.x,6); if((uint8_t)w.x==6) goto double_heading; goto reduce_heading;
        } }
    }
    B(x,FC_X_BYTE,rd_u8(w.record+100)); w.x=narrow_word(w.x,w.x&0x60); observe(h,FC_X_AND_WORD,0x60,0);
    observe(h,FC_COMPARE_WORD,rd_u16(w.record+74),0x180); if(rd_s16(w.record+74)<0x180) goto reduce_heading;
    observe(h,FC_COMPARE_WORD,w.x,0x60); if((uint16_t)w.x!=0x60) goto check_reduced_heading;
double_heading:
    observe(h,FC_COMPARE_WORD,rd_u16(w.record+74),0x300); if(rd_s16(w.record+74)>=0x300) goto default_heading;
    AW(speed,FC_SPEED_ADD_WORD,w.speed); observe(h,FC_COMPARE_WORD,w.speed,0x1bc0); if((int16_t)w.speed<=0x1bc0) goto minimum_heading;
default_heading:
    observe(h,FC_COMPARE_BYTE,rd_u8(w.record+98),21); if(rd_u8(w.record+98)==21) { W(speed,FC_SPEED_WORD,0x1ec0); goto minimum_heading; }
    if(byte_bit(h,w.record+1,3)) W(speed,FC_SPEED_WORD,0x15c0); else W(speed,FC_SPEED_WORD,0x1d40); goto minimum_heading;
check_reduced_heading:
    observe(h,FC_COMPARE_WORD,rd_u16(w.record+74),0x300); if(rd_s16(w.record+74)>0x300) goto minimum_heading;
reduce_heading:
    W(value,FC_VALUE_WORD,w.speed); observe(h,FC_VALUE_ASR_WORD,3,0); w.value=narrow_word(w.value,(uint16_t)((int16_t)w.value>>3)); observe(h,FC_COMPARE_WORD,w.value,0x420);
    if((int16_t)w.value>=0x420) W(value,FC_VALUE_WORD,0x420); SW(speed,FC_SPEED_SUB_WORD,w.value);
minimum_heading:
    observe(h,FC_COMPARE_WORD,w.speed,0x900); if((int16_t)w.speed<=0x900) W(speed,FC_SPEED_WORD,0x900);
blend_heading:
    AW(speed,FC_SPEED_ADD_WORD,w.turn); observe(h,FC_SPEED_ASR_WORD,1,0); w.speed=narrow_word(w.speed,(uint16_t)((int16_t)w.speed>>1)); W(value,FC_VALUE_WORD,w.speed); W(x,FC_X_WORD,0x90); W(y,FC_Y_WORD,w.x);
    negative=(int32_t)(int16_t)w.value-(int16_t)w.turn<0; SW(value,FC_VALUE_SUB_WORD,w.turn);
    if(negative) { w.value=narrow_word(w.value,(uint16_t)(0u-w.value)); observe(h,FC_VALUE_NEG_WORD,0,0); w.y=narrow_word(w.y,(uint16_t)(0u-w.y)); observe(h,FC_Y_NEG_WORD,0,0); }
    observe(h,FC_COMPARE_WORD,w.value,w.x); if((int16_t)w.value>=(int16_t)w.x) { AW(turn,FC_TURN_ADD_WORD,w.y); W(speed,FC_SPEED_WORD,w.turn); }
    observe(h,FC_COMPARE_WORD,w.speed,rd_u16(w.record+108)); if((int16_t)w.speed==(int16_t)rd_u16(w.record+108)) goto unchanged_heading;
    L(turn,FC_TURN_LONG,(int16_t)w.speed<(int16_t)rd_u16(w.record+108)?2:1); goto publish_heading;
no_heading:
    byte(h,w.record+43,0);
unchanged_heading:
    L(turn,FC_TURN_LONG,0);
publish_heading:
    B(value,FC_VALUE_BYTE,rd_u8(w.record+101)); w.value=narrow_byte(w.value,w.value&0xfc); observe(h,FC_VALUE_AND_BYTE,0xfc,0); w.value=narrow_byte(w.value,w.value|w.turn); observe(h,FC_VALUE_OR_BYTE,w.turn,0); byte(h,w.record+101,(uint8_t)w.value);
route_motion:
    if(test_byte(h,0xc457aeu)) goto keep_record;
    observe(h,FC_COMPARE_BYTE,rd_u8(w.record+5),8); if(rd_u8(w.record+5)!=8) goto select_record_reference;
    if(byte_bit(h,w.record+2,0)) goto select_record_reference;
    observe(h,FC_COMPARE_WORD,rd_u16(w.record+74),0x480); if(rd_s16(w.record+74)>0x480) goto select_record_reference;
    L(value,FC_VALUE_LONG,0); B(value,FC_VALUE_BYTE,rd_u8(w.record+58)); AW(value,FC_VALUE_ADD_WORD,w.value);
    w.route=0xc295e0u; observe(h,FC_ROUTE,w.route,0); w.route+=(uint32_t)(int32_t)rd_s16(w.route+(uint32_t)(int32_t)(int16_t)w.value); observe(h,FC_ROUTE,w.route,0);
    for(;;) {
        L(value,FC_VALUE_LONG,rd_u32(w.route)); observe(h,FC_COMPARE_LONG,w.value,rd_u32(w.record+44));
        if(w.value==rd_u32(w.record+44)) { L(value,FC_VALUE_LONG,rd_u32(w.route+4)); observe(h,FC_COMPARE_LONG,w.value,rd_u32(w.record+48)); if(w.value==rd_u32(w.record+48)) break; }
        w.route+=10; observe(h,FC_ROUTE,w.route,0); value=rd_u16(w.route); observe(h,FC_TEST_WORD,value,0);
        if((int16_t)value<0) { word(h,0xc4599eu,0x35); w=consume(h,FC_ROUTE_FAULT); goto select_record_reference; }
    }
    value=rd_u16(w.route+10); observe(h,FC_TEST_WORD,value,0);
    if((int16_t)value<0) { if(!(memory_bit(h,w.record+1,7,FC_MEMORY_BIT_CLEAR)&0x80)) goto select_record_reference;
        observe(h,FC_COMPARE_BYTE,rd_u8(w.record+98),21); if(rd_u8(w.record+98)==21) word(h,w.record,rd_u16(w.record)|0x200); }
    else {
        observe(h,FC_ROUTE_LOAD,w.route+10,0); w.value=sign_word(rd_u16(w.route+10)); w.speed=sign_word(rd_u16(w.route+12)); w.turn=sign_word(rd_u16(w.route+14)); w.x=sign_word(rd_u16(w.route+16)); w.y=sign_word(rd_u16(w.route+18));
        wr_u16(w.record+44,(uint16_t)w.value); wr_u16(w.record+46,(uint16_t)w.speed); wr_u16(w.record+48,(uint16_t)w.turn); wr_u16(w.record+50,(uint16_t)w.x); observe(h,FC_ROUTE_STORE,0,0);
        w.y=sign_word(w.y); observe(h,FC_Y_EXT_LONG,w.y,0); longword(h,w.record+52,w.y); word(h,w.record+74,0x7fff);
    }
select_record_reference:
    observe(h,FC_COMPARE_BYTE,rd_u8(w.record+5),8); if(rd_u8(w.record+5)==8) goto finish_view_status;
    W(value,FC_VALUE_WORD,rd_u16(w.record)); W(speed,FC_SPEED_WORD,w.value); w.value=narrow_word(w.value,w.value&8); observe(h,FC_VALUE_AND_WORD,8,0);
    if((uint16_t)w.value) { w.speed=narrow_word(w.speed,w.speed&2); observe(h,FC_SPEED_AND_WORD,2,0); if((uint16_t)w.speed) goto clear_reference_flag; goto keep_record; }
    w.viewer=0xc46184u; observe(h,FC_VIEWER,w.viewer,0); B(value,FC_VALUE_BYTE,rd_u8(w.record+56)); observe(h,FC_COMPARE_BYTE,w.value,255);
    if((uint8_t)w.value==255) goto select_nearest_reference;
    w.value=narrow_word(w.value,w.value&0x7f); observe(h,FC_VALUE_AND_WORD,0x7f,0); w.value=narrow_word(w.value,(uint16_t)(w.value<<8)); observe(h,FC_VALUE_ASL_WORD,8,0); AW(value,FC_VALUE_ADD_WORD,w.value);
    w.viewer+=(uint32_t)(int32_t)(int16_t)w.value; observe(h,FC_VIEWER,w.viewer,0); goto check_reference;
autonomous_motion:
    B(speed,FC_SPEED_BYTE,rd_u8(0xc45788u)); w.speed=narrow_byte(w.speed,w.speed|rd_u8(0xc457aeu)); observe(h,FC_SPEED_OR_BYTE,rd_u8(0xc457aeu),0); if((uint8_t)w.speed) goto keep_record;
    W(speed,FC_SPEED_WORD,rd_u16(w.record)); w.speed=narrow_word(w.speed,w.speed&0x400); observe(h,FC_SPEED_AND_WORD,0x400,0);
    if((uint16_t)w.speed) { word(h,w.auxiliary+2,rd_u16(w.auxiliary+2)|32); goto clear_record; }
    w.value=rd_u32(w.record+62); w.speed=rd_u32(w.record+66); w.turn=rd_u32(w.record+70); observe(h,FC_MOTION_LOAD,w.record,0);
    if(byte_bit(h,w.record,7)) {
        W(x,FC_X_WORD,rd_u16(w.record+76));
        if((int16_t)w.x>0) { observe(h,FC_COMPARE_WORD,w.x,5); if((int16_t)w.x<5) { value=rd_u16(w.record+76); wr_u16(w.record+76,(uint16_t)(value-1)); observe(h,FC_MEMORY_SUB_WORD,value,1); } else word(h,w.record+76,4); }
        value=rd_u16(w.record+76); observe(h,FC_TEST_WORD,value,0);
        if((int16_t)value<=0) {
            observe(h,FC_COMPARE_BYTE,rd_u8(w.record+98),48);
            if(rd_u8(w.record+98)==48 && !test_byte(h,0xc4582au)) { observe(h,FC_COMPARE_BYTE,rd_u8(w.record+98),48); if(rd_u8(w.record+98)==48) { byte(h,0xc4582au,2); byte(h,0xc4582cu,4); } }
        }
        observe(h,FC_COMPARE_LONG,w.value,0xc0); if((int32_t)w.value<0xc0) { w.value+=12; observe(h,FC_VALUE_ADD_LONG,12,0); }
        L(speed,FC_SPEED_LONG,0); observe(h,FC_TEST_LONG,w.turn,0);
        if((int32_t)w.turn<=0) { ++w.turn; observe(h,FC_TURN_ADD_LONG,1,0); }
        else { --w.turn; observe(h,FC_TURN_SUB_LONG,1,0); } goto publish_autonomous_motion;
    }
    W(x,FC_X_WORD,rd_u16(w.record+76));
    if((int16_t)w.x<0) { add_word(h,w.record+76,1); L(rate,FC_RATE_LONG,6); goto check_motion_reset; }
    add_word(h,w.record+76,1); L(rate,FC_RATE_LONG,2); observe(h,FC_COMPARE_WORD,rd_u16(w.record+76),3); if(rd_s16(w.record+76)<3) goto decelerate_motion;
    L(rate,FC_RATE_LONG,3); observe(h,FC_COMPARE_BYTE,rd_u8(w.record+98),48); if(rd_u8(w.record+98)!=48) goto decelerate_motion;
    W(y,FC_Y_WORD,rd_u16(0xc46184u));
    if(!bit(h,w.y,6)) goto reset_motion;
    if(!bit(h,w.y,2)) goto check_motion_reset;
    observe(h,FC_COMPARE_WORD,rd_u16(w.record+76),5); if(rd_s16(w.record+76)<5) goto reset_motion; goto decelerate_motion;
check_motion_reset:
    W(y,FC_Y_WORD,rd_u16(0xc46184u)); w.y=narrow_word(w.y,w.y&0x600); observe(h,FC_Y_AND_WORD,0x600,0); if((uint16_t)w.y) goto reset_motion;
    observe(h,FC_COMPARE_LONG,rd_u32(w.record+24),0x2000); if(rd_s32(w.record+24)<0x2000) goto reset_motion;
    observe(h,FC_COMPARE_WORD,rd_u16(w.record+76),20); if(rd_s16(w.record+76)<20) goto decelerate_motion;
reset_motion:
    saved_value=w.value; saved_speed=w.speed; saved_turn=w.turn; observe(h,FC_SAVE_MOTION,0,0);
    if(test_byte(h,0xc457b5u)) {
        W(speed,FC_SPEED_WORD,rd_u16(0xc459b4u)); observe(h,FC_COMPARE_WORD,w.speed,rd_u16(0xc458dcu));
        if((int16_t)w.speed!=(int16_t)rd_u16(0xc458dcu)) { w=consume(h,FC_REFRESH_VIEW); byte(h,0xc457d9u,0); w=consume(h,FC_REFRESH_SCENE); }
    }
    w.value=saved_value; w.speed=saved_speed; w.turn=saved_turn; observe(h,FC_RESTORE_MOTION,0,0);
decelerate_motion:
    L(y,FC_Y_LONG,w.value); if((int32_t)w.y<0) { w.y=0u-w.y; observe(h,FC_Y_NEG_LONG,0,0); }
    L(z,FC_Z_LONG,w.turn); if((int32_t)w.z<0) { w.z=0u-w.z; observe(h,FC_Z_NEG_LONG,0,0); }
    observe(h,FC_COMPARE_LONG,w.z,w.y); if((int32_t)w.z<(int32_t)w.y) { old=w.y; w.y=w.z; w.z=old; observe(h,FC_EXCHANGE_Y_Z,0,0); }
    W(y,FC_Y_WORD,rd_u16(w.record+76)); w.y=narrow_word(w.y,(uint16_t)(0u-w.y)); observe(h,FC_Y_NEG_WORD,0,0);
    negative=(int32_t)(int16_t)w.y+240<=0; w.y=narrow_word(w.y,(uint16_t)(w.y+240)); observe(h,FC_Y_ADD_WORD,240,0); if(negative) W(y,FC_Y_WORD,0);
    w.y=sign_word(w.y); observe(h,FC_Y_EXT_LONG,w.y,0); observe(h,FC_COMPARE_LONG,w.z,w.y);
    if((int32_t)w.z>=(int32_t)w.y) {
        L(y,FC_Y_LONG,w.value); L(z,FC_Z_LONG,w.turn); w.y=shift_right(w.y,w.rate); observe(h,FC_Y_ASR_LONG,w.rate,0); w.value-=w.y; observe(h,FC_VALUE_SUB_LONG,w.y,0);
        w.z=shift_right(w.z,w.rate); observe(h,FC_Z_ASR_LONG,w.rate,0); w.turn-=w.z; observe(h,FC_TURN_SUB_LONG,w.z,0);
    }
    observe(h,FC_COMPARE_WORD,rd_u16(w.record+76),3);
    if(rd_s16(w.record+76)>=3) { observe(h,FC_TEST_LONG,w.speed,0); if((int32_t)w.speed>0) { w.speed-=960; observe(h,FC_SPEED_SUB_LONG,960,0); goto publish_autonomous_motion; } }
    observe(h,FC_COMPARE_LONG,w.speed,0xfffffd00);
    if((int32_t)w.speed>-768) { w.speed-=60; observe(h,FC_SPEED_SUB_LONG,60,0); }
    else { w.speed+=0xfffffd00; observe(h,FC_SPEED_ADD_LONG,0xfffffd00,0); w.speed=shift_right(w.speed,1); observe(h,FC_SPEED_ASR_LONG,1,0); }
publish_autonomous_motion:
    wr_u32(w.record+62,w.value); wr_u32(w.record+66,w.speed); wr_u32(w.record+70,w.turn); observe(h,FC_MOTION_STORE,0,0);
    observe(h,FC_TEST_LONG,w.value,0); if((int32_t)w.value<0) { w.value=0u-w.value; observe(h,FC_VALUE_NEG_LONG,0,0); }
    observe(h,FC_TEST_LONG,w.speed,0); if((int32_t)w.speed<0) { w.speed=0u-w.speed; observe(h,FC_SPEED_NEG_LONG,0,0); }
    observe(h,FC_TEST_LONG,w.turn,0); if((int32_t)w.turn<0) { w.turn=0u-w.turn; observe(h,FC_TURN_NEG_LONG,0,0); }
    observe(h,FC_COMPARE_LONG,w.speed,w.value);
    if((int32_t)w.speed>(int32_t)w.value) { observe(h,FC_COMPARE_LONG,w.turn,w.speed); L(value,FC_VALUE_LONG,(int32_t)w.turn>(int32_t)w.speed?w.turn:w.speed); }
    else { observe(h,FC_COMPARE_LONG,w.turn,w.value); if((int32_t)w.turn>(int32_t)w.value) L(value,FC_VALUE_LONG,w.turn); }
    w.value=shift_right(w.value,2); observe(h,FC_VALUE_ASR_LONG,2,0); word(h,w.record+110,(uint16_t)w.value); goto keep_record;
unclassified_record:
    if(!byte_bit(h,w.record+1,6)) return w;
    B(speed,FC_SPEED_BYTE,rd_u8(0xc45788u)); w.speed=narrow_byte(w.speed,w.speed|rd_u8(0xc457aeu)); observe(h,FC_SPEED_OR_BYTE,rd_u8(0xc457aeu),0);
    if(!(uint8_t)w.speed) { value=rd_u16(w.record+76); wr_u16(w.record+76,(uint16_t)(value-1)); observe(h,FC_MEMORY_SUB_WORD,value,1); if((int32_t)(int16_t)value-1<=0) goto clear_record; }
    W(speed,FC_SPEED_WORD,rd_u16(w.record)); w.speed=narrow_word(w.speed,w.speed&0x400); observe(h,FC_SPEED_AND_WORD,0x400,0);
    if(!(uint16_t)w.speed) { W(speed,FC_SPEED_WORD,rd_u16(w.record+76)); observe(h,FC_COMPARE_WORD,w.speed,50); if((int16_t)w.speed>=50) goto accelerate_heading;
        observe(h,FC_COMPARE_WORD,w.speed,12); if((int16_t)w.speed<12) word(h,w.record,rd_u16(w.record)|0x400); }
    W(speed,FC_SPEED_WORD,0x3000); goto approach_heading;
selected_reference_heading:
    W(speed,FC_SPEED_WORD,0x4200);
approach_heading:
    longword(h,w.record+52,0); W(value,FC_VALUE_WORD,rd_u16(w.record+108)); observe(h,FC_COMPARE_WORD,w.value,w.speed);
    if((int16_t)w.value<=(int16_t)w.speed) { AW(value,FC_VALUE_ADD_WORD,0x240); observe(h,FC_COMPARE_WORD,w.value,w.speed); if((int16_t)w.value>(int16_t)w.speed) W(value,FC_VALUE_WORD,w.speed); }
    else { SW(value,FC_VALUE_SUB_WORD,0x240); observe(h,FC_COMPARE_WORD,w.value,w.speed); if((int16_t)w.value<(int16_t)w.speed) W(value,FC_VALUE_WORD,w.speed); }
    word(h,w.record+108,(uint16_t)w.value); word(h,w.record+110,(uint16_t)w.value); goto keep_record;
accelerate_heading:
    W(value,FC_VALUE_WORD,rd_u16(w.record+108)); AW(value,FC_VALUE_ADD_WORD,0x1e0); observe(h,FC_COMPARE_WORD,w.value,0x4200); if((int16_t)w.value>0x4200) W(value,FC_VALUE_WORD,0x4200);
    word(h,w.record+108,(uint16_t)w.value); word(h,w.record+110,(uint16_t)w.value);
    if(!byte_bit(h,w.record+1,1)) goto track_target;
    word(h,w.record,rd_u16(w.record)&0xfffe); if(!byte_bit(h,w.record+1,3)) goto track_target; goto select_reference_target;
clear_reference_flag:
    word(h,w.record,rd_u16(w.record)&0xfffe); if(!byte_bit(h,w.record+1,3)) goto keep_record;
select_reference_target:
    W(value,FC_VALUE_WORD,rd_u16(0xc459c0u)); observe(h,FC_COMPARE_WORD,w.value,0xffff); if((uint16_t)w.value==0xffff) goto selected_reference_heading;
    observe(h,FC_COMPARE_WORD,w.value,rd_u16(0xc459b6u)); if((uint16_t)w.value==rd_u16(0xc459b6u)) goto selected_reference_heading;
    w.auxiliary=0xc46184u; observe(h,FC_AUXILIARY,w.auxiliary,0); w.auxiliary+=(uint32_t)(int32_t)(int16_t)w.value; observe(h,FC_AUXILIARY,w.auxiliary,0);
    word(h,w.record+44,rd_u16(w.auxiliary+6)); word(h,w.record+46,rd_u16(w.auxiliary+8));
    w.turn=sign_word(rd_u16(w.auxiliary+12)); w.x=sign_word(rd_u16(w.auxiliary+14)); observe(h,FC_REFERENCE_PAIR_LOAD,w.auxiliary+12,0); L(y,FC_Y_LONG,rd_u32(w.auxiliary+16));
    wr_u16(w.record+48,(uint16_t)w.turn); wr_u16(w.record+50,(uint16_t)w.x); observe(h,FC_REFERENCE_PAIR_STORE,0,0); longword(h,w.record+52,w.y);
    w.value=narrow_word(w.value,(uint16_t)w.value>>8); observe(h,FC_VALUE_LSR_WORD,8,0); w.value=narrow_word(w.value,(uint16_t)w.value>>1); observe(h,FC_VALUE_LSR_WORD,1,0);
    w.value=narrow_byte(w.value,w.value|0x80); observe(h,FC_VALUE_OR_BYTE,0x80,0); byte(h,w.record+56,(uint8_t)w.value); goto keep_record;
check_reference:
    observe(h,FC_COMPARE_BYTE,rd_u8(w.record+122),5); if(rd_u8(w.record+122)==5) byte(h,w.record+122,3);
    if(!byte_bit(h,w.viewer+1,6)) goto resolve_zone;
    word(h,w.record,rd_u16(w.record)|1); L(value,FC_VALUE_LONG,0xc46184u); observe(h,FC_COMPARE_LONG,w.value,w.viewer); if(w.value==w.viewer) goto prepare_view_projection;
select_nearest_reference:
    observe(h,FC_COMPARE_BYTE,rd_u8(w.record+5),8); if(rd_u8(w.record+5)==8) goto prepare_view_projection;
    w.value=rd_u32(w.record+20); w.speed=rd_u32(w.record+24); w.turn=rd_u32(w.record+28); observe(h,FC_POSITION_LOAD,w.record,0);
    w.x=rd_u32(0xc46198u); w.y=rd_u32(0xc4619cu); w.z=rd_u32(0xc461a0u); observe(h,FC_REFERENCE_LOAD,0,0);
    observe(h,FC_COMPARE_BYTE,rd_u8(0xc458a7u),3);
    if(rd_s8(0xc458a7u)>=3) L(rate,FC_RATE_LONG,0x300000);
    else { observe(h,FC_COMPARE_BYTE,rd_u8(0xc458a7u),2); L(rate,FC_RATE_LONG,rd_s8(0xc458a7u)>=2?0x240000:0x180000); }
    negative=(int64_t)(int32_t)w.x-(int32_t)w.value<0; w.x-=w.value; observe(h,FC_X_SUB_LONG,w.value,0); if(negative) { w.x=0u-w.x; observe(h,FC_X_NEG_LONG,0,0); }
    observe(h,FC_COMPARE_LONG,w.x,w.rate); if((int32_t)w.x>(int32_t)w.rate) goto prepare_view_projection;
    negative=(int64_t)(int32_t)w.y-(int32_t)w.speed<0; w.y-=w.speed; observe(h,FC_Y_SUB_LONG,w.speed,0); if(negative) { w.y=0u-w.y; observe(h,FC_Y_NEG_LONG,0,0); }
    observe(h,FC_COMPARE_LONG,w.y,w.rate); if((int32_t)w.y>(int32_t)w.rate) goto prepare_view_projection;
    negative=(int64_t)(int32_t)w.z-(int32_t)w.turn<0; w.z-=w.turn; observe(h,FC_Z_SUB_LONG,w.turn,0); if(negative) { w.z=0u-w.z; observe(h,FC_Z_NEG_LONG,0,0); }
    observe(h,FC_COMPARE_LONG,w.z,w.rate); if((int32_t)w.z>(int32_t)w.rate) goto prepare_view_projection; byte(h,w.record+56,0x80);
prepare_view_projection:
    W(value,FC_VALUE_WORD,rd_u16(w.record+2)); w.value=narrow_word(w.value,w.value&1); observe(h,FC_VALUE_AND_WORD,1,0);
    if((uint16_t)w.value) {
        if(byte_bit(h,w.record+100,8)) { W(y,FC_Y_WORD,byte_bit(h,w.record+100,1)?0x60:0xffa0); L(x,FC_X_LONG,0); W(z,FC_Z_WORD,0xffd0); }
        else { W(x,FC_X_WORD,byte_bit(h,w.record+100,1)?0xa8:0xffa0); L(y,FC_Y_LONG,0); W(z,FC_Z_WORD,0xffd0); }
    } else {
        W(x,FC_X_WORD,rd_u16(0xc458dau)); w.x=narrow_word(w.x,w.x&255); observe(h,FC_X_AND_WORD,255,0);
        if(!(uint16_t)w.x) {
            B(x,FC_X_BYTE,20); B(y,FC_Y_BYTE,rd_u8(w.viewer+40)); if((int8_t)w.y<0) { w.y=narrow_byte(w.y,(uint8_t)(0u-w.y)); observe(h,FC_Y_NEG_BYTE,0,0); }
            observe(h,FC_COMPARE_WORD,w.y,w.x);
            if((int16_t)w.y<=(int16_t)w.x) { B(y,FC_Y_BYTE,rd_u8(w.viewer+42)); if((int8_t)w.y<0) { w.y=narrow_byte(w.y,(uint8_t)(0u-w.y)); observe(h,FC_Y_NEG_BYTE,0,0); }
                observe(h,FC_COMPARE_BYTE,w.y,w.x); if((int8_t)w.y<=(int8_t)w.x) goto default_projection; }
            W(y,FC_Y_WORD,rd_u16(w.viewer+22)); w.y=narrow_word(w.y,w.y&4); observe(h,FC_Y_AND_WORD,4,0); byte(h,0xc4578au,(uint8_t)w.y);
        }
default_projection:
        test_byte(h,0xc4578au); L(x,FC_X_LONG,0); L(y,FC_Y_LONG,0); W(z,FC_Z_WORD,0xffdc);
    }
    w=consume(h,FC_PROJECT_VIEW); L(x,FC_X_LONG,w.value); L(y,FC_Y_LONG,w.turn);
    w.x=shift_right(w.x,8); observe(h,FC_X_ASR_LONG,8,0); w.y=shift_right(w.y,8); observe(h,FC_Y_ASR_LONG,8,0);
    w.x=narrow_word(w.x,w.x&0x3fff); observe(h,FC_X_AND_WORD,0x3fff,0); w.y=narrow_word(w.y,w.y&0x3fff); observe(h,FC_Y_AND_WORD,0x3fff,0);
    w.value=swap_words(w.value); observe(h,FC_VALUE_SWAP,0,0); w.turn=swap_words(w.turn); observe(h,FC_TURN_SWAP,0,0);
    w.value=narrow_word(w.value,(uint16_t)((int16_t)w.value>>6)); observe(h,FC_VALUE_ASR_WORD,6,0); w.turn=narrow_word(w.turn,(uint16_t)((int16_t)w.turn>>6)); observe(h,FC_TURN_ASR_WORD,6,0);
    wr_u16(w.record+44,(uint16_t)w.value); wr_u16(w.record+46,(uint16_t)w.turn); wr_u16(w.record+48,(uint16_t)w.x); wr_u16(w.record+50,(uint16_t)w.y); observe(h,FC_PROJECTED_VIEW_STORE,0,0);
    w.speed=shift_right(w.speed,8); observe(h,FC_SPEED_ASR_LONG,8,0); longword(h,w.record+52,w.speed);
finish_view_status:
    if(byte_bit(h,w.record+32,1)) goto finish_pending;
    w=consume(h,FC_SIGHT_RECORD); B(value,FC_VALUE_BYTE,rd_u8(w.record+5));
    if(!(uint8_t)w.value) goto select_status_row;
    observe(h,FC_COMPARE_BYTE,w.value,1); if((uint8_t)w.value==1) goto select_status_row;
    observe(h,FC_COMPARE_BYTE,w.value,6); if((uint8_t)w.value==6) goto finish_pending;
    observe(h,FC_COMPARE_BYTE,w.value,8);
    if((uint8_t)w.value==8) { observe(h,FC_COMPARE_BYTE,rd_u8(0xc458a6u),5); if(rd_u8(0xc458a6u)!=5 || !test_byte(h,0xc457b7u)) goto finish_pending; }
select_status_row:
    if(!byte_bit(h,w.record+4,5)) goto finish_pending;
    w.route=0xc2bb98u; observe(h,FC_ROUTE,w.route,0); W(value,FC_VALUE_WORD,rd_u16(0xc458c6u)); L(turn,FC_TURN_LONG,0);
    observe(h,FC_COMPARE_WORD,rd_u16(w.viewer+74),0x300);
    if(rd_s16(w.viewer+74)>0x300) { w.route+=96; observe(h,FC_ROUTE,w.route,0); observe(h,FC_COMPARE_WORD,rd_u16(w.viewer+74),0xc00);
        if(rd_s16(w.viewer+74)>0xc00) { w.route+=96; observe(h,FC_ROUTE,w.route,0); observe(h,FC_COMPARE_WORD,rd_u16(w.viewer+74),0x1e00); if(rd_s16(w.viewer+74)>0x1e00) { w.route+=96; observe(h,FC_ROUTE,w.route,0); goto publish_status_row; } }
        code=1;
    } else code=0;
    W(speed,FC_SPEED_WORD,rd_u16(w.record+108)); negative=(int32_t)(int16_t)w.speed-rd_s16(w.viewer+108)<0; SW(speed,FC_SPEED_SUB_WORD,rd_u16(w.viewer+108));
    if(negative) { w.speed=narrow_word(w.speed,(uint16_t)(0u-w.speed)); observe(h,FC_SPEED_NEG_WORD,0,0); L(turn,FC_TURN_LONG,1); }
    observe(h,FC_COMPARE_WORD,w.speed,0xc0);
    if((int16_t)w.speed>=0xc0) { w.route+=32; observe(h,FC_ROUTE,w.route,0); observe(h,FC_TEST_BYTE,w.turn,0); if(!(uint8_t)w.turn) { w.route+=32; observe(h,FC_ROUTE,w.route,0); } }
    if(!code) { w.value=narrow_word(w.value,w.value&8); observe(h,FC_VALUE_AND_WORD,8,0); goto publish_status_row; }
    W(value,FC_VALUE_WORD,rd_u16(0xc459c0u)); if((int16_t)w.value<0) goto publish_status_row;
    observe(h,FC_COMPARE_WORD,w.value,rd_u16(0xc459b6u)); if((uint16_t)w.value!=rd_u16(0xc459b6u) || !test_byte(h,0xc457b7u)) goto publish_status_row;
    byte(h,0xc457b7u,0); w.route+=16; observe(h,FC_ROUTE,w.route,0);
publish_status_row:
    B(speed,FC_SPEED_BYTE,rd_u8(0xc458a7u)); w.speed=narrow_word(w.speed,(uint16_t)(int16_t)(int8_t)w.speed); observe(h,FC_SPEED_EXT_WORD,0,0); observe(h,FC_COMPARE_WORD,w.speed,3);
    if((int16_t)w.speed>3) L(speed,FC_SPEED_LONG,3); AW(speed,FC_SPEED_ADD_WORD,w.speed); AW(speed,FC_SPEED_ADD_WORD,w.speed);
    p=w.route+(uint32_t)(int32_t)(int16_t)w.speed; word(h,w.record+76,rd_u16(p)); W(value,FC_VALUE_WORD,rd_u16(p+2));
    observe(h,FC_COMPARE_BYTE,rd_u8(w.record+5),8);
    if(rd_u8(w.record+5)==8 && byte_bit(h,w.record+1,3)) { memory_bit(h,w.record+1,3,FC_MEMORY_BIT_CLEAR); code=rd_u8(0xc458a9u); wr_u8(0xc458a9u,(uint8_t)(code+1)); observe(h,FC_MEMORY_ADD_BYTE,code,1); code=rd_u8(0xc458aau); wr_u8(0xc458aau,(uint8_t)(code+1)); observe(h,FC_MEMORY_ADD_BYTE,code,1); }
    byte(h,w.record+5,(uint8_t)w.value); goto finish_pending;
resolve_zone:
    if(!byte_bit(h,w.record,4)) goto keep_record;
    w.viewer=rd_u32(0xc4573au); observe(h,FC_VIEWER,w.viewer,0);
    for(;;) {
        for(;;) {
            value=rd_u16(w.viewer); observe(h,FC_TEST_WORD,value,0); if((int16_t)value<0) break;
            W(value,FC_VALUE_WORD,rd_u16(w.viewer+4)); w.value=narrow_word(w.value,w.value&255); observe(h,FC_VALUE_AND_WORD,255,0); observe(h,FC_COMPARE_WORD,w.value,rd_u16(0xc459b4u)); if((uint16_t)w.value==rd_u16(0xc459b4u)) goto publish_zone;
            w.viewer+=10; observe(h,FC_VIEWER,w.viewer,0);
        }
        B(value,FC_VALUE_BYTE,rd_u8(w.record+93)); if((int8_t)w.value<=0) { word(h,0xc4599eu,0x34); w=consume(h,FC_ZONE_FAULT); goto finish_pending; }
        w.viewer=0xc29720u; observe(h,FC_VIEWER,w.viewer,0); w.value=narrow_byte(w.value,(uint8_t)(w.value-1)); observe(h,FC_VALUE_SUB_BYTE,1,0);
        w.value=narrow_word(w.value,(uint16_t)(int16_t)(int8_t)w.value); observe(h,FC_VALUE_EXT_WORD,0,0); w.value=narrow_word(w.value,(uint16_t)(w.value<<2)); observe(h,FC_VALUE_ASL_WORD,2,0);
        w.viewer=rd_u32(w.viewer+(uint32_t)(int32_t)(int16_t)w.value); observe(h,FC_VIEWER,w.viewer,0); w.viewer+=10; observe(h,FC_VIEWER,w.viewer,0);
    }
publish_zone:
    W(turn,FC_TURN_WORD,rd_u16(w.viewer+6)); w.route=0xc295e0u; observe(h,FC_ROUTE,w.route,0); w.route+=(uint32_t)(int32_t)rd_s16(w.route+(uint32_t)(int32_t)(int16_t)w.turn); observe(h,FC_ROUTE,w.route,0);
    w.turn=sign_word(rd_u16(w.route)); w.x=sign_word(rd_u16(w.route+2)); w.y=sign_word(rd_u16(w.route+4)); w.z=sign_word(rd_u16(w.route+6)); w.rate=sign_word(rd_u16(w.route+8)); observe(h,FC_ZONE_VIEW_LOAD,0,0); w.route+=10;
    word(h,w.record+44,(uint16_t)w.turn); word(h,w.record+46,(uint16_t)w.x); word(h,w.record+48,(uint16_t)w.y); word(h,w.record+50,(uint16_t)w.z); longword(h,w.record+52,w.rate); byte(h,w.record+56,255); memory_bit(h,w.record+1,0,FC_MEMORY_BIT_CLEAR); goto finish_pending;
track_target:
    word(h,w.record,rd_u16(w.record)|1); B(value,FC_VALUE_BYTE,rd_u8(w.record+56)); w.value=narrow_word(w.value,w.value&0x7f); observe(h,FC_VALUE_AND_WORD,0x7f,0);
    w.value=narrow_word(w.value,(uint16_t)(w.value<<8)); observe(h,FC_VALUE_ASL_WORD,8,0); AW(value,FC_VALUE_ADD_WORD,w.value);
    if(!(uint16_t)w.value) {
        observe(h,FC_COMPARE_BYTE,rd_u8(w.record+98),0); B(speed,FC_SPEED_BYTE,rd_u8(rd_u8(w.record+98)==0?0xc4588cu:0xc4588du)); if((uint8_t)w.speed) goto selected_control_target;
    }
    w.viewer=0xc46184u; observe(h,FC_VIEWER,w.viewer,0); w.viewer+=(uint32_t)(int32_t)(int16_t)w.value; observe(h,FC_VIEWER,w.viewer,0);
    if(!byte_bit(h,w.record+1,6)) memory_bit(h,w.record+1,7,FC_MEMORY_BIT_CLEAR);
    word(h,w.record+44,rd_u16(w.viewer+6)); word(h,w.record+46,rd_u16(w.viewer+8)); word(h,w.record+48,rd_u16(w.viewer+12)); word(h,w.record+50,rd_u16(w.viewer+14)); longword(h,w.record+52,rd_u32(w.viewer+16)); goto keep_record;
selected_control_target:
    w.viewer=0xc45c72u; observe(h,FC_VIEWER,w.viewer,0); w.speed=narrow_byte(w.speed,(uint8_t)(w.speed-1)); observe(h,FC_SPEED_SUB_BYTE,1,0);
    w.speed=narrow_word(w.speed,(uint16_t)(int16_t)(int8_t)w.speed); observe(h,FC_SPEED_EXT_WORD,0,0); w.speed=narrow_word(w.speed,(uint16_t)(w.speed<<6)); observe(h,FC_SPEED_ASL_WORD,6,0); w.viewer+=(uint32_t)(int32_t)(int16_t)w.speed; observe(h,FC_VIEWER,w.viewer,0);
    L(x,FC_X_LONG,0); L(z,FC_Z_LONG,0); W(x,FC_X_WORD,rd_u16(w.viewer+48)); W(z,FC_Z_WORD,rd_u16(w.viewer+50)); word(h,w.record+44,(uint16_t)w.x); word(h,w.record+46,(uint16_t)w.z);
    w.x=narrow_word(w.x,(uint16_t)(w.x<<6)); observe(h,FC_X_ASL_WORD,6,0); w.z=narrow_word(w.z,(uint16_t)(w.z<<6)); observe(h,FC_Z_ASL_WORD,6,0);
    w.x=swap_words(w.x); observe(h,FC_X_SWAP,0,0); w.z=swap_words(w.z); observe(h,FC_Z_SWAP,0,0);
    w.value=rd_u32(w.viewer); w.speed=rd_u32(w.viewer+4); w.turn=rd_u32(w.viewer+8); observe(h,FC_SELECTED_CONTROL_LOAD,w.viewer,0);
    w.x+=w.value; observe(h,FC_X_ADD_LONG,w.value,0); w.z+=w.turn; observe(h,FC_Z_ADD_LONG,w.turn,0); L(y,FC_Y_LONG,w.speed);
    w.value=shift_right(w.value,8); observe(h,FC_VALUE_ASR_LONG,8,0); w.speed=shift_right(w.speed,8); observe(h,FC_SPEED_ASR_LONG,8,0); w.turn=shift_right(w.turn,8); observe(h,FC_TURN_ASR_LONG,8,0);
    word(h,w.record+48,(uint16_t)w.value); word(h,w.record+50,(uint16_t)w.turn); longword(h,w.record+52,w.speed); W(value,FC_VALUE_WORD,0x4800);
    negative=(int64_t)(int32_t)w.x-rd_s32(w.record+20)<0; operand=rd_u32(w.record+20); w.x-=operand; observe(h,FC_X_SUB_LONG,operand,0); if(negative) { w.x=0u-w.x; observe(h,FC_X_NEG_LONG,0,0); }
    observe(h,FC_COMPARE_LONG,w.x,w.value); if((int32_t)w.x>(int32_t)w.value) goto distant_control_target;
    negative=(int64_t)(int32_t)w.y-rd_s32(w.record+24)<0; operand=rd_u32(w.record+24); w.y-=operand; observe(h,FC_Y_SUB_LONG,operand,0); if(negative) { w.y=0u-w.y; observe(h,FC_Y_NEG_LONG,0,0); }
    observe(h,FC_COMPARE_LONG,w.y,w.value); if((int32_t)w.y>(int32_t)w.value) goto distant_control_target;
    negative=(int64_t)(int32_t)w.z-rd_s32(w.record+28)<0; operand=rd_u32(w.record+28); w.z-=operand; observe(h,FC_Z_SUB_LONG,operand,0); if(negative) { w.z=0u-w.z; observe(h,FC_Z_NEG_LONG,0,0); }
    observe(h,FC_COMPARE_LONG,w.z,w.value); if((int32_t)w.z>(int32_t)w.value) goto distant_control_target;
    memory_bit(h,w.record+3,0,FC_MEMORY_BIT_SET); goto keep_record;
distant_control_target:
    if(byte_bit(h,w.record+3,0)) memory_bit(h,w.record+32,5,FC_MEMORY_BIT_SET); goto keep_record;
clear_record:
    word(h,w.record,0); byte(h,0xc45858u,12); L(value,FC_VALUE_LONG,0); goto done;
finish_pending:
    byte(h,0xc457b7u,0);
keep_record:
    L(value,FC_VALUE_LONG,1);
done:
    return w;
#undef B
#undef W
#undef L
#undef AW
#undef SW
}
