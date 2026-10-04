/* Complete original C0D04C/C33370/C1FE24/C1FE46/C0CF98 owners. */
#include "hud_history_stream.h"
#include <stdlib.h>
static void observe(const HudHistoryStreamHooks *h,enum HudHistoryStreamPhase p,enum HudHistoryStreamField f,uint32_t v,uint32_t o) {
    if(h && h->observe) h->observe(h->context,p,f,v,o);
}
static HudHistoryStreamState consume(const HudHistoryStreamHooks *h,enum HudHistoryStreamChild child) {
    if(h && h->consume) return h->consume(h->context,child); abort();
}
static HudHistoryStreamState restored(const HudHistoryStreamHooks *h) {
    if(h && h->restored) return h->restored(h->context); abort();
}
static uint32_t low_word(uint32_t old,uint16_t v) { return (old&0xffff0000u)|v; }
static uint32_t low_byte(uint32_t old,uint8_t v) { return (old&0xffffff00u)|v; }
#define W(f,id,v) do { w.f=low_word(w.f,(uint16_t)(v)); observe(h,HHS_WORD,id,w.f,0); } while(0)
#define B(f,id,v) do { w.f=low_byte(w.f,(uint8_t)(v)); observe(h,HHS_BYTE,id,w.f,0); } while(0)
#define L(f,id,v) do { w.f=(uint32_t)(v); observe(h,HHS_LONG,id,w.f,0); } while(0)
#define P(f,id,v) do { w.f=(gaddr)(v); observe(h,HHS_POINTER,id,w.f,0); } while(0)
#define AW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f+n_)); observe(h,HHS_ADD_WORD,id,n_,0); } while(0)
#define SW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f-n_)); observe(h,HHS_SUB_WORD,id,n_,0); } while(0)
#define AL(f,id,v) do { uint32_t n_=(uint32_t)(v); w.f+=n_; observe(h,HHS_ADD_LONG,id,n_,0); } while(0)
#define SL(f,id,v) do { uint32_t n_=(uint32_t)(v); w.f-=n_; observe(h,HHS_SUB_LONG,id,n_,0); } while(0)
#define ANDW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f&n_)); observe(h,HHS_AND_WORD,id,n_,0); } while(0)
#define ANDB(f,id,v) do { uint8_t n_=(uint8_t)(v); w.f=low_byte(w.f,(uint8_t)(w.f&n_)); observe(h,HHS_AND_BYTE,id,n_,0); } while(0)
#define ORW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f|n_)); observe(h,HHS_OR_WORD,id,n_,0); } while(0)
#define EL(f,id) do { w.f=(uint32_t)(int32_t)(int16_t)w.f; observe(h,HHS_EXT_LONG,id,0,0); } while(0)
#define NEG(f,id) do { w.f=low_word(w.f,(uint16_t)(0u-w.f)); observe(h,HHS_NEG_WORD,id,0,0); } while(0)
#define SWAP(f,id) do { w.f=(w.f<<16)|(w.f>>16); observe(h,HHS_SWAP,id,0,0); } while(0)
#define ASL(f,id,n) do { w.f=low_word(w.f,(uint16_t)((((unsigned)(n)&63u)<16?w.f<<((unsigned)(n)&63u):0))); observe(h,HHS_ASL_WORD,id,n,0); } while(0)
#define LSL(f,id,n) do { w.f=low_word(w.f,(uint16_t)((((unsigned)(n)&63u)<16?w.f<<((unsigned)(n)&63u):0))); observe(h,HHS_LSL_WORD,id,n,0); } while(0)
#define ASR(f,id,n) do { unsigned n_=(unsigned)(n)&63u; w.f=low_word(w.f,(uint16_t)((int16_t)w.f>>(n_<16?n_:15))); observe(h,HHS_ASR_WORD,id,n_,0); } while(0)
#define CW(v,n) observe(h,HHS_COMPARE_WORD,HHS_VALUE,(uint16_t)(v),(uint16_t)(n))
#define CB(v,n) observe(h,HHS_COMPARE_BYTE,HHS_VALUE,(uint8_t)(v),(uint8_t)(n))
#define TW(v) observe(h,HHS_TEST_WORD,HHS_VALUE,(uint16_t)(v),0)
#define TB(v) observe(h,HHS_TEST_BYTE,HHS_VALUE,(uint8_t)(v),0)
static void word(const HudHistoryStreamHooks *h,gaddr a,uint16_t v) { wr_u16(a,v); observe(h,HHS_STORE_WORD,HHS_VALUE,v,0); }
static void byte(const HudHistoryStreamHooks *h,gaddr a,uint8_t v) { wr_u8(a,v); observe(h,HHS_STORE_BYTE,HHS_VALUE,v,0); }
static void longword(const HudHistoryStreamHooks *h,gaddr a,uint32_t v) { wr_u32(a,v); observe(h,HHS_STORE_LONG,HHS_VALUE,v,0); }
static void decrement_byte(const HudHistoryStreamHooks *h,gaddr a) { uint8_t old=rd_u8(a); wr_u8(a,(uint8_t)(old-1)); observe(h,HHS_MEMORY_SUB_BYTE,HHS_VALUE,old,1); }
static int bit(const HudHistoryStreamHooks *h,gaddr a,unsigned b) { uint8_t v=rd_u8(a); observe(h,HHS_BIT_TEST,HHS_VALUE,v,b); return (v>>b)&1; }

/* MOVEM transfers retain CCR; individual MOVE transfers above update it. */
#define RAW(f,id,v) do { w.f=(uint32_t)(v); observe(h,HHS_RAW_LONG,id,w.f,0); } while(0)
#define MUL(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=(uint32_t)((int32_t)(int16_t)w.f*(int16_t)n_); observe(h,HHS_MULS,id,n_,0); } while(0)
#define MUP(f,id,ptr,pi) do { MUL(f,id,rd_u16(w.ptr)); P(ptr,pi,w.ptr+2); } while(0)
#define ARL(f,id,n) do { unsigned n_=(unsigned)(n)&63u; w.f=(uint32_t)((int32_t)w.f>>(n_<32?n_:31)); observe(h,HHS_ASR_LONG,id,n_,0); } while(0)
#define DU(f,id,n) do { uint16_t n_=(uint16_t)(n); if(n_ && w.f/n_<=65535u) w.f=(w.f%n_)<<16|(w.f/n_); observe(h,HHS_DIVU,id,n_,0); } while(0)

#define AB(f,id,v) do { uint8_t n_=(uint8_t)(v); w.f=low_byte(w.f,(uint8_t)(w.f+n_)); observe(h,HHS_ADD_BYTE,id,n_,0); } while(0)
#define SB(f,id,v) do { uint8_t n_=(uint8_t)(v); w.f=low_byte(w.f,(uint8_t)(w.f-n_)); observe(h,HHS_SUB_BYTE,id,n_,0); } while(0)
#define EW(f,id) do { w.f=low_word(w.f,(uint16_t)(int16_t)(int8_t)w.f); observe(h,HHS_EXT_WORD,id,0,0); } while(0)
#define NL(f,id) do { w.f=0u-w.f; observe(h,HHS_NEG_LONG,id,0,0); } while(0)
#define CL(v,n) observe(h,HHS_COMPARE_LONG,HHS_VALUE,(uint32_t)(v),(uint32_t)(n))
#define TL(v) observe(h,HHS_TEST_LONG,HHS_VALUE,(uint32_t)(v),0)
static int memory_decrement(const HudHistoryStreamHooks *h,gaddr at) {
    uint8_t v=rd_u8(at); wr_u8(at,(uint8_t)(v-1)); observe(h,HHS_MEMORY_SUB_BYTE,HHS_VALUE,v,1); return (int8_t)v-1>=0;
}
static void memory_increment(const HudHistoryStreamHooks *h,gaddr at) {
    uint8_t v=rd_u8(at);wr_u8(at,(uint8_t)(v+1));observe(h,HHS_MEMORY_ADD_BYTE,HHS_VALUE,v,1);
}
static HudHistoryStreamState load_words(HudHistoryStreamState w,const HudHistoryStreamHooks *h,gaddr at,uint16_t mask) {
    (void)w;observe(h,HHS_LOAD_WORDS_AT,HHS_VALUE,at,mask);return restored(h);
}
static HudHistoryStreamState load_longs(HudHistoryStreamState w,const HudHistoryStreamHooks *h,gaddr at,uint16_t mask) {
    (void)w;observe(h,HHS_LOAD_LONGS_AT,HHS_VALUE,at,mask);return restored(h);
}
static void store_words(const HudHistoryStreamHooks *h,gaddr at,uint16_t mask) { observe(h,HHS_STORE_WORDS_AT,HHS_VALUE,at,mask); }
static void history_drawn(HudHistoryStreamState w,const HudHistoryStreamHooks *h) { word(h,w.frame-0x1e,(uint16_t)(rd_u16(w.frame-0x1e)|w.value)); }

/* C0D04C's three source interpolation sites share the same saved current
 * point. Preserve the ordered quarter/half/three-quarter register arithmetic. */
static HudHistoryStreamState history_interpolate(HudHistoryStreamState w,const HudHistoryStreamHooks *h,unsigned fraction) {
    w=load_words(w,h,w.frame-0x2c,0x10b8); SWAP(value,HHS_VALUE);
    P(screen,HHS_SCREEN,w.screen-(gaddr)(int32_t)(int16_t)w.descriptor); W(value,HHS_VALUE,w.screen);
    ASL(secondary,HHS_SECONDARY,w.value); ASL(source,HHS_SOURCE,w.value); ASL(stride,HHS_STRIDE,w.value); SWAP(value,HHS_VALUE);
    if(fraction<3) {
        unsigned shift=fraction==1?2:1;
        SW(value,HHS_VALUE,w.secondary); SW(base,HHS_BASE,w.source); SW(control,HHS_CONTROL,w.stride); SW(size,HHS_SIZE,w.offset);
        ASR(value,HHS_VALUE,shift); ASR(base,HHS_BASE,shift); ASR(control,HHS_CONTROL,shift); ASR(size,HHS_SIZE,shift);
    } else {
        SW(secondary,HHS_SECONDARY,w.value); SW(source,HHS_SOURCE,w.base); SW(stride,HHS_STRIDE,w.control); SW(offset,HHS_OFFSET,w.size);
        ASR(secondary,HHS_SECONDARY,2); ASR(source,HHS_SOURCE,2); ASR(stride,HHS_STRIDE,2); ASR(offset,HHS_OFFSET,2);
    }
    AW(value,HHS_VALUE,w.secondary); AW(base,HHS_BASE,w.source); AW(control,HHS_CONTROL,w.stride); AW(size,HHS_SIZE,w.offset);
    w=consume(h,(enum HudHistoryStreamChild)(HHS_HISTORY_QUARTER+fraction-1));history_drawn(w,h);return w;
}

/* C0D04C: complete history-slot traversal, scale, colour, interpolation and
 * status union. The previous-radius word is the actual overlapping local
 * MOVEM slot at -$26, rather than an invented validity flag. */
void hud_history_projection(HudHistoryStreamState w,const HudHistoryStreamHooks *h) {
    int64_t signed_sum; int32_t sum; uint8_t old;
    W(value,HHS_VALUE,rd_u16(0xc459b6u)); CW(w.value,rd_u16(0xc4fdd2u));
    if((uint16_t)w.value!=rd_u16(0xc4fdd2u)) { L(value,HHS_VALUE,0);return; }
    observe(h,HHS_LINK_FRAME,HHS_VALUE,0x2e,0);w=restored(h);word(h,w.frame-0x26,0xffff);word(h,w.frame-0x1e,0);
    P(table,HHS_TABLE,0xc46184u);P(table,HHS_TABLE,w.table+(gaddr)(int32_t)(int16_t)w.value);B(value,HHS_VALUE,0);
    CB(rd_u8(0xc4fdd0u),6);if(rd_s8(0xc4fdd0u)>=6) B(value,HHS_VALUE,rd_u8(0xc4fdd1u));
    byte(h,w.frame-0x20,(uint8_t)w.value);byte(h,w.frame-0xa,rd_u8(w.table+0x3d));if(rd_s8(w.frame-0xa)<=0) goto finish;
    CB(rd_u8(0xc4fdd0u),6);if(rd_s8(0xc4fdd0u)<6) memory_decrement(h,w.frame-0xa);
    w=load_longs(w,h,w.table+0x3e,7);w=load_longs(w,h,w.table+0x14,0x38);
    SL(secondary,HHS_SECONDARY,rd_u32(0xc45a7cu));SL(source,HHS_SOURCE,rd_u32(0xc45a80u));SL(stride,HHS_STRIDE,rd_u32(0xc45a84u));
    CW(rd_u16(w.table+0x6e),0x100);if(rd_s16(w.table+0x6e)>=0x100) { ARL(value,HHS_VALUE,8);ARL(base,HHS_BASE,8);ARL(control,HHS_CONTROL,8); }
    ARL(secondary,HHS_SECONDARY,8);ARL(source,HHS_SOURCE,8);ARL(stride,HHS_STRIDE,8);
    MUL(value,HHS_VALUE,w.secondary);MUL(base,HHS_BASE,w.source);MUL(control,HHS_CONTROL,w.stride);AL(control,HHS_CONTROL,w.value);
    signed_sum=(int64_t)(int32_t)w.control+(int32_t)w.base;AL(control,HHS_CONTROL,w.base);
    if(signed_sum<0) word(h,w.frame-0x22,0xffff);
    else {
        word(h,w.frame-0x22,0);B(value,HHS_VALUE,rd_u8(0xc4fdd1u));SB(value,HHS_VALUE,rd_u8(0xc4fdd0u));
        CB(rd_u8(0xc4fdd0u),6);if(rd_s8(0xc4fdd0u)<6) SB(value,HHS_VALUE,1);
        sum=(int8_t)w.value+rd_s8(w.table+0x3d);AB(value,HHS_VALUE,rd_u8(w.table+0x3d));if(sum<0) AB(value,HHS_VALUE,rd_u8(0xc4fdd0u));
        byte(h,w.frame-0x20,(uint8_t)w.value);
    }
    do {
        P(table,HHS_TABLE,0xc46184u);P(table,HHS_TABLE,w.table+(gaddr)(int32_t)rd_s16(0xc459b6u));
        B(value,HHS_VALUE,rd_u8(w.frame-0x20));EW(value,HHS_VALUE);AW(value,HHS_VALUE,w.value);AW(value,HHS_VALUE,w.value);
        W(secondary,HHS_SECONDARY,w.value);AW(value,HHS_VALUE,w.value);AW(value,HHS_VALUE,w.secondary);
        P(descriptor,HHS_DESCRIPTOR,0xc4fdd4u);P(descriptor,HHS_DESCRIPTOR,w.descriptor+(gaddr)(int32_t)(int16_t)w.value);
        w=load_longs(w,h,w.descriptor,0x38);SL(secondary,HHS_SECONDARY,rd_u32(0xc45a7cu));SL(source,HHS_SOURCE,rd_u32(0xc45a80u));SL(stride,HHS_STRIDE,rd_u32(0xc45a84u));
        L(offset,HHS_OFFSET,0);L(value,HHS_VALUE,w.secondary);if((int32_t)w.value<0) NL(value,HHS_VALUE);
        L(base,HHS_BASE,w.source);if((int32_t)w.base<0) NL(base,HHS_BASE);L(control,HHS_CONTROL,w.stride);if((int32_t)w.control<0) NL(control,HHS_CONTROL);
        L(size,HHS_SIZE,0x4000);CL(w.value,w.size);
        if((int32_t)w.value<=0x4000) { CL(w.base,w.size);if((int32_t)w.base<=0x4000) { CL(w.control,w.size);if((int32_t)w.control<=0x4000) goto colour; } }
        L(offset,HHS_OFFSET,8);L(size,HHS_SIZE,0x40000);CL(w.value,w.size);
        if((int32_t)w.value<=0x40000) { CL(w.base,w.size);if((int32_t)w.base<=0x40000) { CL(w.control,w.size);if((int32_t)w.control<=0x40000) L(offset,HHS_OFFSET,4); } }
        ARL(secondary,HHS_SECONDARY,w.offset);ARL(source,HHS_SOURCE,w.offset);ARL(stride,HHS_STRIDE,w.offset);
        ARL(value,HHS_VALUE,w.offset);ARL(base,HHS_BASE,w.offset);ARL(control,HHS_CONTROL,w.offset);
colour:
        if(bit(h,w.frame-0x20,0)) { if(bit(h,w.table+0x20,1)) word(h,0xc45954u,3);else word(h,0xc45954u,8); }
        else { if(bit(h,w.table+0x20,1)) word(h,0xc45954u,12);else word(h,0xc45954u,10); }
        W(size,HHS_SIZE,0x9c0);observe(h,HHS_SAVE_WORDS,HHS_VALUE,0x1e00,0);
        W(source,HHS_SOURCE,w.control);W(secondary,HHS_SECONDARY,w.base);W(control,HHS_CONTROL,w.value);w=consume(h,HHS_HISTORY_MAGNITUDE);
        observe(h,HHS_RESTORE_WORDS,HHS_VALUE,0x78,0);w=restored(h);L(value,HHS_VALUE,8);SW(value,HHS_VALUE,w.offset);
        P(descriptor,HHS_DESCRIPTOR,(gaddr)(int32_t)(int16_t)w.offset);ASR(base,HHS_BASE,w.value);W(offset,HHS_OFFSET,w.base);observe(h,HHS_PUSH_WORD,HHS_VALUE,w.size,0);
        w=consume(h,HHS_HISTORY_TRANSFORM);W(value,HHS_VALUE,w.size);W(base,HHS_BASE,w.control);W(control,HHS_CONTROL,w.stride);
        observe(h,HHS_POP_WORD,HHS_SIZE,0,0);w=restored(h);EL(size,HHS_SIZE);TW(w.offset);
        if((int16_t)w.offset>0) { DU(size,HHS_SIZE,w.offset);CW(w.size,127);if((int16_t)w.size<=127) goto radius; }
        W(size,HHS_SIZE,127);
radius:
        TW(rd_u16(w.frame-0x26));if(rd_s16(w.frame-0x26)>=0) {
            CB(rd_u8(w.table+0x3d),1);if(rd_s8(w.table+0x3d)>1) {
                observe(h,HHS_SAVE_WORDS,HHS_VALUE,0xe210,0);
                w=history_interpolate(w,h,1);w=restored(h);w=load_words(w,h,w.stack,0x847);
                w=history_interpolate(w,h,2);w=restored(h);w=load_words(w,h,w.stack,0x847);
                w=history_interpolate(w,h,3);observe(h,HHS_RESTORE_WORDS,HHS_VALUE,0x847,0);w=restored(h);
            }
        }
        store_words(h,w.frame-0x2c,0x847);TB(rd_u8(w.frame-0xa));if(rd_s8(w.frame-0xa)<=0) { W(offset,HHS_OFFSET,w.size);ASR(offset,HHS_OFFSET,3);SW(size,HHS_SIZE,w.offset); }
        w=consume(h,HHS_HISTORY_POINT);history_drawn(w,h);TW(rd_u16(w.frame-0x22));
        if(rd_s16(w.frame-0x22)>=0) {
            if(!memory_decrement(h,w.frame-0x20)) { B(value,HHS_VALUE,rd_u8(0xc4fdd0u));SB(value,HHS_VALUE,1);byte(h,w.frame-0x20,(uint8_t)w.value); }
        } else {
            memory_increment(h,w.frame-0x20);B(value,HHS_VALUE,rd_u8(0xc4fdd0u));SB(value,HHS_VALUE,1);CB(w.value,rd_u8(w.frame-0x20));
            if((int8_t)w.value<rd_s8(w.frame-0x20)) byte(h,w.frame-0x20,0);
        }
        old=rd_u8(w.frame-0xa);memory_decrement(h,w.frame-0xa);
    } while((int8_t)old-1>=0);
finish:
    W(value,HHS_VALUE,rd_u16(w.frame-0x1e));observe(h,HHS_UNLINK_FRAME,HHS_VALUE,0,0);
}

/* Original indirect display-stream children and their saved cursors. */
void hud_stream_point(HudHistoryStreamState w,const HudHistoryStreamHooks *h,int fixed) {
    P(descriptor,HHS_DESCRIPTOR,0xc48390u);P(descriptor,HHS_DESCRIPTOR,w.descriptor+(gaddr)(int32_t)rd_s16(w.stream));P(stream,HHS_STREAM,w.stream+2);
    w=load_words(w,h,w.descriptor,7);word(h,0xc45954u,rd_u16(w.stream));P(stream,HHS_STREAM,w.stream+2);
    observe(h,HHS_SAVE_LONGS,HHS_VALUE,0x60,0);w=consume(h,fixed?HHS_STREAM_FIXED_POINT:HHS_STREAM_POINT);observe(h,HHS_RESTORE_LONGS,HHS_VALUE,0x600,0);
}
void hud_stream_circle(HudHistoryStreamState w,const HudHistoryStreamHooks *h) {
    P(descriptor,HHS_DESCRIPTOR,0xc48390u);P(descriptor,HHS_DESCRIPTOR,w.descriptor+(gaddr)(int32_t)rd_s16(w.stream));P(stream,HHS_STREAM,w.stream+2);
    word(h,0xc45954u,rd_u16(w.stream));P(stream,HHS_STREAM,w.stream+2);W(size,HHS_SIZE,rd_u16(w.stream));P(stream,HHS_STREAM,w.stream+2);
    observe(h,HHS_SAVE_LONGS,HHS_VALUE,0x64,0);w=consume(h,HHS_STREAM_CIRCLE);observe(h,HHS_RESTORE_LONGS,HHS_VALUE,0x2600,0);
}

#define LSR(f,id,n) do { w.f>>=(n);observe(h,HHS_LSR_LONG,id,n,0); } while(0)
#define DS(f,id,n) do { int32_t old_=(int32_t)w.f,q_=old_/(n); if(q_>=-32768&&q_<=32767) w.f=(uint32_t)(uint16_t)(old_%(n))<<16|(uint16_t)q_;observe(h,HHS_DIVS,id,n,0); } while(0)
static HudHistoryStreamState pop_word(HudHistoryStreamState w,const HudHistoryStreamHooks *h,enum HudHistoryStreamField field) { (void)w;observe(h,HHS_POP_WORD,field,0,0);return restored(h); }
static HudHistoryStreamState pop_long(HudHistoryStreamState w,const HudHistoryStreamHooks *h,enum HudHistoryStreamField field) { (void)w;observe(h,HHS_POP_LONG,field,0,0);return restored(h); }
static HudHistoryStreamState pop_display(HudHistoryStreamState w,const HudHistoryStreamHooks *h) { (void)w;observe(h,HHS_POP_MEMORY_LONG,HHS_VALUE,0xc45b22u,0);return restored(h); }
static void add_display(const HudHistoryStreamHooks *h,uint32_t delta) {
    uint32_t old=rd_u32(0xc45b1eu);wr_u32(0xc45b1eu,old+delta);observe(h,HHS_MEMORY_ADD_LONG,HHS_VALUE,old,delta);
}
static void push_display(const HudHistoryStreamHooks *h) { observe(h,HHS_PUSH_LONG,HHS_VALUE,rd_u32(0xc45b22u),0); }

/* C33370's three-byte packed-decimal adjustment. ADD.W #0 immediately
 * precedes every triple in the source, so the first decimal carry is zero.
 * The hook retains original X/C and Musashi's undefined N/V convention. */
static HudHistoryStreamState decimal_step(HudHistoryStreamState w,const HudHistoryStreamHooks *h,int add) {
    unsigned i,carry=0;
    P(table,HHS_TABLE,0xc45b26u);P(stream,HHS_STREAM,0xc45b2au);AW(value,HHS_VALUE,0);
    for(i=0;i<3;++i) {
        uint8_t source,destination;uint32_t result;
        P(stream,HHS_STREAM,w.stream-1);source=rd_u8(w.stream);P(table,HHS_TABLE,w.table-1);destination=rd_u8(w.table);
        if(add) {
            result=(source&15u)+(destination&15u)+carry;if(result>9) result+=6;
            result+=(source&0xf0u)+(destination&0xf0u);carry=result>0x99u;if(carry) result-=0xa0u;
        } else {
            result=(destination&15u)-(source&15u)-carry;if(result>9) result-=6;
            result+=(destination&0xf0u)-(source&0xf0u);carry=result>0x99u;if(carry) result+=0xa0u;
        }
        wr_u8(w.table,(uint8_t)result);observe(h,add?HHS_DECIMAL_ADD:HHS_DECIMAL_SUB,HHS_VALUE,source,destination);
    }
    return w;
}
static HudHistoryStreamState round_tape(HudHistoryStreamState w,const HudHistoryStreamHooks *h,enum HudHistoryStreamChild rounding) {
    L(value,HHS_VALUE,rd_u32(0xc45b22u));L(base,HHS_BASE,w.value);ANDW(value,HHS_VALUE,0xff0f);ANDW(base,HHS_BASE,255);CW(w.base,0x20);
    if((int16_t)w.base>0x20) {
        CW(w.base,0x70);
        if((int16_t)w.base<=0x70) { ORW(value,HHS_VALUE,0x50);L(control,HHS_CONTROL,0xffffffceu); }
        else { L(control,HHS_CONTROL,0xffffff9cu);add_display(h,100);w=consume(h,rounding);L(value,HHS_VALUE,rd_u32(0xc45b22u));ANDW(value,HHS_VALUE,0xff00); }
    }
    LSR(value,HHS_VALUE,4);longword(h,0xc45b22u,w.value);push_display(h);longword(h,0xc45b22u,w.base);return w;
}

static HudHistoryStreamState altitude_tape(HudHistoryStreamState w,const HudHistoryStreamHooks *h) {
    DU(value,HHS_VALUE,10);EL(value,HHS_VALUE);L(control,HHS_CONTROL,0);longword(h,0xc45b1eu,w.value);w=consume(h,HHS_ALTITUDE_ANALOG_PACK);
    w=round_tape(w,h,HHS_ALTITUDE_ROUND_PACK);w=consume(h,HHS_ALTITUDE_UNPACK);w=pop_display(w,h);
    L(base,HHS_BASE,rd_u32(0xc45b1eu));AL(base,HHS_BASE,w.control);DS(base,HHS_BASE,3);W(value,HHS_VALUE,0xdf);AW(value,HHS_VALUE,rd_u16(0xc45988u));
    observe(h,HHS_PUSH_WORD,HHS_VALUE,w.base,0);AW(base,HHS_BASE,0x5b);AW(base,HHS_BASE,rd_u16(0xc458d8u));word(h,0xc4598cu,(uint16_t)w.base);AW(base,HHS_BASE,2);TW(w.value);
    if((int16_t)w.value>0) { CW(w.value,0x13f);if((int16_t)w.value<0x13f) w=consume(h,HHS_ALTITUDE_PIXEL); }
    w=pop_word(w,h,HHS_STRIDE);ASL(stride,HHS_STRIDE,3);W(secondary,HHS_SECONDARY,w.stride);AW(secondary,HHS_SECONDARY,w.secondary);AW(secondary,HHS_SECONDARY,w.secondary);AW(stride,HHS_STRIDE,w.secondary);EL(stride,HHS_STRIDE);AL(stride,HHS_STRIDE,0xe02);
    observe(h,HHS_PUSH_LONG,HHS_VALUE,w.stride,0);observe(h,HHS_PUSH_WORD,HHS_VALUE,w.base,0);w=consume(h,HHS_ALTITUDE_TICK);longword(h,0xc45b26u,5);
    w=pop_word(w,h,HHS_BASE);w=pop_long(w,h,HHS_STRIDE);push_display(h);observe(h,HHS_PUSH_LONG,HHS_VALUE,w.stride,0);observe(h,HHS_PUSH_WORD,HHS_VALUE,w.base,0);
    for(;;) {
        SL(stride,HHS_STRIDE,600);CL(w.stride,0xaf0);if((int32_t)w.stride<0xaf0) break;
        SW(base,HHS_BASE,15);observe(h,HHS_PUSH_WORD,HHS_VALUE,w.base,0);observe(h,HHS_PUSH_LONG,HHS_VALUE,w.stride,0);W(value,HHS_VALUE,0xdf);w=consume(h,HHS_ALTITUDE_DOWN_PIXEL);
        w=pop_long(w,h,HHS_STRIDE);observe(h,HHS_PUSH_LONG,HHS_VALUE,w.stride,0);w=decimal_step(w,h,1);w=consume(h,HHS_ALTITUDE_DOWN_TICK);
        w=pop_long(w,h,HHS_STRIDE);w=pop_word(w,h,HHS_BASE);
    }
    w=pop_word(w,h,HHS_BASE);w=pop_long(w,h,HHS_STRIDE);w=pop_display(w,h);
    for(;;) {
        TL(rd_u32(0xc45b22u));if(!rd_u32(0xc45b22u)) break;
        AL(stride,HHS_STRIDE,600);CL(w.stride,0x1130);if((int32_t)w.stride>0x1130) break;
        AW(base,HHS_BASE,15);observe(h,HHS_PUSH_WORD,HHS_VALUE,w.base,0);observe(h,HHS_PUSH_LONG,HHS_VALUE,w.stride,0);W(value,HHS_VALUE,0xdf);w=consume(h,HHS_ALTITUDE_UP_PIXEL);
        w=pop_long(w,h,HHS_STRIDE);observe(h,HHS_PUSH_LONG,HHS_VALUE,w.stride,0);w=decimal_step(w,h,0);w=consume(h,HHS_ALTITUDE_UP_TICK);
        w=pop_long(w,h,HHS_STRIDE);w=pop_word(w,h,HHS_BASE);
    }
    W(value,HHS_VALUE,0xd4);W(base,HHS_BASE,1);w=consume(h,HHS_ALTITUDE_BASE);W(value,HHS_VALUE,0xce);return consume(h,HHS_ALTITUDE_SCALE);
}

static HudHistoryStreamState speed_tape(HudHistoryStreamState w,const HudHistoryStreamHooks *h) {
    EL(value,HHS_VALUE);DU(value,HHS_VALUE,12);EL(value,HHS_VALUE);L(control,HHS_CONTROL,0);longword(h,0xc45b1eu,w.value);w=consume(h,HHS_SPEED_ANALOG_PACK);
    w=round_tape(w,h,HHS_SPEED_ROUND_PACK);w=consume(h,HHS_SPEED_UNPACK);w=pop_display(w,h);
    L(base,HHS_BASE,rd_u32(0xc45b1eu));AL(base,HHS_BASE,w.control);DS(base,HHS_BASE,3);W(stride,HHS_STRIDE,w.base);AW(base,HHS_BASE,0x5b);AW(base,HHS_BASE,rd_u16(0xc458d8u));word(h,0xc4598cu,(uint16_t)w.base);
    ASL(stride,HHS_STRIDE,3);W(secondary,HHS_SECONDARY,w.stride);AW(secondary,HHS_SECONDARY,w.secondary);AW(secondary,HHS_SECONDARY,w.secondary);AW(stride,HHS_STRIDE,w.secondary);EL(stride,HHS_STRIDE);AL(stride,HHS_STRIDE,0xdf2);
    observe(h,HHS_PUSH_LONG,HHS_VALUE,w.stride,0);w=consume(h,HHS_SPEED_TICK);w=pop_long(w,h,HHS_STRIDE);longword(h,0xc45b26u,5);push_display(h);observe(h,HHS_PUSH_LONG,HHS_VALUE,w.stride,0);
    for(;;) {
        SL(stride,HHS_STRIDE,600);CL(w.stride,0xaf0);if((int32_t)w.stride<0xaf0) break;
        w=decimal_step(w,h,1);observe(h,HHS_PUSH_LONG,HHS_VALUE,w.stride,0);w=consume(h,HHS_SPEED_DOWN_TICK);w=pop_long(w,h,HHS_STRIDE);
    }
    w=pop_long(w,h,HHS_STRIDE);w=pop_display(w,h);
    for(;;) {
        TL(rd_u32(0xc45b22u));if(!rd_u32(0xc45b22u)) break;
        AL(stride,HHS_STRIDE,600);CL(w.stride,0x1130);if((int32_t)w.stride>0x1130) break;
        w=decimal_step(w,h,0);observe(h,HHS_PUSH_LONG,HHS_VALUE,w.stride,0);w=consume(h,HHS_SPEED_UP_TICK);w=pop_long(w,h,HHS_STRIDE);
    }
    W(value,HHS_VALUE,0x69);W(base,HHS_BASE,0);w=consume(h,HHS_SPEED_BASE);W(value,HHS_VALUE,0x6b);w=consume(h,HHS_SPEED_SCALE);
    W(value,HHS_VALUE,0x6e);AW(value,HHS_VALUE,rd_u16(0xc45988u));CW(w.value,5);if((int16_t)w.value<=5) return w;
    CW(w.value,0x13b);if((int16_t)w.value>=0x13b) return w;
    W(base,HHS_BASE,0x56);AW(base,HHS_BASE,rd_u16(0xc458d8u));w=consume(h,HHS_SPEED_LEFT_FIRST);W(value,HHS_VALUE,0x6c);W(base,HHS_BASE,0x57);w=consume(h,HHS_SPEED_POINT_FIRST);
    AW(base,HHS_BASE,1);w=consume(h,HHS_SPEED_RIGHT_FIRST);W(value,HHS_VALUE,0x6e);AW(value,HHS_VALUE,rd_u16(0xc45988u));AW(base,HHS_BASE,1);return consume(h,HHS_SPEED_LEFT_SECOND);
}

static HudHistoryStreamState heading_tape(HudHistoryStreamState w,const HudHistoryStreamHooks *h) {
    P(registers,HHS_REGISTERS,0xc46184u);P(registers,HHS_REGISTERS,w.registers+(gaddr)(int32_t)rd_s16(0xc458deu));W(value,HHS_VALUE,rd_u16(w.registers+0x68));ASR(value,HHS_VALUE,3);EL(value,HHS_VALUE);L(control,HHS_CONTROL,0);longword(h,0xc45b1eu,w.value);w=consume(h,HHS_HEADING_PACK);
    L(value,HHS_VALUE,rd_u32(0xc45b22u));L(base,HHS_BASE,w.value);ANDW(value,HHS_VALUE,0xff0f);ANDW(base,HHS_BASE,255);CW(w.base,0x40);
    if((int16_t)w.base>0x40) { L(control,HHS_CONTROL,0xffffff9cu);add_display(h,100);w=consume(h,HHS_HEADING_ROUND_PACK);L(value,HHS_VALUE,rd_u32(0xc45b22u));ANDW(value,HHS_VALUE,0xff00); }
    LSR(value,HHS_VALUE,4);CL(w.value,0x360);if((int32_t)w.value>=0x360) L(value,HHS_VALUE,0);longword(h,0xc45b22u,w.value);push_display(h);longword(h,0xc45b22u,w.base);
    w=consume(h,HHS_HEADING_UNPACK);w=pop_display(w,h);L(base,HHS_BASE,rd_u32(0xc45b1eu));AL(base,HHS_BASE,w.control);DS(base,HHS_BASE,5);NEG(base,HHS_BASE);W(stride,HHS_STRIDE,w.base);AW(stride,HHS_STRIDE,159);word(h,0xc4598cu,(uint16_t)w.stride);
    ASL(base,HHS_BASE,3);P(table,HHS_TABLE,0xc33a16u);P(table,HHS_TABLE,w.table+(gaddr)(int32_t)(int16_t)w.base);observe(h,HHS_PUSH_WORD,HHS_VALUE,w.base,0);w=consume(h,HHS_HEADING_TICK);w=pop_word(w,h,HHS_BASE);
    longword(h,0xc45b26u,10);push_display(h);observe(h,HHS_PUSH_WORD,HHS_VALUE,w.base,0);
    for(;;) {
        AW(base,HHS_BASE,0xa0);CW(w.base,0xb0);if((int16_t)w.base>0xb0) break;
        w=decimal_step(w,h,1);CL(rd_u32(0xc45b22u),0x360);if(rd_s32(0xc45b22u)>=0x360) longword(h,0xc45b22u,0);
        P(table,HHS_TABLE,0xc33a16u);P(table,HHS_TABLE,w.table+(gaddr)(int32_t)(int16_t)w.base);observe(h,HHS_PUSH_WORD,HHS_VALUE,w.base,0);w=consume(h,HHS_HEADING_RIGHT_TICK);w=pop_word(w,h,HHS_BASE);
    }
    w=pop_word(w,h,HHS_BASE);w=pop_display(w,h);
    for(;;) {
        TL(rd_u32(0xc45b22u));if(rd_s32(0xc45b22u)<=0) longword(h,0xc45b22u,0x360);
        SW(base,HHS_BASE,0xa0);CW(w.base,0xff50);if((int16_t)w.base<-0xb0) break;
        w=decimal_step(w,h,0);P(table,HHS_TABLE,0xc33a16u);P(table,HHS_TABLE,w.table+(gaddr)(int32_t)(int16_t)w.base);observe(h,HHS_PUSH_WORD,HHS_VALUE,w.base,0);w=consume(h,HHS_HEADING_LEFT_TICK);w=pop_word(w,h,HHS_BASE);
    }
    L(base,HHS_BASE,0x3d);AW(base,HHS_BASE,rd_u16(0xc458d8u));return consume(h,HHS_HEADING_END);
}

/* C33370: complete altitude, speed, centre mark and heading display. Each
 * tape consumes its actual child state; source stack slots restore only the
 * originally saved values. Both numeric and analog record classes are kept. */
void hud_postflight_display(HudHistoryStreamState w,const HudHistoryStreamHooks *h) {
    word(h,0xc45954u,13);P(registers,HHS_REGISTERS,0xc46184u);P(registers,HHS_REGISTERS,w.registers+(gaddr)(int32_t)rd_s16(0xc458deu));TB(rd_u8(0xc457a4u));
    if(rd_u8(0xc457a4u)) { L(value,HHS_VALUE,rd_u32(0xc45658u));CL(w.value,99999);if((int32_t)w.value>99999) L(value,HHS_VALUE,99999); }
    else { L(value,HHS_VALUE,rd_u32(w.registers+0x18));ARL(value,HHS_VALUE,7);ARL(value,HHS_VALUE,3);L(base,HHS_BASE,w.value);AL(value,HHS_VALUE,w.value);AL(value,HHS_VALUE,w.value);AL(value,HHS_VALUE,w.base); }
    CB(rd_u8(w.registers+0x62),0x10);
    if(rd_u8(w.registers+0x62)==0x10) w=altitude_tape(w,h);
    else {
        P(stream,HHS_STREAM,0xc457fau);P(registers,HHS_REGISTERS,w.stream+6);L(size,HHS_SIZE,5);L(offset,HHS_OFFSET,5);longword(h,0xc45b1eu,w.value);w=consume(h,HHS_ALTITUDE_PACK);
        P(table,HHS_TABLE,0xc33270u);W(control,HHS_CONTROL,w.offset);P(screen,HHS_SCREEN,0xd38);W(value,HHS_VALUE,0x18);SWAP(value,HHS_VALUE);W(value,HHS_VALUE,w.size);w=consume(h,HHS_ALTITUDE_DIGITS);
        W(value,HHS_VALUE,0xca);P(stream,HHS_STREAM,0xc33418u);P(registers,HHS_REGISTERS,w.stream+2);P(table,HHS_TABLE,0xc3328cu);P(screen,HHS_SCREEN,0xe52);W(value,HHS_VALUE,0x1a);SWAP(value,HHS_VALUE);W(value,HHS_VALUE,1);w=consume(h,HHS_ALTITUDE_LABEL);
    }
    P(registers,HHS_REGISTERS,0xc46184u);P(registers,HHS_REGISTERS,w.registers+(gaddr)(int32_t)rd_s16(0xc458deu));L(value,HHS_VALUE,0);
    if(!bit(h,w.registers,7)) { W(value,HHS_VALUE,rd_u16(w.registers+0x6e));if((int16_t)w.value<0) NEG(value,HHS_VALUE); }
    CB(rd_u8(w.registers+0x62),0x10);
    if(rd_u8(w.registers+0x62)==0x10) w=speed_tape(w,h);
    else {
        P(stream,HHS_STREAM,0xc457fau);P(registers,HHS_REGISTERS,w.stream+4);L(size,HHS_SIZE,3);L(offset,HHS_OFFSET,3);EL(value,HHS_VALUE);DU(value,HHS_VALUE,12);EL(value,HHS_VALUE);longword(h,0xc45b1eu,w.value);w=consume(h,HHS_SPEED_PACK);
        P(table,HHS_TABLE,0xc33294u);W(control,HHS_CONTROL,w.offset);P(screen,HHS_SCREEN,0xd2a);W(value,HHS_VALUE,10);SWAP(value,HHS_VALUE);W(value,HHS_VALUE,w.size);w=consume(h,HHS_SPEED_DIGITS);
        W(value,HHS_VALUE,0x71);P(stream,HHS_STREAM,0xc33642u);P(registers,HHS_REGISTERS,w.stream+2);P(table,HHS_TABLE,0xc332a8u);P(screen,HHS_SCREEN,0xe44);W(value,HHS_VALUE,12);SWAP(value,HHS_VALUE);W(value,HHS_VALUE,1);w=consume(h,HHS_SPEED_LABEL);
    }
    W(value,HHS_VALUE,159);W(base,HHS_BASE,63);w=consume(h,HHS_CENTRE_POINT);
    if(!w.less) { AW(base,HHS_BASE,1);w=consume(h,HHS_CENTRE_RIGHT);AW(base,HHS_BASE,1);w=consume(h,HHS_CENTRE_SECOND); }
    (void)heading_tape(w,h);
}
