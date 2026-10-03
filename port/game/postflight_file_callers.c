/* Game-side file decisions; actual platform children are supplied by hooks.
 * The formatter retains the original rewritten caller arguments and locals. */
#include "postflight_file_callers.h"
#include "globals.h"
#include <stdlib.h>
#define FILE_CHECK_BUFFER 0xc4fdc4u
#define MODE_FILE_READ_RESULT 0xc1ab78u
static void observe(const PostflightFileHooks *h,enum PostflightFilePhase p,uint32_t v,uint32_t other) {
    if(h && h->observe) h->observe(h->context,p,v,other);
}
static uint32_t consume(const PostflightFileHooks *h,enum PostflightFileChild child) {
    if(h && h->consume) return (uint32_t)h->consume(h->context,child);
    abort();
}
static void byte(const PostflightFileHooks *h,gaddr a,uint8_t v) { wr_u8(a,v); observe(h,PFF_STORE_BYTE,v,0); }
static void word(const PostflightFileHooks *h,gaddr a,uint16_t v) { wr_u16(a,v); observe(h,PFF_STORE_WORD,v,0); }
static void longword(const PostflightFileHooks *h,gaddr a,uint32_t v) { wr_u32(a,v); observe(h,PFF_STORE_LONG,v,0); }
static uint8_t read_byte(const PostflightFileHooks *h,gaddr a) { uint8_t v=rd_u8(a); observe(h,PFF_D0_BYTE,v,0); return v; }
static uint32_t read_long(const PostflightFileHooks *h,gaddr a) { uint32_t v=rd_u32(a); observe(h,PFF_D0_LONG,v,0); return v; }
static gaddr address(const PostflightFileHooks *h,gaddr a) { observe(h,PFF_A0,a,0); return a; }
static void compare_long(const PostflightFileHooks *h,uint32_t v,uint32_t limit) { observe(h,PFF_CMP_LONG,v,limit); }
static void compare_byte(const PostflightFileHooks *h,uint8_t v,uint8_t limit) { observe(h,PFF_CMP_BYTE,v,limit); }
static void add_local(const PostflightFileHooks *h,gaddr a,uint32_t amount,unsigned width,int subtract) {
    uint32_t old=width==1?rd_u8(a):rd_u32(a),v=subtract?old-amount:old+amount;
    observe(h,width==1?(subtract?PFF_SUB_MEMORY_BYTE:PFF_ADD_MEMORY_BYTE):(subtract?PFF_SUB_MEMORY_LONG:PFF_ADD_MEMORY_LONG),old,amount);
    if(width==1) wr_u8(a,(uint8_t)v); else wr_u32(a,v);
}
void format_postflight_hex_frame(gaddr frame,const PostflightFileHooks *h) {
    uint8_t width,count,digit; uint32_t value; gaddr cursor;
    width=read_byte(h,frame+19); observe(h,PFF_EXT_WORD,(uint16_t)(int16_t)(int8_t)width,0);
    value=(uint32_t)(int32_t)(int8_t)width; observe(h,PFF_EXT_LONG,value,0);
    add_local(h,frame+8,value,4,0); byte(h,frame-1,0);
    for(;;) {
        count=read_byte(h,frame-1); width=rd_u8(frame+19); compare_byte(h,count,width);
        if((int8_t)count>=(int8_t)width) break;
        value=read_long(h,frame+12); observe(h,PFF_AND_LONG,15,0); value&=15;
        observe(h,PFF_ADD_LONG,0x30,0); digit=(uint8_t)(value+0x30); byte(h,frame-2,digit);
        compare_byte(h,digit,0x39); if((int8_t)digit>0x39) add_local(h,frame-2,7,1,0);
        cursor=address(h,rd_u32(frame+8)); byte(h,cursor,rd_u8(frame-2));
        add_local(h,frame+8,1,4,1); add_local(h,frame-1,1,1,0);
        value=read_long(h,frame+12); observe(h,PFF_LSR_LONG,4,0); longword(h,frame+12,value>>4);
    }
    add_local(h,frame+19,1,1,1); add_local(h,frame+8,1,4,0); byte(h,frame-1,0);
    for(;;) {
        count=read_byte(h,frame-1); width=rd_u8(frame+19); compare_byte(h,count,width);
        if((int8_t)count>=(int8_t)width) break;
        cursor=address(h,rd_u32(frame+8)); digit=read_byte(h,cursor); compare_byte(h,digit,0x30);
        if(digit!=0x30) break;
        byte(h,cursor,0x20); add_local(h,frame+8,1,4,0); add_local(h,frame-1,1,1,0);
    }
}
uint32_t check_postflight_mode_file(gaddr frame,const PostflightFileHooks *h) {
    uint32_t buffer,tag,result;
    buffer=consume(h,PFF_ALLOCATE_CHECK); longword(h,FILE_CHECK_BUFFER,buffer);
    observe(h,PFF_ADD_LONG,3,0); buffer+=3; observe(h,PFF_AND_LONG,0xfffffffcu,0); buffer&=0xfffffffcu;
    longword(h,FILE_CHECK_BUFFER,buffer); consume(h,PFF_LOCK_CHECK); consume(h,PFF_EXAMINE_CHECK); consume(h,PFF_UNLOCK_CHECK);
    buffer=address(h,rd_u32(FILE_CHECK_BUFFER)); tag=read_long(h,buffer+24);
    longword(h,frame-4,tag); observe(h,PFF_TEST_LONG,tag,0);
    if((int32_t)tag<0) { observe(h,PFF_D0_LONG,1,0); longword(h,frame-8,1); }
    else {
        tag=rd_u32(frame-4); compare_long(h,tag,0x42414400u);
        if(tag==0x42414400u) { observe(h,PFF_D0_LONG,2,0); longword(h,frame-8,2); }
        else {
            buffer=address(h,rd_u32(FILE_CHECK_BUFFER)); result=rd_u32(buffer+8); compare_long(h,result,0x50);
            if(result==0x50) { observe(h,PFF_D0_LONG,3,0); longword(h,frame-8,3); }
            else longword(h,frame-8,0);
        }
    }
    buffer=rd_u32(FILE_CHECK_BUFFER); observe(h,PFF_TEST_LONG,buffer,0); if(buffer) consume(h,PFF_FREE_CHECK);
    return read_long(h,frame-8);
}
void refresh_postflight_mode_file(const PostflightFileHooks *h) {
    uint32_t status,result;
    consume(h,PFF_RELEASE_TABLE); status=consume(h,PFF_CHECK_TABLE);
    word(h,MENU_TABLE_STATUS,(uint16_t)status); observe(h,PFF_TEST_WORD,status,0);
    if(!(uint16_t)status) {
        result=consume(h,PFF_LOAD_TABLE); word(h,MODE_FILE_READ_RESULT,(uint16_t)result); observe(h,PFF_SUB_WORD,1,0);
        if((uint16_t)result==1) word(h,MENU_FILE_READY,1);
        else consume(h,PFF_SAVE_TABLE);
    }
    consume(h,PFF_OWN_TABLE);
}
uint32_t save_postflight_mode_file(gaddr frame,const PostflightFileHooks *h) {
    uint32_t handle,result;
    handle=consume(h,PFF_OPEN_SAVE); longword(h,frame-12,handle); observe(h,PFF_TEST_LONG,handle,0);
    if((int32_t)handle<=0) { observe(h,PFF_D0_LONG,0,0); return 0; }
    result=consume(h,PFF_WRITE_SAVE); longword(h,frame-8,result); observe(h,PFF_ADD_LONG,1,0);
    if(result==0xffffffffu) { observe(h,PFF_D0_LONG,0,0); return 0; }
    word(h,MENU_FILE_READY,1); return consume(h,PFF_CLOSE_SAVE);
}
uint32_t read_postflight_mode_file(gaddr frame,const PostflightFileHooks *h) {
    uint32_t handle,result;
    handle=consume(h,PFF_OPEN_LOAD); longword(h,frame-12,handle); observe(h,PFF_TEST_LONG,handle,0);
    if((int32_t)handle<=0) { word(h,MENU_FILE_READY,0); observe(h,PFF_D0_LONG,0,0); return 0; }
    word(h,MENU_FILE_READY,1); result=consume(h,PFF_READ_LOAD);
    observe(h,PFF_PUSH_LOAD_HANDLE,0,0); longword(h,frame-8,result); consume(h,PFF_CLOSE_LOAD);
    result=rd_u32(frame-8); compare_long(h,result,0xffffffffu);
    if(result==0xffffffffu) { observe(h,PFF_D0_LONG,0,0); return 0; }
    word(h,MENU_FILE_READY,1); observe(h,PFF_D0_LONG,1,0); return 1;
}
