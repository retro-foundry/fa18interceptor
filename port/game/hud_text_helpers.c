/* Original numeric HUD fields, weapon cues and shared small-text tails. */
#include "hud_text_helpers.h"
#include <stdlib.h>
static void observe(const HudTextHooks *h,enum HudTextPhase p,enum HudTextField f,uint32_t v,uint32_t o) {
    if(h && h->observe) h->observe(h->context,p,f,v,o);
}
static HudTextState consume(const HudTextHooks *h,enum HudTextChild child) {
    if(h && h->consume) return h->consume(h->context,child); abort();
}
static HudTextState restored(const HudTextHooks *h) {
    if(h && h->restored) return h->restored(h->context); abort();
}
static uint32_t low_word(uint32_t old,uint16_t v) { return (old&0xffff0000u)|v; }
static uint32_t low_byte(uint32_t old,uint8_t v) { return (old&0xffffff00u)|v; }
#define W(f,id,v) do { w.f=low_word(w.f,(uint16_t)(v)); observe(h,HTH_WORD,id,w.f,0); } while(0)
#define B(f,id,v) do { w.f=low_byte(w.f,(uint8_t)(v)); observe(h,HTH_BYTE,id,w.f,0); } while(0)
#define L(f,id,v) do { w.f=(uint32_t)(v); observe(h,HTH_LONG,id,w.f,0); } while(0)
#define P(f,id,v) do { w.f=(gaddr)(v); observe(h,HTH_POINTER,id,w.f,0); } while(0)
#define AW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f+n_)); observe(h,HTH_ADD_WORD,id,n_,0); } while(0)
#define SW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f-n_)); observe(h,HTH_SUB_WORD,id,n_,0); } while(0)
#define AL(f,id,v) do { uint32_t n_=(uint32_t)(v); w.f+=n_; observe(h,HTH_ADD_LONG,id,n_,0); } while(0)
#define SL(f,id,v) do { uint32_t n_=(uint32_t)(v); w.f-=n_; observe(h,HTH_SUB_LONG,id,n_,0); } while(0)
#define ANDW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f&n_)); observe(h,HTH_AND_WORD,id,n_,0); } while(0)
#define ANDB(f,id,v) do { uint8_t n_=(uint8_t)(v); w.f=low_byte(w.f,(uint8_t)(w.f&n_)); observe(h,HTH_AND_BYTE,id,n_,0); } while(0)
#define ORW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f|n_)); observe(h,HTH_OR_WORD,id,n_,0); } while(0)
#define EL(f,id) do { w.f=(uint32_t)(int32_t)(int16_t)w.f; observe(h,HTH_EXT_LONG,id,0,0); } while(0)
#define NEG(f,id) do { w.f=low_word(w.f,(uint16_t)(0u-w.f)); observe(h,HTH_NEG_WORD,id,0,0); } while(0)
#define SWAP(f,id) do { w.f=(w.f<<16)|(w.f>>16); observe(h,HTH_SWAP,id,0,0); } while(0)
#define ASL(f,id,n) do { w.f=low_word(w.f,(uint16_t)(w.f<<(n))); observe(h,HTH_ASL_WORD,id,n,0); } while(0)
#define LSL(f,id,n) do { w.f=low_word(w.f,(uint16_t)(w.f<<(n))); observe(h,HTH_LSL_WORD,id,n,0); } while(0)
#define ASR(f,id,n) do { w.f=low_word(w.f,(uint16_t)((int16_t)w.f>>(n))); observe(h,HTH_ASR_WORD,id,n,0); } while(0)
#define CW(v,n) observe(h,HTH_COMPARE_WORD,HTH_VALUE,(uint16_t)(v),(uint16_t)(n))
#define CB(v,n) observe(h,HTH_COMPARE_BYTE,HTH_VALUE,(uint8_t)(v),(uint8_t)(n))
#define TW(v) observe(h,HTH_TEST_WORD,HTH_VALUE,(uint16_t)(v),0)
#define TB(v) observe(h,HTH_TEST_BYTE,HTH_VALUE,(uint8_t)(v),0)
static void word(const HudTextHooks *h,gaddr a,uint16_t v) { wr_u16(a,v); observe(h,HTH_STORE_WORD,HTH_VALUE,v,0); }
static void byte(const HudTextHooks *h,gaddr a,uint8_t v) { wr_u8(a,v); observe(h,HTH_STORE_BYTE,HTH_VALUE,v,0); }
static void longword(const HudTextHooks *h,gaddr a,uint32_t v) { wr_u32(a,v); observe(h,HTH_STORE_LONG,HTH_VALUE,v,0); }
static void decrement_byte(const HudTextHooks *h,gaddr a) { uint8_t old=rd_u8(a); wr_u8(a,(uint8_t)(old-1)); observe(h,HTH_MEMORY_SUB_BYTE,HTH_VALUE,old,1); }
static int bit(const HudTextHooks *h,gaddr a,unsigned b) { uint8_t v=rd_u8(a); observe(h,HTH_BIT_TEST,HTH_VALUE,v,b); return (v>>b)&1; }

static void draw_glyphs(HudTextState w,const HudTextHooks *h) {
    int32_t sum;
    P(screen,HTH_SCREEN,w.screen+w.offset); W(offset,HTH_OFFSET,0x142); P(registers,HTH_REGISTERS,rd_u32(0xc456b6u));
    L(base,HTH_BASE,rd_u32(w.registers+(gaddr)(int32_t)(int16_t)w.stride)); AW(size,HTH_SIZE,w.size);
    do {
        W(stride,HTH_STRIDE,rd_u16(w.table)); P(table,HTH_TABLE,w.table+2); W(secondary,HTH_SECONDARY,rd_u16(w.table)); P(table,HTH_TABLE,w.table+2);
        B(source,HTH_SOURCE,rd_u8(w.stream)); P(stream,HTH_STREAM,w.stream+1); W(control,HTH_CONTROL,w.modulo); AW(control,HTH_CONTROL,w.size);
        sum=(int16_t)w.control+(int16_t)w.stride; AW(control,HTH_CONTROL,w.stride);
        if(sum>=0) {
            CW(w.control,40); if((int16_t)w.control<40) {
                AW(stride,HTH_STRIDE,w.size); EL(stride,HTH_STRIDE); AL(stride,HTH_STRIDE,w.screen); AL(stride,HTH_STRIDE,w.base);
                ANDW(source,HTH_SOURCE,255); SW(source,HTH_SOURCE,32); AW(source,HTH_SOURCE,w.source); P(descriptor,HTH_DESCRIPTOR,0xc3d790u);
                P(descriptor,HTH_DESCRIPTOR,w.descriptor+(gaddr)(int32_t)rd_s16(w.descriptor+(gaddr)(int32_t)(int16_t)w.source));
                L(source,HTH_SOURCE,w.descriptor); SWAP(size,HTH_SIZE); W(control,HTH_CONTROL,w.size); SWAP(size,HTH_SIZE); ORW(control,HTH_CONTROL,w.secondary);
                observe(h,HTH_BIT_TEST,HTH_STRIDE,w.stride,0);
                if(!(w.stride&1)) { observe(h,HTH_PUSH_LONG,HTH_BASE,w.base,0); L(base,HTH_BASE,w.stride); w=consume(h,HTH_SMALL_GLYPH); observe(h,HTH_POP_LONG,HTH_BASE,0,0); w=restored(h); }
                else { word(h,0xc4599eu,70); w=consume(h,HTH_FAULT); }
            }
        }
        w.value=low_word(w.value,(uint16_t)(w.value-1)); observe(h,HTH_DECREMENT,HTH_VALUE,w.value,0);
    } while((uint16_t)w.value!=0xffff);
}
static void small_digits(HudTextState w,const HudTextHooks *h) {
    L(secondary,HTH_SECONDARY,rd_u32(0xc45b22u));
    do {
        W(base,HTH_BASE,w.secondary); ANDW(base,HTH_BASE,15); AW(base,HTH_BASE,48); CW(w.base,57); if((int16_t)w.base>57) AW(base,HTH_BASE,7);
        P(registers,HTH_REGISTERS,w.registers-1); byte(h,w.registers,(uint8_t)w.base); w.secondary>>=4; observe(h,HTH_LSR_LONG,HTH_SECONDARY,4,0);
        w.control=low_word(w.control,(uint16_t)(w.control-1)); observe(h,HTH_DECREMENT,HTH_CONTROL,w.control,0);
    } while((uint16_t)w.control!=0xffff);
    TB(w.source);
    if(!(uint8_t)w.source) {
        W(control,HTH_CONTROL,w.value); SW(control,HTH_CONTROL,1);
        do {
            uint8_t digit=rd_u8(w.registers); P(registers,HTH_REGISTERS,w.registers+1); CB(digit,48); if(digit!=48) break;
            byte(h,w.registers-1,32); w.control=low_word(w.control,(uint16_t)(w.control-1)); observe(h,HTH_DECREMENT,HTH_CONTROL,w.control,0);
        } while((uint16_t)w.control!=0xffff);
    }
    draw_glyphs(w,h);
}


#define LSR(f,id,n) do { w.f>>=(n); observe(h,HTH_LSR_LONG,id,n,0); } while(0)
static gaddr indexed(gaddr base,uint32_t value) { return base+(gaddr)(int32_t)(int16_t)value; }
static int decrement(uint32_t *v,enum HudTextField field,const HudTextHooks *h) {
    *v=low_word(*v,(uint16_t)(*v-1)); observe(h,HTH_DECREMENT,field,*v,0); return (uint16_t)*v!=0xffff;
}
static void view_glyphs(HudTextState w,const HudTextHooks *h) {
    unsigned plane; int32_t sum;
    P(screen,HTH_SCREEN,w.screen+w.offset); W(offset,HTH_OFFSET,322); P(modulo,HTH_MODULO,rd_u32(0xc456b6u)); AW(size,HTH_SIZE,w.size);
    do {
        W(stride,HTH_STRIDE,rd_u16(w.table)); P(table,HTH_TABLE,w.table+2); W(secondary,HTH_SECONDARY,rd_u16(w.table)); P(table,HTH_TABLE,w.table+2);
        B(source,HTH_SOURCE,rd_u8(w.stream)); P(stream,HTH_STREAM,w.stream+1); CB(w.source,32); if((uint8_t)w.source==32) goto next_glyph;
        SWAP(value,HTH_VALUE); W(control,HTH_CONTROL,w.value); SWAP(value,HTH_VALUE); AW(control,HTH_CONTROL,w.size);
        sum=(int16_t)w.control+(int16_t)w.stride; AW(control,HTH_CONTROL,w.stride); if(sum<0) goto next_glyph;
        CW(w.control,40); if((int16_t)w.control>=40) goto next_glyph;
        AW(stride,HTH_STRIDE,w.size); EL(stride,HTH_STRIDE); AL(stride,HTH_STRIDE,w.screen);
        ANDW(source,HTH_SOURCE,255); SW(source,HTH_SOURCE,32); AW(source,HTH_SOURCE,w.source); P(descriptor,HTH_DESCRIPTOR,0xc3d790u); P(descriptor,HTH_DESCRIPTOR,indexed(w.descriptor,rd_u16(indexed(w.descriptor,w.source))));
        L(source,HTH_SOURCE,w.descriptor); L(base,HTH_BASE,rd_u32(w.modulo)); AL(base,HTH_BASE,w.stride);
        observe(h,HTH_BIT_TEST,HTH_VALUE,w.base,0); if(w.base&1) goto next_glyph;
        for(plane=0;plane<4;++plane) {
            if(plane) { L(base,HTH_BASE,rd_u32(w.modulo+4*plane)); AL(base,HTH_BASE,w.stride); }
            W(control,HTH_CONTROL,0xb0a); observe(h,HTH_BIT_TEST,HTH_VALUE,rd_u8(0xc45955u),3-plane);
            if(rd_u8(0xc45955u)&(1u<<(3-plane))) W(control,HTH_CONTROL,0xbfa);
            ORW(control,HTH_CONTROL,w.secondary); w=consume(h,(enum HudTextChild)(HTH_VIEW_FIRST+plane));
        }
next_glyph:
        ;
    } while(decrement(&w.value,HTH_VALUE,h));
}

static void view_digits(HudTextState w,const HudTextHooks *h) {
    int32_t sum;
    L(secondary,HTH_SECONDARY,rd_u32(0xc45b22u)); W(stride,HTH_STRIDE,w.control);
    do {
        W(base,HTH_BASE,w.secondary); ANDW(base,HTH_BASE,15); AW(base,HTH_BASE,48);
        P(registers,HTH_REGISTERS,w.registers-1); byte(h,w.registers,(uint8_t)w.base); LSR(secondary,HTH_SECONDARY,4);
    } while(decrement(&w.control,HTH_CONTROL,h));
    observe(h,HTH_TEST_BYTE,HTH_VALUE,w.source,0);
    if(!(uint8_t)w.source) {
        SW(stride,HTH_STRIDE,1);
        for(;;) {
            uint8_t c=rd_u8(w.registers); P(registers,HTH_REGISTERS,w.registers+1); CB(c,48); if(c!=48) break;
            byte(h,w.registers-1,32); sum=(int16_t)w.stride-1; SW(stride,HTH_STRIDE,1); if(sum<0) break;
        }
    }
    view_glyphs(w,h);
}

void hud_text_cache(HudTextState w,const HudTextHooks *h) {
    TB(rd_u8(0xc45837u));
    if(rd_s8(0xc45837u)<=0) {
        W(control,HTH_CONTROL,rd_u16(w.table));
        if((int16_t)w.control>=0) {
            CW(w.value,w.control); if((uint16_t)w.value==(uint16_t)w.control) { L(value,HTH_VALUE,0xffffffffu); return; }
            if(bit(h,0xc458dbu,0)) { L(value,HTH_VALUE,0xffffffffu); return; }
            TB(rd_u8(0xc457aeu)); if(!rd_u8(0xc457aeu)) goto publish;
        }
        word(h,w.table,(uint16_t)(rd_u16(w.table)&0x7fff)); ANDW(control,HTH_CONTROL,0x7fff); W(value,HTH_VALUE,w.control); goto finish;
    }
publish:
    word(h,w.table,(uint16_t)w.value); word(h,w.table,(uint16_t)(rd_u16(w.table)|0x8000));
finish:
    TW(w.value);
}
void hud_text_small_hex(HudTextState w,const HudTextHooks *h) {
    SWAP(size,HTH_SIZE); W(size,HTH_SIZE,0); L(offset,HTH_OFFSET,0); W(control,HTH_CONTROL,w.value); L(source,HTH_SOURCE,0); small_digits(w,h);
}
void hud_text_small_fixed(HudTextState w,const HudTextHooks *h) {
    SWAP(size,HTH_SIZE); W(size,HTH_SIZE,0); L(offset,HTH_OFFSET,0); L(source,HTH_SOURCE,1); small_digits(w,h);
}
void hud_text_small_inverse(HudTextState w,const HudTextHooks *h) {
    W(size,HTH_SIZE,0xf3a); SWAP(size,HTH_SIZE); W(size,HTH_SIZE,rd_u16(0xc45986u)); L(offset,HTH_OFFSET,rd_u32(0xc45918u)); W(control,HTH_CONTROL,w.value); L(source,HTH_SOURCE,0); small_digits(w,h);
}
void hud_text_small_line(HudTextState w,const HudTextHooks *h) { draw_glyphs(w,h); }
void hud_text_view_digits(HudTextState w,const HudTextHooks *h,int clear_leading) {
    if(clear_leading) L(source,HTH_SOURCE,0); W(size,HTH_SIZE,rd_u16(0xc45986u)); L(offset,HTH_OFFSET,rd_u32(0xc45918u)); view_digits(w,h);
}
void hud_text_view_line(HudTextState w,const HudTextHooks *h) {
    W(size,HTH_SIZE,rd_u16(0xc45986u)); L(offset,HTH_OFFSET,rd_u32(0xc45918u)); view_glyphs(w,h);
}
