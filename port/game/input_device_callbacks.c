/* Complete game-side mouse-counter, viewport and input-device setup owners. */
#include "input_device_callbacks.h"
#include "globals.h"
#include <stdlib.h>
#define COUNTER_X 0xc1ac06u
#define COUNTER_Y 0xc1ac08u
#define INPUT_X 0xc45776u
#define INPUT_Y 0xc45778u
#define INPUT_TICKS 0xc45774u
#define MIN_X 0xc081acu
#define MIN_Y 0xc081aeu
#define MAX_X 0xc081b0u
#define MAX_Y 0xc081b2u
#define VIEWPORT_COUNTDOWN 0xc458a3u
#define SAVED_VIEW 0xc1821cu
#define SAVED_PALETTE 0xc18232u
static void observe(const InputDeviceHooks *h,enum InputDevicePhase p,uint32_t v,uint32_t other) {
    if(h && h->observe) h->observe(h->context,p,v,other);
}
static uint32_t consume(const InputDeviceHooks *h,enum InputDeviceChild child) {
    if(h && h->consume) return (uint32_t)h->consume(h->context,child);
    abort();
}
static void byte(const InputDeviceHooks *h,gaddr a,uint8_t v) { wr_u8(a,v); observe(h,IDC_STORE_BYTE,v,0); }
static void word(const InputDeviceHooks *h,gaddr a,uint16_t v) { wr_u16(a,v); observe(h,IDC_STORE_WORD,v,0); }
static void longword(const InputDeviceHooks *h,gaddr a,uint32_t v) { wr_u32(a,v); observe(h,IDC_STORE_LONG,v,0); }
static uint8_t read_byte(const InputDeviceHooks *h,gaddr a,unsigned reg) {
    uint8_t v=rd_u8(a); observe(h,reg==2?IDC_D2_BYTE:reg?IDC_D1_BYTE:IDC_D0_BYTE,v,0); return v;
}
static uint16_t read_word(const InputDeviceHooks *h,gaddr a,unsigned reg) {
    uint16_t v=rd_u16(a); observe(h,reg?IDC_D1_WORD:IDC_D0_WORD,v,0); return v;
}
static gaddr address(const InputDeviceHooks *h,unsigned reg,gaddr a) { observe(h,reg?IDC_A1:IDC_A0,a,0); return a; }
static void compare_word(const InputDeviceHooks *h,uint16_t dst,uint16_t src) { observe(h,IDC_COMPARE_WORD,dst,src); }
static void compare_byte(const InputDeviceHooks *h,uint8_t dst,uint8_t src) { observe(h,IDC_COMPARE_BYTE,dst,src); }
static void add_local(const InputDeviceHooks *h,gaddr a,uint32_t amount,unsigned width,int subtract) {
    uint32_t v=width==2?rd_u16(a):rd_u32(a);
    observe(h,width==2?(subtract?IDC_SUB_MEMORY_WORD:IDC_ADD_MEMORY_WORD):IDC_ADD_MEMORY_LONG,v,amount);
    v=subtract?v-amount:v+amount; if(width==2) wr_u16(a,(uint16_t)v); else wr_u32(a,v);
}
static void read_counters(gaddr frame,const InputDeviceHooks *h,gaddr raw_local,int deltas,uint16_t raw) {
    uint16_t x,y,old;
    word(h,raw_local,raw); observe(h,IDC_AND_D0_WORD,0xff,0); x=raw&255;
    y=read_word(h,raw_local,1); observe(h,IDC_ASR_D1_WORD,8,0); y=(uint16_t)((int16_t)y>>8);
    observe(h,IDC_AND_D1_WORD,0xff,0); y&=255;
    if(!deltas) { word(h,COUNTER_X,x); word(h,COUNTER_Y,y); return; }
    word(h,frame-6,x); old=rd_u16(COUNTER_X); observe(h,IDC_SUB_D0_WORD,old,0); x=(uint16_t)(x-old);
    word(h,frame-8,y); old=rd_u16(COUNTER_Y); observe(h,IDC_SUB_D1_WORD,old,0); y=(uint16_t)(y-old);
    word(h,frame-10,x); word(h,frame-12,y);
    compare_word(h,x,0xff80);
    if((int16_t)x<-128) add_local(h,frame-10,256,2,0);
    else {
        x=read_word(h,frame-10,0); compare_word(h,x,127); if((int16_t)x>127) add_local(h,frame-10,256,2,1);
    }
    y=read_word(h,frame-12,0); compare_word(h,y,0xff80);
    if((int16_t)y<-128) add_local(h,frame-12,256,2,0);
    else {
        y=read_word(h,frame-12,0); compare_word(h,y,127); if((int16_t)y>127) add_local(h,frame-12,256,2,1);
    }
}
static void clamp_axis(const InputDeviceHooks *h,gaddr a,gaddr minimum,gaddr maximum) {
    uint16_t current=read_word(h,a,0),upper=read_word(h,maximum,1),lower,selected;
    compare_word(h,upper,current); selected=upper;
    if((int16_t)upper>=(int16_t)current) selected=read_word(h,a,1);
    lower=read_word(h,minimum,0); compare_word(h,lower,selected);
    if((int16_t)lower>(int16_t)selected) { word(h,a,lower); return; }
    current=read_word(h,a,0); upper=read_word(h,maximum,1); compare_word(h,upper,current); selected=upper;
    if((int16_t)upper>=(int16_t)current) selected=read_word(h,a,1);
    observe(h,IDC_D0_FROM_D1,0,0); word(h,a,selected);
}
static void restore_view_pair(gaddr frame,const InputDeviceHooks *h,int first) {
    uint16_t index,page; uint32_t offset; gaddr a;
    if(first) {
        page=read_word(h,DRAW_PAGE,0); observe(h,IDC_D1_LONG,1,0); observe(h,IDC_SUB_D1_WORD,page,0);
        index=(uint16_t)(1-page); word(h,frame-22,index); observe(h,IDC_EXT_D1_LONG,(uint32_t)(int32_t)(int16_t)index,0);
        observe(h,IDC_ASL_D1_LONG,2,0); offset=(uint32_t)(int32_t)(int16_t)index<<2;
    } else {
        index=read_word(h,frame-22,0); observe(h,IDC_EXT_D0_LONG,(uint32_t)(int32_t)(int16_t)index,0);
        observe(h,IDC_ASL_D0_LONG,2,0); offset=(uint32_t)(int32_t)(int16_t)index<<2;
    }
    a=address(h,0,offset); a=address(h,0,a+0xc182ba); longword(h,SAVED_VIEW,rd_u32(a));
    index=read_word(h,frame-22,0); observe(h,IDC_EXT_D0_LONG,(uint32_t)(int32_t)(int16_t)index,0);
    observe(h,IDC_ASL_D0_LONG,2,0); offset=(uint32_t)(int32_t)(int16_t)index<<2;
    a=address(h,0,offset); a=address(h,0,a+0xc182c2); longword(h,SAVED_PALETTE,rd_u32(a));
}
static void advance_sampled_callback(gaddr frame,const InputDeviceHooks *h,uint16_t raw) {
    uint8_t ready; uint16_t v,delta;
    read_counters(frame,h,frame-4,1,raw);
    ready=read_byte(h,PLAYER_READY,0); observe(h,IDC_TEST_BYTE,ready,0);
    if(!ready) {
        v=rd_u16(frame-12); observe(h,IDC_TEST_WORD,v,0);
        if((int16_t)v<0) { v=read_word(h,frame-12,0); observe(h,IDC_ASR_D0_WORD,1,0); word(h,frame-12,(uint16_t)((int16_t)v>>1)); }
    }
    v=read_word(h,INPUT_X,0); delta=rd_u16(frame-10); observe(h,IDC_ADD_D0_WORD,delta,0); word(h,INPUT_X,(uint16_t)(v+delta));
    v=read_word(h,INPUT_Y,1); delta=rd_u16(frame-12); observe(h,IDC_SUB_D1_WORD,delta,0); word(h,INPUT_Y,(uint16_t)(v-delta));
    clamp_axis(h,INPUT_X,MIN_X,MAX_X); clamp_axis(h,INPUT_Y,MIN_Y,MAX_Y);
    word(h,COUNTER_X,rd_u16(frame-6)); word(h,COUNTER_Y,rd_u16(frame-8));
    v=read_word(h,INPUT_TICKS,0); observe(h,IDC_ADD_D0_WORD,1,0); word(h,INPUT_TICKS,(uint16_t)(v+1));
    advance_viewport_palette(frame,h);
}
void advance_input_device_callback(gaddr frame,const InputDeviceHooks *h) {
    observe(h,IDC_READ_COUNTERS,1,0);
    uint16_t raw=read_word(h,0xdff00a,0);
    advance_sampled_callback(frame,h,raw);
}
void advance_input_device_callback_sample(gaddr frame,const InputDeviceHooks *h,uint16_t raw) {
    /* The host supplies the two byte counters. All delta/wrap/clamp and PAL
     * bookkeeping remains the same C1718E owner as the hardware caller. */
    observe(h,IDC_READ_COUNTERS,1,0);observe(h,IDC_D0_WORD,raw,0);
    advance_sampled_callback(frame,h,raw);
}
void advance_viewport_palette(gaddr frame,const InputDeviceHooks *h) {
    uint8_t mode,target,count; uint16_t v; int32_t signed_mode; gaddr src,dst; uint32_t offset;
    mode=read_byte(h,VIEWPORT_MODE,0); target=read_byte(h,VIEWPORT_TARGET,1); compare_byte(h,mode,target);
    if(mode!=target) {
        count=read_byte(h,VIEWPORT_COUNTDOWN,2); observe(h,IDC_SUB_D2_BYTE,1,0); count=(uint8_t)(count-1);
        byte(h,VIEWPORT_COUNTDOWN,count); observe(h,IDC_TEST_BYTE,count,0); if((int8_t)count>=0) goto fade;
        compare_byte(h,mode,target);
        if((int8_t)mode>(int8_t)target) { observe(h,IDC_SUB_D0_BYTE,1,0); byte(h,VIEWPORT_MODE,(uint8_t)(mode-1)); byte(h,VIEWPORT_COUNTDOWN,1); }
        else { mode=read_byte(h,VIEWPORT_MODE,0); observe(h,IDC_ADD_D0_BYTE,1,0); byte(h,VIEWPORT_MODE,(uint8_t)(mode+1)); byte(h,VIEWPORT_COUNTDOWN,2); }
        src=address(h,0,0xc08510); signed_mode=(int8_t)read_byte(h,VIEWPORT_MODE,0);
        observe(h,IDC_EXT_WORD,(uint16_t)signed_mode,0); observe(h,IDC_EXT_D0_LONG,(uint32_t)signed_mode,0);
        observe(h,IDC_D1_LONG,15,0); observe(h,IDC_SUB_D1_LONG,(uint32_t)signed_mode,0);
        observe(h,IDC_ASL_D1_LONG,4,0); observe(h,IDC_ASL_D1_LONG,1,0);
        offset=(15u-(uint32_t)signed_mode)<<5; src=address(h,0,src+offset);
        observe(h,IDC_PUSH_FIRST_PALETTE,0,0); longword(h,frame-16,src);
        consume(h,IDC_PALETTE_FIRST); restore_view_pair(frame,h,1);
        consume(h,IDC_PALETTE_SECOND); restore_view_pair(frame,h,0);
        mode=read_byte(h,VIEWPORT_MODE,0); target=read_byte(h,VIEWPORT_TARGET,1); compare_byte(h,mode,target);
        if(mode!=target) goto fade;
        longword(h,frame-20,rd_u32(LONG_TABLE)); word(h,frame-2,0);
        for(;;) {
            v=rd_u16(frame-2); compare_word(h,v,16); if((int16_t)v>=16) break;
            src=address(h,0,rd_u32(frame-16)); dst=address(h,1,rd_u32(frame-20)); word(h,dst,rd_u16(src));
            add_local(h,frame-16,2,4,0); add_local(h,frame-20,2,4,0); add_local(h,frame-2,1,2,0);
        }
        byte(h,TABLE_CLEAR_MODE,3);
    } else {
        count=rd_u8(TABLE_CLEAR_MODE); observe(h,IDC_TEST_BYTE,count,0); if(count) consume(h,IDC_PALETTE_STABLE);
    }
fade:
    consume(h,IDC_FADE); observe(h,IDC_D0_LONG,0,0);
}
void install_input_device_callback(const InputDeviceHooks *h) {
    byte(h,0xc1abf8,2); byte(h,0xc1abf9,0); longword(h,0xc1abfa,0xc081b4);
    address(h,0,0xc1718e); longword(h,0xc1ac02,0xc1718e); consume(h,IDC_ADD_SERVER);
}
void remove_input_device_callback(const InputDeviceHooks *h) { consume(h,IDC_REMOVE_SERVER); }
void prepare_input_device_port(gaddr frame,const InputDeviceHooks *h) {
    gaddr port; uint32_t result;
    observe(h,IDC_D0_LONG,0,0); port=address(h,0,rd_u32(frame+8));
    byte(h,port+14,0); byte(h,port+9,0); longword(h,port+10,0); byte(h,port+8,4);
    result=consume(h,IDC_ALLOCATE_SIGNAL); port=address(h,0,rd_u32(frame+8)); byte(h,port+15,(uint8_t)result);
    result=consume(h,IDC_FIND_TASK); port=address(h,0,rd_u32(frame+8)); longword(h,port+16,result);
    address(h,0,port+20); consume(h,IDC_SET_PORT_LIST);
}
void open_input_device_timer(gaddr frame,const InputDeviceHooks *h) {
    uint32_t result=consume(h,IDC_OPEN_TIMER); word(h,frame-2,(uint16_t)result); observe(h,IDC_TEST_WORD,result,0);
}
void open_input_device_request(const InputDeviceHooks *h) {
    uint32_t result=consume(h,IDC_OPEN_INPUT); gaddr request,port;
    observe(h,IDC_PUSH_INPUT_PORT,0,0); word(h,0xc1abb0,(uint16_t)result); consume(h,IDC_PREPARE_INPUT_PORT);
    request=address(h,0,rd_u32(0xc08134)); longword(h,request+14,rd_u32(0xc0815c));
    observe(h,IDC_D0_LONG,22,0); longword(h,request+36,22);
    port=address(h,1,0xc1abb2); longword(h,request+40,port); word(h,request+28,9);
    observe(h,IDC_PUSH_INPUT_REQUEST,0,0); longword(h,0xc1abac,port); consume(h,IDC_SEND_INPUT_REQUEST);
}
void set_input_device_bounds(gaddr frame,const InputDeviceHooks *h) {
    word(h,MIN_X,rd_u16(frame+10)); word(h,MIN_Y,rd_u16(frame+14));
    word(h,MAX_X,rd_u16(frame+18)); word(h,MAX_Y,rd_u16(frame+22));
}
void initialise_input_device_counters(gaddr frame,const InputDeviceHooks *h) {
    observe(h,IDC_READ_COUNTERS,0,0);
    uint16_t raw=read_word(h,0xdff00a,0);
    read_counters(frame,h,frame-2,0,raw); consume(h,IDC_START_SERVER);
}
void initialise_input_device_counters_sample(gaddr frame,const InputDeviceHooks *h,uint16_t raw) {
    observe(h,IDC_READ_COUNTERS,0,0);observe(h,IDC_D0_WORD,raw,0);
    read_counters(frame,h,frame-2,0,raw); consume(h,IDC_START_SERVER);
}
