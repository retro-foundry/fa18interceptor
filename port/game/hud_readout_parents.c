/* Original numeric HUD fields, weapon cues and shared small-text tails. */
#include "hud_readout_parents.h"
#include <stdlib.h>
static void observe(const HudReadoutParentHooks *h,enum HudReadoutParentPhase p,enum HudReadoutParentField f,uint32_t v,uint32_t o) {
    if(h && h->observe) h->observe(h->context,p,f,v,o);
}
static HudReadoutParentState consume(const HudReadoutParentHooks *h,enum HudReadoutParentChild child) {
    if(h && h->consume) return h->consume(h->context,child); abort();
}
static HudReadoutParentState restored(const HudReadoutParentHooks *h) {
    if(h && h->restored) return h->restored(h->context); abort();
}
static uint32_t low_word(uint32_t old,uint16_t v) { return (old&0xffff0000u)|v; }
static uint32_t low_byte(uint32_t old,uint8_t v) { return (old&0xffffff00u)|v; }
#define W(f,id,v) do { w.f=low_word(w.f,(uint16_t)(v)); observe(h,HRP_WORD,id,w.f,0); } while(0)
#define B(f,id,v) do { w.f=low_byte(w.f,(uint8_t)(v)); observe(h,HRP_BYTE,id,w.f,0); } while(0)
#define L(f,id,v) do { w.f=(uint32_t)(v); observe(h,HRP_LONG,id,w.f,0); } while(0)
#define P(f,id,v) do { w.f=(gaddr)(v); observe(h,HRP_POINTER,id,w.f,0); } while(0)
#define AW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f+n_)); observe(h,HRP_ADD_WORD,id,n_,0); } while(0)
#define SW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f-n_)); observe(h,HRP_SUB_WORD,id,n_,0); } while(0)
#define AL(f,id,v) do { uint32_t n_=(uint32_t)(v); w.f+=n_; observe(h,HRP_ADD_LONG,id,n_,0); } while(0)
#define SL(f,id,v) do { uint32_t n_=(uint32_t)(v); w.f-=n_; observe(h,HRP_SUB_LONG,id,n_,0); } while(0)
#define ANDW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f&n_)); observe(h,HRP_AND_WORD,id,n_,0); } while(0)
#define ANDB(f,id,v) do { uint8_t n_=(uint8_t)(v); w.f=low_byte(w.f,(uint8_t)(w.f&n_)); observe(h,HRP_AND_BYTE,id,n_,0); } while(0)
#define ORW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f|n_)); observe(h,HRP_OR_WORD,id,n_,0); } while(0)
#define EL(f,id) do { w.f=(uint32_t)(int32_t)(int16_t)w.f; observe(h,HRP_EXT_LONG,id,0,0); } while(0)
#define NEG(f,id) do { w.f=low_word(w.f,(uint16_t)(0u-w.f)); observe(h,HRP_NEG_WORD,id,0,0); } while(0)
#define SWAP(f,id) do { w.f=(w.f<<16)|(w.f>>16); observe(h,HRP_SWAP,id,0,0); } while(0)
#define ASL(f,id,n) do { w.f=low_word(w.f,(uint16_t)(w.f<<(n))); observe(h,HRP_ASL_WORD,id,n,0); } while(0)
#define LSL(f,id,n) do { w.f=low_word(w.f,(uint16_t)(w.f<<(n))); observe(h,HRP_LSL_WORD,id,n,0); } while(0)
#define ASR(f,id,n) do { w.f=low_word(w.f,(uint16_t)((int16_t)w.f>>(n))); observe(h,HRP_ASR_WORD,id,n,0); } while(0)
#define CW(v,n) observe(h,HRP_COMPARE_WORD,HRP_VALUE,(uint16_t)(v),(uint16_t)(n))
#define CB(v,n) observe(h,HRP_COMPARE_BYTE,HRP_VALUE,(uint8_t)(v),(uint8_t)(n))
#define TW(v) observe(h,HRP_TEST_WORD,HRP_VALUE,(uint16_t)(v),0)
#define TB(v) observe(h,HRP_TEST_BYTE,HRP_VALUE,(uint8_t)(v),0)
static void word(const HudReadoutParentHooks *h,gaddr a,uint16_t v) { wr_u16(a,v); observe(h,HRP_STORE_WORD,HRP_VALUE,v,0); }
static void byte(const HudReadoutParentHooks *h,gaddr a,uint8_t v) { wr_u8(a,v); observe(h,HRP_STORE_BYTE,HRP_VALUE,v,0); }
static void longword(const HudReadoutParentHooks *h,gaddr a,uint32_t v) { wr_u32(a,v); observe(h,HRP_STORE_LONG,HRP_VALUE,v,0); }
static void decrement_byte(const HudReadoutParentHooks *h,gaddr a) { uint8_t old=rd_u8(a); wr_u8(a,(uint8_t)(old-1)); observe(h,HRP_MEMORY_SUB_BYTE,HRP_VALUE,old,1); }
static int bit(const HudReadoutParentHooks *h,gaddr a,unsigned b) { uint8_t v=rd_u8(a); observe(h,HRP_BIT_TEST,HRP_VALUE,v,b); return (v>>b)&1; }

static void draw_glyphs(HudReadoutParentState w,const HudReadoutParentHooks *h) {
    int32_t sum;
    P(screen,HRP_SCREEN,w.screen+w.offset); W(offset,HRP_OFFSET,0x142); P(registers,HRP_REGISTERS,rd_u32(0xc456b6u));
    L(base,HRP_BASE,rd_u32(w.registers+(gaddr)(int32_t)(int16_t)w.stride)); AW(size,HRP_SIZE,w.size);
    do {
        W(stride,HRP_STRIDE,rd_u16(w.table)); P(table,HRP_TABLE,w.table+2); W(secondary,HRP_SECONDARY,rd_u16(w.table)); P(table,HRP_TABLE,w.table+2);
        B(source,HRP_SOURCE,rd_u8(w.stream)); P(stream,HRP_STREAM,w.stream+1); W(control,HRP_CONTROL,w.modulo); AW(control,HRP_CONTROL,w.size);
        sum=(int16_t)w.control+(int16_t)w.stride; AW(control,HRP_CONTROL,w.stride);
        if(sum>=0) {
            CW(w.control,40); if((int16_t)w.control<40) {
                AW(stride,HRP_STRIDE,w.size); EL(stride,HRP_STRIDE); AL(stride,HRP_STRIDE,w.screen); AL(stride,HRP_STRIDE,w.base);
                ANDW(source,HRP_SOURCE,255); SW(source,HRP_SOURCE,32); AW(source,HRP_SOURCE,w.source); P(descriptor,HRP_DESCRIPTOR,0xc3d790u);
                P(descriptor,HRP_DESCRIPTOR,w.descriptor+(gaddr)(int32_t)rd_s16(w.descriptor+(gaddr)(int32_t)(int16_t)w.source));
                L(source,HRP_SOURCE,w.descriptor); SWAP(size,HRP_SIZE); W(control,HRP_CONTROL,w.size); SWAP(size,HRP_SIZE); ORW(control,HRP_CONTROL,w.secondary);
                observe(h,HRP_BIT_TEST,HRP_STRIDE,w.stride,0);
                if(!(w.stride&1)) { observe(h,HRP_PUSH_LONG,HRP_BASE,w.base,0); L(base,HRP_BASE,w.stride); w=consume(h,HRP_CALL_C327EE); observe(h,HRP_POP_LONG,HRP_BASE,0,0); w=restored(h); }
                else { word(h,0xc4599eu,70); w=consume(h,HRP_CALL_C32804); }
            }
        }
        w.value=low_word(w.value,(uint16_t)(w.value-1)); observe(h,HRP_DECREMENT,HRP_VALUE,w.value,0);
    } while((uint16_t)w.value!=0xffff);
}
static void draw_digits(HudReadoutParentState w,const HudReadoutParentHooks *h,int in_view) {
    SWAP(size,HRP_SIZE); W(size,HRP_SIZE,in_view?rd_u16(0xc45986u):0); L(offset,HRP_OFFSET,in_view?rd_u32(0xc45918u):0); W(control,HRP_CONTROL,w.value); L(source,HRP_SOURCE,0); L(secondary,HRP_SECONDARY,rd_u32(0xc45b22u));
    do {
        W(base,HRP_BASE,w.secondary); ANDW(base,HRP_BASE,15); AW(base,HRP_BASE,48); CW(w.base,57); if((int16_t)w.base>57) AW(base,HRP_BASE,7);
        P(registers,HRP_REGISTERS,w.registers-1); byte(h,w.registers,(uint8_t)w.base); w.secondary>>=4; observe(h,HRP_LSR_LONG,HRP_SECONDARY,4,0);
        w.control=low_word(w.control,(uint16_t)(w.control-1)); observe(h,HRP_DECREMENT,HRP_CONTROL,w.control,0);
    } while((uint16_t)w.control!=0xffff);
    TB(w.source);
    if(!(uint8_t)w.source) {
        W(control,HRP_CONTROL,w.value); SW(control,HRP_CONTROL,1);
        do {
            uint8_t digit=rd_u8(w.registers); P(registers,HRP_REGISTERS,w.registers+1); CB(digit,48); if(digit!=48) break;
            byte(h,w.registers-1,32); w.control=low_word(w.control,(uint16_t)(w.control-1)); observe(h,HRP_DECREMENT,HRP_CONTROL,w.control,0);
        } while((uint16_t)w.control!=0xffff);
    }
    draw_glyphs(w,h);
}

#define ASRL(f,id,n) do { w.f=(uint32_t)((int32_t)w.f>>(n)); observe(h,HRP_ASR_LONG,id,n,0); } while(0)
#define LSRB(f,id,n) do { w.f=low_byte(w.f,(uint8_t)((uint8_t)w.f>>(n))); observe(h,HRP_LSR_BYTE,id,n,0); } while(0)
#define DIVU(f,id,n) do { uint32_t old_=w.f,q_=old_/(uint16_t)(n); if((old_>>16)<(uint16_t)(n)) w.f=(old_%(uint16_t)(n))<<16|q_; observe(h,HRP_DIVU,id,n,0); } while(0)
#define DIVS(f,id,n) do { int32_t old_=(int32_t)w.f,q_=old_/(int16_t)(n); if(q_>=-32768 && q_<=32767) w.f=(uint32_t)(uint16_t)(old_%(int16_t)(n))<<16|(uint16_t)q_; observe(h,HRP_DIVS,id,n,0); } while(0)
static void saved(const HudReadoutParentHooks *h,uint16_t mask) { observe(h,HRP_SAVE_LONGS,HRP_VALUE,mask,0); }
static HudReadoutParentState restore(const HudReadoutParentHooks *h,uint16_t mask) { observe(h,HRP_RESTORE_LONGS,HRP_VALUE,mask,0); return restored(h); }
static int context_visible(const HudReadoutParentHooks *h) {
    TB(rd_u8(0xc457d9u)); if(!rd_u8(0xc457d9u)) return 0;
    return bit(h,0xc458cdu,6);
}
static void record_table(HudReadoutParentState *state,const HudReadoutParentHooks *h) {
    HudReadoutParentState w=*state; P(table,HRP_TABLE,0xc46184u); P(table,HRP_TABLE,w.table+(gaddr)(int32_t)rd_s16(0xc458deu)); *state=w;
}
void hud_readout_C31F4C(HudReadoutParentState w,const HudReadoutParentHooks *h) {
    record_table(&w,h); L(value,HRP_VALUE,0);
    if(!bit(h,w.table,7)) { W(value,HRP_VALUE,rd_u16(w.table+0x6e)); if((int16_t)w.value<0) NEG(value,HRP_VALUE); }
    TB(rd_u8(0xc45785u));
    if(rd_u8(0xc45785u)) { if(!context_visible(h)) return; }
    else { P(table,HRP_TABLE,0xc458f8u); w=consume(h,HRP_CALL_C31F90); if(w.less) return; }
    EL(value,HRP_VALUE); DIVU(value,HRP_VALUE,12); EL(value,HRP_VALUE); longword(h,0xc45b1eu,w.value); w=consume(h,HRP_CALL_C31FA8);
    L(value,HRP_VALUE,3); P(stream,HRP_STREAM,0xc457ffu); P(registers,HRP_REGISTERS,w.stream+4);
    TB(rd_u8(0xc45785u));
    if(rd_u8(0xc45785u)) { P(table,HRP_TABLE,0xc31958u); P(screen,HRP_SCREEN,0x1cd2); P(modulo,HRP_MODULO,18); }
    else { P(table,HRP_TABLE,0xc31948u); P(screen,HRP_SCREEN,0x1a0e); P(modulo,HRP_MODULO,30); }
    W(size,HRP_SIZE,0xfca); TB(rd_u8(0xc45785u));
    if(!rd_u8(0xc45785u)) { W(stride,HRP_STRIDE,4); draw_digits(w,h,1); return; }
    W(stride,HRP_STRIDE,0); AW(value,HRP_VALUE,3); byte(h,w.stream+4,'K'); byte(h,w.stream+5,'T'); byte(h,w.stream+6,'S');
    saved(h,0x80ec); w=consume(h,HRP_CALL_C3200A); w=restore(h,0x3701); W(size,HRP_SIZE,0xf3a); W(stride,HRP_STRIDE,12); draw_digits(w,h,0);
}
void hud_readout_C3201A(HudReadoutParentState w,const HudReadoutParentHooks *h) {
    TB(rd_u8(0xc457a4u));
    if(rd_u8(0xc457a4u)) { L(value,HRP_VALUE,rd_u32(0xc4565cu)); observe(h,HRP_COMPARE_LONG,HRP_VALUE,w.value,99999); if((int32_t)w.value>99999) L(value,HRP_VALUE,99999); }
    else {
        P(registers,HRP_REGISTERS,0xc46184u); P(registers,HRP_REGISTERS,w.registers+(gaddr)(int32_t)rd_s16(0xc458deu));
        L(value,HRP_VALUE,rd_u32(w.registers+0x18)); ASRL(value,HRP_VALUE,7); ASRL(value,HRP_VALUE,3); L(base,HRP_BASE,w.value); AL(value,HRP_VALUE,w.value); AL(value,HRP_VALUE,w.value); AL(value,HRP_VALUE,w.base);
    }
    TB(rd_u8(0xc45785u));
    if(rd_u8(0xc45785u)) { if(!context_visible(h)) return; }
    else {
        TB(rd_u8(0xc45837u)); if(rd_s8(0xc45837u)<=0) {
            L(control,HRP_CONTROL,rd_u32(0xc45900u));
            if((int32_t)w.control<0) {
                uint32_t v=rd_u32(0xc45900u)&0x7fffffffu; wr_u32(0xc45900u,v); observe(h,HRP_MEMORY_LOGIC_LONG,HRP_VALUE,v,0);
                w.control&=0x7fffffffu; observe(h,HRP_AND_LONG,HRP_CONTROL,0x7fffffff,0); L(value,HRP_VALUE,w.control); goto ready;
            }
            observe(h,HRP_COMPARE_LONG,HRP_VALUE,w.value,w.control); if(w.value==w.control) return; if(bit(h,0xc458dbu,0)) return;
        }
    }
    longword(h,0xc45900u,w.value);
    { uint32_t v=rd_u32(0xc45900u)|0x80000000u; wr_u32(0xc45900u,v); observe(h,HRP_MEMORY_LOGIC_LONG,HRP_VALUE,v,0); }
ready:
    longword(h,0xc45b1eu,w.value); w=consume(h,HRP_CALL_C320C2); L(value,HRP_VALUE,5); P(table,HRP_TABLE,0xc31928u); P(stream,HRP_STREAM,0xc457feu); P(registers,HRP_REGISTERS,w.stream+6);
    TB(rd_u8(0xc45785u));
    if(rd_u8(0xc45785u)) { P(screen,HRP_SCREEN,0x1cda); P(modulo,HRP_MODULO,26); }
    else { P(screen,HRP_SCREEN,0x18ce); P(modulo,HRP_MODULO,30); }
    W(size,HRP_SIZE,0xfca); TB(rd_u8(0xc45785u));
    if(!rd_u8(0xc45785u)) { W(stride,HRP_STRIDE,4); draw_digits(w,h,1); return; }
    W(stride,HRP_STRIDE,0); AW(value,HRP_VALUE,2); byte(h,w.stream+6,'F'); byte(h,w.stream+7,'T'); saved(h,0x80ec);
    w=consume(h,HRP_CALL_C3211A); w=restore(h,0x3701); W(size,HRP_SIZE,0xf3a); W(stride,HRP_STRIDE,12); draw_digits(w,h,0);
}
void hud_readout_C3212A(HudReadoutParentState w,const HudReadoutParentHooks *h) {
    record_table(&w,h); L(value,HRP_VALUE,rd_u32(w.table+0x72)); ASRL(value,HRP_VALUE,8); P(table,HRP_TABLE,0xc458f6u); w=consume(h,HRP_CALL_C32146); if(w.less) return;
    EL(value,HRP_VALUE); longword(h,0xc45b1eu,w.value); w=consume(h,HRP_CALL_C32158); L(value,HRP_VALUE,4);
    P(table,HRP_TABLE,0xc31994u); P(stream,HRP_STREAM,0xc457fbu); P(registers,HRP_REGISTERS,w.stream+5); P(screen,HRP_SCREEN,0x1ca4); P(modulo,HRP_MODULO,12); W(stride,HRP_STRIDE,0); W(size,HRP_SIZE,0xf3a); draw_digits(w,h,1);
}
void hud_readout_C32178(HudReadoutParentState w,const HudReadoutParentHooks *h) {
    record_table(&w,h); B(value,HRP_VALUE,rd_u8(w.table+0x2b)); w.value=low_word(w.value,(uint16_t)(int16_t)(int8_t)w.value); observe(h,HRP_EXT_WORD,HRP_VALUE,0,0);
    /* ASL's signed N xor V tests the untruncated signed product. */
    { int32_t scaled=(int16_t)w.value*256; ASL(value,HRP_VALUE,8); if(scaled<0) NEG(value,HRP_VALUE); }
    EL(value,HRP_VALUE); DIVU(value,HRP_VALUE,0x133); P(table,HRP_TABLE,0xc458fcu); w=consume(h,HRP_CALL_C321A0); if(w.less) return;
    EL(value,HRP_VALUE); longword(h,0xc45b1eu,w.value); w=consume(h,HRP_CALL_C321B2); L(value,HRP_VALUE,2);
    P(table,HRP_TABLE,0xc3198cu); P(stream,HRP_STREAM,0xc457fau); P(registers,HRP_REGISTERS,w.stream+3); P(screen,HRP_SCREEN,0x1b76); P(modulo,HRP_MODULO,30); W(stride,HRP_STRIDE,4); W(size,HRP_SIZE,0xfca); draw_digits(w,h,1);
}
static void grid_readout(HudReadoutParentState w,const HudReadoutParentHooks *h,int x) {
    gaddr cache=x?0xc4595e:0xc4595c,gate=x?0xc4583a:0xc45839;
    record_table(&w,h); L(value,HRP_VALUE,rd_u32(w.table+(x?0x14:0x1c))); SL(value,HRP_VALUE,x?0x0f000000:0x10000000); ASRL(value,HRP_VALUE,8); DIVS(value,HRP_VALUE,x?0x5999:0x7000); AW(value,HRP_VALUE,x?0x4c4:0x177);
    CW(w.value,rd_u16(cache)); if((uint16_t)w.value==rd_u16(cache)) { TB(rd_u8(gate)); if(rd_s8(gate)<=0) return; decrement_byte(h,gate); }
    else { word(h,cache,(uint16_t)w.value); byte(h,gate,2); }
    TB(rd_u8(0xc457a4u)); if(!rd_u8(0xc457a4u)) { EL(value,HRP_VALUE); longword(h,0xc45b1eu,w.value); w=consume(h,x?HRP_CALL_C322BC:HRP_CALL_C3222E); }
    L(value,HRP_VALUE,3); P(table,HRP_TABLE,x?0xc31a24u:0xc31a38u); P(stream,HRP_STREAM,0xc457fau); P(registers,HRP_REGISTERS,w.stream+4);
    P(screen,HRP_SCREEN,x?0x1d84:0x1c44); P(modulo,HRP_MODULO,36); W(stride,HRP_STRIDE,4); saved(h,0x80ec); w=consume(h,x?HRP_CALL_C322E0:HRP_CALL_C32252); w=restore(h,0x3701); W(stride,HRP_STRIDE,12); W(size,HRP_SIZE,0xf3a); draw_digits(w,h,1);
}
void hud_readout_C321D2(HudReadoutParentState w,const HudReadoutParentHooks *h) { grid_readout(w,h,0); }
void hud_readout_C32260(HudReadoutParentState w,const HudReadoutParentHooks *h) { grid_readout(w,h,1); }
void hud_readout_C31EB6(HudReadoutParentState w,const HudReadoutParentHooks *h) {
    TB(rd_u8(0xc45785u)); if(!rd_u8(0xc45785u)) return; if(!context_visible(h)) return;
    P(registers,HRP_REGISTERS,0xc46184u); W(base,HRP_BASE,rd_u16(0xc458deu)); W(value,HRP_VALUE,rd_u16(w.registers+0x68+(gaddr)(int32_t)(int16_t)w.base));
    EL(value,HRP_VALUE); ASRL(value,HRP_VALUE,3); DIVU(value,HRP_VALUE,10); word(h,0xc459a0u,(uint16_t)w.value); EL(value,HRP_VALUE); longword(h,0xc45b1eu,w.value); w=consume(h,HRP_CALL_C31EFE);
    P(table,HRP_TABLE,0xc31974u); P(stream,HRP_STREAM,0xc457fau); P(registers,HRP_REGISTERS,w.stream+6); byte(h,w.stream,'H'); byte(h,w.stream+1,'D'); byte(h,w.stream+2,'G');
    L(value,HRP_VALUE,5); L(control,HRP_CONTROL,2); P(screen,HRP_SCREEN,0x1cca); P(modulo,HRP_MODULO,10); W(size,HRP_SIZE,0xfca); W(stride,HRP_STRIDE,0);
    saved(h,0xa0ec); w=consume(h,HRP_CALL_C31F38); w=restore(h,0x3705); W(size,HRP_SIZE,0xf3a); W(stride,HRP_STRIDE,12); (void)consume(h,HRP_CALL_C31F48);
}
/* The weapon count selects the original packed label and digit field. */
static HudReadoutParentState weapon_label(HudReadoutParentState w,const HudReadoutParentHooks *h) {
    CB(w.control,0x10);
    if((uint8_t)w.control==0x10) {
        W(value,HRP_VALUE,rd_u16(w.table+0x60)); byte(h,w.stream,'G'); byte(h,w.stream+1,'U'); byte(h,w.stream+2,'N'); P(registers,HRP_REGISTERS,w.stream+7); L(size,HRP_SIZE,6); L(offset,HRP_OFFSET,2);
    } else {
        CB(w.control,0x30);
        byte(h,w.stream,' '); byte(h,w.stream+1,(uint8_t)w.control==0x30?'A':'S'); byte(h,w.stream+2,(uint8_t)w.control==0x30?'M':'W');
        if((uint8_t)w.control==0x30) ANDB(value,HRP_VALUE,15); else LSRB(value,HRP_VALUE,4);
        P(registers,HRP_REGISTERS,w.stream+5); L(size,HRP_SIZE,4); L(offset,HRP_OFFSET,0);
    }
    return w;
}
void hud_readout_C31C60(HudReadoutParentState w,const HudReadoutParentHooks *h) {
    word(h,0xc45954u,13); record_table(&w,h); P(stream,HRP_STREAM,0xc457fau); L(value,HRP_VALUE,0); B(control,HRP_CONTROL,rd_u8(w.table+0x63)); ANDB(control,HRP_CONTROL,0xf0); if(!(uint8_t)w.control) return;
    /* The high-nibble branches read stores only for the non-gun labels. */
    CB(w.control,0x10); if((uint8_t)w.control!=0x10) B(value,HRP_VALUE,rd_u8(w.table+0x5f));
    w=weapon_label(w,h); byte(h,w.stream+3,' '); longword(h,0xc45b1eu,w.value); w=consume(h,HRP_CALL_C31CFE);
    P(table,HRP_TABLE,0xc31998u); W(control,HRP_CONTROL,w.offset); P(screen,HRP_SCREEN,0x1432); W(value,HRP_VALUE,10); SWAP(value,HRP_VALUE); W(value,HRP_VALUE,w.size); (void)consume(h,HRP_CALL_C31D14);
}
void hud_readout_C31D16(HudReadoutParentState w,const HudReadoutParentHooks *h) {
    word(h,0xc45954u,13); P(stream,HRP_STREAM,0xc457fau); TW(w.value);
    if((int16_t)w.value<0) { NEG(value,HRP_VALUE); byte(h,w.stream,' '); } else byte(h,w.stream,'-');
    P(registers,HRP_REGISTERS,w.stream+5); L(size,HRP_SIZE,4); L(offset,HRP_OFFSET,3); EL(value,HRP_VALUE); longword(h,0xc45b1eu,w.value); w=consume(h,HRP_CALL_C31D4A);
    P(table,HRP_TABLE,0xc31998u); W(control,HRP_CONTROL,w.offset); P(screen,HRP_SCREEN,0x134e); W(value,HRP_VALUE,22); SWAP(value,HRP_VALUE); W(value,HRP_VALUE,w.size); L(source,HRP_SOURCE,1); (void)consume(h,HRP_CALL_C31D62);
}
void hud_readout_C31E6C(HudReadoutParentState w,const HudReadoutParentHooks *h) {
    record_table(&w,h); B(value,HRP_VALUE,rd_u8(w.table+0x63)); ANDB(value,HRP_VALUE,0xf0); CB(w.value,0x10);
    P(stream,HRP_STREAM,(uint8_t)w.value==0x10?0xc31e65u:0xc31e5fu); P(registers,HRP_REGISTERS,w.stream+6); P(table,HRP_TABLE,0xc31998u); P(screen,HRP_SCREEN,0x1440);
    W(value,HRP_VALUE,24); SWAP(value,HRP_VALUE); W(value,HRP_VALUE,5); word(h,0xc45954u,13); (void)consume(h,HRP_CALL_C31EB2);
}
void hud_readout_C31D64(HudReadoutParentState w,const HudReadoutParentHooks *h) {
    int32_t sum;
    P(screen,HRP_SCREEN,0xc46184u); P(screen,HRP_SCREEN,w.screen+(gaddr)(int32_t)rd_s16(0xc458deu)); word(h,0xc45954u,13); longword(h,0xc456e6u,0xfffff); CB(rd_u8(w.screen+0x62),0x11);
    if(rd_u8(w.screen+0x62)!=0x11) {
        W(value,HRP_VALUE,114); W(base,HRP_BASE,65); w=consume(h,HRP_CALL_C31D98); P(screen,HRP_SCREEN,0x994); W(control,HRP_CONTROL,12); P(table,HRP_TABLE,0xc31900u);
    } else {
        P(stream,HRP_STREAM,0xc31e5eu); P(registers,HRP_REGISTERS,w.stream+1); P(table,HRP_TABLE,0xc31918u); P(screen,HRP_SCREEN,0x12f2); W(value,HRP_VALUE,10); SWAP(value,HRP_VALUE); W(value,HRP_VALUE,0); w=consume(h,HRP_CALL_C31DC8);
        W(value,HRP_VALUE,100); W(base,HRP_BASE,125); w=consume(h,HRP_CALL_C31DD6); P(screen,HRP_SCREEN,0x12f2); W(control,HRP_CONTROL,10); P(table,HRP_TABLE,0xc3190cu);
    }
    P(stream,HRP_STREAM,0xc457fau); P(registers,HRP_REGISTERS,0xc46184u); P(registers,HRP_REGISTERS,w.registers+(gaddr)(int32_t)rd_s16(0xc458deu));
    W(value,HRP_VALUE,rd_u16(w.registers+0x56)); W(base,HRP_BASE,w.value); ASR(base,HRP_BASE,3); AW(value,HRP_VALUE,w.base); W(size,HRP_SIZE,rd_u16(0xc45946u));
    if((int16_t)w.size<0) NEG(size,HRP_SIZE); CW(w.size,1); if((int16_t)w.size>1) AW(value,HRP_VALUE,rd_u16(0xc45946u));
    ASR(value,HRP_VALUE,3); W(base,HRP_BASE,rd_u16(0xc458cau)); ANDW(base,HRP_BASE,2);
    if((uint16_t)w.base) { sum=(int16_t)w.value+10; AW(value,HRP_VALUE,10); } else { sum=(int16_t)w.value-10; SW(value,HRP_VALUE,10); }
    if(sum<0) { NEG(value,HRP_VALUE); byte(h,w.stream,' '); } else byte(h,w.stream,'-');
    P(registers,HRP_REGISTERS,w.stream+3); L(size,HRP_SIZE,2); L(offset,HRP_OFFSET,1); EL(value,HRP_VALUE); longword(h,0xc45b1eu,w.value); w=consume(h,HRP_CALL_C31E50);
    W(value,HRP_VALUE,w.control); W(control,HRP_CONTROL,w.offset); SWAP(value,HRP_VALUE); W(value,HRP_VALUE,w.size); (void)consume(h,HRP_CALL_C31E5C);
}
void hud_readout_C33F54(HudReadoutParentState w,const HudReadoutParentHooks *h) {
    L(source,HRP_SOURCE,1); W(value,HRP_VALUE,16); L(stride,HRP_STRIDE,0x858); L(size,HRP_SIZE,1); L(offset,HRP_OFFSET,2); P(stream,HRP_STREAM,0xc457fau); P(registers,HRP_REGISTERS,w.stream+3);
    W(control,HRP_CONTROL,w.offset); P(screen,HRP_SCREEN,w.stride); SWAP(value,HRP_VALUE); W(value,HRP_VALUE,w.size); (void)consume(h,HRP_CALL_C33FB0);
}
static HudReadoutParentState status_line(HudReadoutParentState w,const HudReadoutParentHooks *h,gaddr rows) {
    W(control,HRP_CONTROL,w.offset); P(screen,HRP_SCREEN,rows); P(modulo,HRP_MODULO,6); W(stride,HRP_STRIDE,4); W(size,HRP_SIZE,0xfca); SWAP(size,HRP_SIZE);
    W(size,HRP_SIZE,rd_u16(0xc45986u)); L(offset,HRP_OFFSET,rd_u32(0xc45918u)); return w;
}
void hud_readout_C328A8(HudReadoutParentState w,const HudReadoutParentHooks *h) {
    TB(rd_u8(0xc45844u)); if(rd_s8(0xc45844u)<=0) return; decrement_byte(h,0xc45844u); record_table(&w,h); L(value,HRP_VALUE,0); B(value,HRP_VALUE,rd_u8(w.table+0x5f)); P(stream,HRP_STREAM,0xc457fau);
    B(control,HRP_CONTROL,rd_u8(w.table+0x63)); ANDB(control,HRP_CONTROL,0xf0);
    if((uint8_t)w.control) {
        w=weapon_label(w,h); P(table,HRP_TABLE,0xc3191cu); byte(h,w.stream+3,' '); byte(h,w.stream+5,' '); byte(h,w.stream+6,' ');
        W(control,HRP_CONTROL,w.offset); P(screen,HRP_SCREEN,0x1cc6); P(modulo,HRP_MODULO,6); W(size,HRP_SIZE,0xfca); W(stride,HRP_STRIDE,4); observe(h,HRP_PUSH_WORD,HRP_VALUE,w.value,0); L(value,HRP_VALUE,2); SWAP(size,HRP_SIZE); W(size,HRP_SIZE,rd_u16(0xc45986u)); L(offset,HRP_OFFSET,rd_u32(0xc45918u)); w=consume(h,HRP_CALL_C32982);
        observe(h,HRP_POP_WORD,HRP_VALUE,0,0); w=restored(h); P(stream,HRP_STREAM,0xc457fau); TW(w.value);
        byte(h,w.stream,(int16_t)w.value>0?'A':' '); byte(h,w.stream+1,(int16_t)w.value>0?'R':'N'); byte(h,w.stream+2,(int16_t)w.value>0?'M':'O');
        P(table,HRP_TABLE,0xc3191cu); w=status_line(w,h,0x1b86); L(value,HRP_VALUE,2); draw_glyphs(w,h);
    } else {
        byte(h,w.stream,' '); byte(h,w.stream+1,' '); byte(h,w.stream+2,' '); P(table,HRP_TABLE,0xc3191cu);
        w=status_line(w,h,0x1cc6); L(value,HRP_VALUE,2); w=consume(h,HRP_CALL_C32A14); P(stream,HRP_STREAM,0xc457fau); P(table,HRP_TABLE,0xc3191cu);
        /* In the empty-kind second line, MOVEQ precedes the mode/global loads. */
        W(control,HRP_CONTROL,w.offset); P(screen,HRP_SCREEN,0x1b86); P(modulo,HRP_MODULO,6); W(stride,HRP_STRIDE,4); L(value,HRP_VALUE,2); W(size,HRP_SIZE,0xfca); SWAP(size,HRP_SIZE); W(size,HRP_SIZE,rd_u16(0xc45986u)); L(offset,HRP_OFFSET,rd_u32(0xc45918u)); draw_glyphs(w,h);
    }
}
