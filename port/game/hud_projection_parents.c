/* Complete original C0DAEE/C0CFFA/C33CD2/C33B38/C332BC/C32662 owners. */
#include "hud_projection_parents.h"
#include <stdlib.h>
static void observe(const HudProjectionHooks *h,enum HudProjectionPhase p,enum HudProjectionField f,uint32_t v,uint32_t o) {
    if(h && h->observe) h->observe(h->context,p,f,v,o);
}
static HudProjectionState consume(const HudProjectionHooks *h,enum HudProjectionChild child) {
    if(h && h->consume) return h->consume(h->context,child); abort();
}
static HudProjectionState restored(const HudProjectionHooks *h) {
    if(h && h->restored) return h->restored(h->context); abort();
}
static uint32_t low_word(uint32_t old,uint16_t v) { return (old&0xffff0000u)|v; }
static uint32_t low_byte(uint32_t old,uint8_t v) { return (old&0xffffff00u)|v; }
#define W(f,id,v) do { w.f=low_word(w.f,(uint16_t)(v)); observe(h,HPR_WORD,id,w.f,0); } while(0)
#define B(f,id,v) do { w.f=low_byte(w.f,(uint8_t)(v)); observe(h,HPR_BYTE,id,w.f,0); } while(0)
#define L(f,id,v) do { w.f=(uint32_t)(v); observe(h,HPR_LONG,id,w.f,0); } while(0)
#define P(f,id,v) do { w.f=(gaddr)(v); observe(h,HPR_POINTER,id,w.f,0); } while(0)
#define AW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f+n_)); observe(h,HPR_ADD_WORD,id,n_,0); } while(0)
#define SW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f-n_)); observe(h,HPR_SUB_WORD,id,n_,0); } while(0)
#define AL(f,id,v) do { uint32_t n_=(uint32_t)(v); w.f+=n_; observe(h,HPR_ADD_LONG,id,n_,0); } while(0)
#define SL(f,id,v) do { uint32_t n_=(uint32_t)(v); w.f-=n_; observe(h,HPR_SUB_LONG,id,n_,0); } while(0)
#define ANDW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f&n_)); observe(h,HPR_AND_WORD,id,n_,0); } while(0)
#define ANDB(f,id,v) do { uint8_t n_=(uint8_t)(v); w.f=low_byte(w.f,(uint8_t)(w.f&n_)); observe(h,HPR_AND_BYTE,id,n_,0); } while(0)
#define ORW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f|n_)); observe(h,HPR_OR_WORD,id,n_,0); } while(0)
#define EL(f,id) do { w.f=(uint32_t)(int32_t)(int16_t)w.f; observe(h,HPR_EXT_LONG,id,0,0); } while(0)
#define NEG(f,id) do { w.f=low_word(w.f,(uint16_t)(0u-w.f)); observe(h,HPR_NEG_WORD,id,0,0); } while(0)
#define SWAP(f,id) do { w.f=(w.f<<16)|(w.f>>16); observe(h,HPR_SWAP,id,0,0); } while(0)
#define ASL(f,id,n) do { w.f=low_word(w.f,(uint16_t)((((unsigned)(n)&63u)<16?w.f<<((unsigned)(n)&63u):0))); observe(h,HPR_ASL_WORD,id,n,0); } while(0)
#define LSL(f,id,n) do { w.f=low_word(w.f,(uint16_t)((((unsigned)(n)&63u)<16?w.f<<((unsigned)(n)&63u):0))); observe(h,HPR_LSL_WORD,id,n,0); } while(0)
#define ASR(f,id,n) do { w.f=low_word(w.f,(uint16_t)((int16_t)w.f>>(n))); observe(h,HPR_ASR_WORD,id,n,0); } while(0)
#define CW(v,n) observe(h,HPR_COMPARE_WORD,HPR_VALUE,(uint16_t)(v),(uint16_t)(n))
#define CB(v,n) observe(h,HPR_COMPARE_BYTE,HPR_VALUE,(uint8_t)(v),(uint8_t)(n))
#define TW(v) observe(h,HPR_TEST_WORD,HPR_VALUE,(uint16_t)(v),0)
#define TB(v) observe(h,HPR_TEST_BYTE,HPR_VALUE,(uint8_t)(v),0)
static void word(const HudProjectionHooks *h,gaddr a,uint16_t v) { wr_u16(a,v); observe(h,HPR_STORE_WORD,HPR_VALUE,v,0); }
static void byte(const HudProjectionHooks *h,gaddr a,uint8_t v) { wr_u8(a,v); observe(h,HPR_STORE_BYTE,HPR_VALUE,v,0); }
static void longword(const HudProjectionHooks *h,gaddr a,uint32_t v) { wr_u32(a,v); observe(h,HPR_STORE_LONG,HPR_VALUE,v,0); }
static void decrement_byte(const HudProjectionHooks *h,gaddr a) { uint8_t old=rd_u8(a); wr_u8(a,(uint8_t)(old-1)); observe(h,HPR_MEMORY_SUB_BYTE,HPR_VALUE,old,1); }
static int bit(const HudProjectionHooks *h,gaddr a,unsigned b) { uint8_t v=rd_u8(a); observe(h,HPR_BIT_TEST,HPR_VALUE,v,b); return (v>>b)&1; }

/* MOVEM transfers retain CCR; individual MOVE transfers above update it. */
#define RAW(f,id,v) do { w.f=(uint32_t)(v); observe(h,HPR_RAW_LONG,id,w.f,0); } while(0)
#define MUL(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=(uint32_t)((int32_t)(int16_t)w.f*(int16_t)n_); observe(h,HPR_MULS,id,n_,0); } while(0)
#define MUP(f,id,ptr,pi) do { MUL(f,id,rd_u16(w.ptr)); P(ptr,pi,w.ptr+2); } while(0)
#define ARL(f,id,n) do { unsigned n_=(unsigned)(n)&63u; w.f=(uint32_t)((int32_t)w.f>>(n_<32?n_:31)); observe(h,HPR_ASR_LONG,id,n_,0); } while(0)
#define DU(f,id,n) do { uint16_t n_=(uint16_t)(n); if(n_ && w.f/n_<=65535u) w.f=(w.f%n_)<<16|(w.f/n_); observe(h,HPR_DIVU,id,n_,0); } while(0)

/* C0DAEE: fixed view tuple multiplied by the original 2.8 matrix. */
void hud_fixed_mark(HudProjectionState w,const HudProjectionHooks *h) {
    W(control,HPR_CONTROL,0xe000); W(secondary,HPR_SECONDARY,0x3800); W(source,HPR_SOURCE,0xe000); P(registers,HPR_REGISTERS,0xc45bd8u);
    W(stride,HPR_STRIDE,w.control); W(size,HPR_SIZE,w.secondary); W(value,HPR_VALUE,w.source);
    MUP(stride,HPR_STRIDE,registers,HPR_REGISTERS); MUP(size,HPR_SIZE,registers,HPR_REGISTERS); MUP(value,HPR_VALUE,registers,HPR_REGISTERS);
    AL(value,HPR_VALUE,w.size); AL(value,HPR_VALUE,w.stride); ARL(value,HPR_VALUE,8);
    W(stride,HPR_STRIDE,w.control); W(size,HPR_SIZE,w.secondary); W(base,HPR_BASE,w.source);
    MUP(stride,HPR_STRIDE,registers,HPR_REGISTERS); MUP(size,HPR_SIZE,registers,HPR_REGISTERS); MUP(base,HPR_BASE,registers,HPR_REGISTERS);
    AL(base,HPR_BASE,w.size); AL(base,HPR_BASE,w.stride); ARL(base,HPR_BASE,8);
    MUP(control,HPR_CONTROL,registers,HPR_REGISTERS); MUP(secondary,HPR_SECONDARY,registers,HPR_REGISTERS); MUP(source,HPR_SOURCE,registers,HPR_REGISTERS);
    AL(control,HPR_CONTROL,w.secondary); AL(control,HPR_CONTROL,w.source); ARL(control,HPR_CONTROL,8);
    L(size,HPR_SIZE,8); word(h,0xc45954u,9); (void)consume(h,HPR_FIXED_CIRCLE);
}

/* C0CFFA: source word shifts and stack MOVEM sign-extension are observable. */
void hud_scaled_circle(HudProjectionState w,const HudProjectionHooks *h) {
    RAW(secondary,HPR_SECONDARY,(int32_t)rd_s16(w.descriptor)); RAW(source,HPR_SOURCE,(int32_t)rd_s16(w.descriptor+2)); RAW(stride,HPR_STRIDE,(int32_t)rd_s16(w.descriptor+4));
    W(value,HPR_VALUE,rd_u16(w.frame-8)); ASL(secondary,HPR_SECONDARY,w.value); ASL(source,HPR_SOURCE,w.value); ASL(stride,HPR_STRIDE,w.value);
    observe(h,HPR_SAVE_WORDS,HPR_VALUE,0x1e00,0);
    W(value,HPR_VALUE,w.secondary); if((int16_t)w.value<0) NEG(value,HPR_VALUE);
    W(base,HPR_BASE,w.source); if((int16_t)w.base<0) NEG(base,HPR_BASE);
    W(control,HPR_CONTROL,w.stride); if((int16_t)w.control<0) NEG(control,HPR_CONTROL);
    W(source,HPR_SOURCE,w.control); W(secondary,HPR_SECONDARY,w.base); W(control,HPR_CONTROL,w.value);
    w=consume(h,HPR_MAGNITUDE); W(offset,HPR_OFFSET,w.base); observe(h,HPR_RESTORE_WORDS,HPR_VALUE,0x47,0); w=restored(h);
    TW(w.offset);
    if((int16_t)w.offset>0) { DU(size,HPR_SIZE,w.offset); CW(w.size,127); if((int16_t)w.size<=127) goto project; }
    W(size,HPR_SIZE,127);
project:
    (void)consume(h,HPR_SCALED_CIRCLE);
}

/* C33CD2: publish the point returned by projection and advance saved positions. */
void hud_record_transform(HudProjectionState w,const HudProjectionHooks *h) {
    P(table,HPR_TABLE,0xc46184u); P(table,HPR_TABLE,w.table+(gaddr)(int32_t)rd_s16(0xc458deu));
    RAW(control,HPR_CONTROL,rd_u32(0xc45722u)); RAW(secondary,HPR_SECONDARY,rd_u32(0xc45726u)); RAW(source,HPR_SOURCE,rd_u32(0xc4572au));
    SL(control,HPR_CONTROL,rd_u32(w.table+0x14)); SL(secondary,HPR_SECONDARY,rd_u32(w.table+0x18)); SL(source,HPR_SOURCE,rd_u32(w.table+0x1c));
    ARL(control,HPR_CONTROL,8); ARL(secondary,HPR_SECONDARY,8); ARL(source,HPR_SOURCE,8); P(stream,HPR_STREAM,0xc45bd8u);
    W(stride,HPR_STRIDE,w.control); W(size,HPR_SIZE,w.secondary); W(offset,HPR_OFFSET,w.source);
    MUP(stride,HPR_STRIDE,stream,HPR_STREAM); MUP(size,HPR_SIZE,stream,HPR_STREAM); MUP(offset,HPR_OFFSET,stream,HPR_STREAM);
    AL(offset,HPR_OFFSET,w.size); AL(offset,HPR_OFFSET,w.stride); ARL(offset,HPR_OFFSET,8); W(value,HPR_VALUE,w.offset);
    W(stride,HPR_STRIDE,w.control); W(size,HPR_SIZE,w.secondary); W(offset,HPR_OFFSET,w.source);
    MUP(stride,HPR_STRIDE,stream,HPR_STREAM); MUP(size,HPR_SIZE,stream,HPR_STREAM); MUP(offset,HPR_OFFSET,stream,HPR_STREAM);
    AL(offset,HPR_OFFSET,w.size); AL(offset,HPR_OFFSET,w.stride); ARL(offset,HPR_OFFSET,8);
    MUP(control,HPR_CONTROL,stream,HPR_STREAM); MUP(secondary,HPR_SECONDARY,stream,HPR_STREAM); MUP(source,HPR_SOURCE,stream,HPR_STREAM);
    AL(source,HPR_SOURCE,w.secondary); AL(source,HPR_SOURCE,w.control); ARL(source,HPR_SOURCE,8); W(base,HPR_BASE,w.offset); W(control,HPR_CONTROL,w.source);
    w=consume(h,HPR_RECORD_POINT); wr_u16(0xc4593eu,(uint16_t)w.value); wr_u16(0xc45940u,(uint16_t)w.base);
    W(source,HPR_SOURCE,0x600); W(stride,HPR_STRIDE,0x4000); P(stream,HPR_STREAM,w.table); P(stream,HPR_STREAM,w.stream+0x92);
    W(value,HPR_VALUE,w.source); W(offset,HPR_OFFSET,w.stride); MUL(value,HPR_VALUE,rd_u16(w.stream+2)); MUL(offset,HPR_OFFSET,rd_u16(w.stream+4)); AL(value,HPR_VALUE,w.offset);
    W(base,HPR_BASE,w.source); W(offset,HPR_OFFSET,w.stride); MUL(base,HPR_BASE,rd_u16(w.stream+8)); MUL(offset,HPR_OFFSET,rd_u16(w.stream+10)); AL(base,HPR_BASE,w.offset);
    W(control,HPR_CONTROL,w.source); MUL(control,HPR_CONTROL,rd_u16(w.stream+14)); MUL(stride,HPR_STRIDE,rd_u16(w.stream+16)); AL(control,HPR_CONTROL,w.stride);
    ARL(value,HPR_VALUE,6); ARL(base,HPR_BASE,6); ARL(control,HPR_CONTROL,6);
    AL(value,HPR_VALUE,rd_u32(w.table+0x14)); AL(base,HPR_BASE,rd_u32(w.table+0x18)); AL(control,HPR_CONTROL,rd_u32(w.table+0x1c));
    RAW(secondary,HPR_SECONDARY,rd_u32(0xc45716u)); RAW(source,HPR_SOURCE,rd_u32(0xc4571au)); RAW(stride,HPR_STRIDE,rd_u32(0xc4571eu));
    wr_u32(0xc45722u,w.secondary); wr_u32(0xc45726u,w.source); wr_u32(0xc4572au,w.stride);
    wr_u32(0xc45716u,w.value); wr_u32(0xc4571au,w.base); wr_u32(0xc4571eu,w.control);
}

static void load_marker(HudProjectionState *p,const HudProjectionHooks *h) {
    HudProjectionState w=*p; RAW(value,HPR_VALUE,(int32_t)rd_s16(0xc4593eu)); RAW(base,HPR_BASE,(int32_t)rd_s16(0xc45940u)); *p=w;
}
static void event_bit(const HudProjectionHooks *h,uint32_t mask) { uint32_t v=rd_u32(0xc45b54u)|mask; wr_u32(0xc45b54u,v); observe(h,HPR_MEMORY_LOGIC_LONG,HPR_VALUE,v,0); }

/* C33B38: source gun-cue transitions, marker/range rendering and closure rate. */
void hud_weapon_cue(HudProjectionState w,const HudProjectionHooks *h) {
    int32_t difference; uint8_t old;
    CW(rd_u16(0xc45a42u),128); if(rd_u16(0xc45a42u)!=128) return;
    P(table,HPR_TABLE,0xc46184u); P(table,HPR_TABLE,w.table+(gaddr)(int32_t)rd_s16(0xc458deu)); B(stride,HPR_STRIDE,rd_u8(w.table+0x63)); ANDB(stride,HPR_STRIDE,0xf0); CB(w.stride,0x10);
    if((uint8_t)w.stride!=0x10) {
        TW(rd_u16(0xc459c0u)); if(rd_s16(0xc459c0u)<0) goto finish;
        W(value,HPR_VALUE,159); W(base,HPR_BASE,91); TW(rd_u16(0xc4593au));
        if(rd_s16(0xc4593au)>0) { TW(rd_u16(0xc459c0u)); if(rd_s16(0xc459c0u)>=0) goto display; }
        wr_u16(0xc4593eu,(uint16_t)w.value); wr_u16(0xc45940u,(uint16_t)w.base); goto display;
    }
    W(value,HPR_VALUE,rd_u16(0xc4593au)); if((int16_t)w.value<=0) goto clear_cue;
    difference=(int16_t)w.value-rd_s16(0xc4593eu); SW(value,HPR_VALUE,rd_u16(0xc4593eu)); if(difference<0) NEG(value,HPR_VALUE);
    CW(w.value,8); if((int16_t)w.value>8) goto clear_cue;
    W(base,HPR_BASE,rd_u16(0xc4593cu)); difference=(int16_t)w.base-rd_s16(0xc45940u); SW(base,HPR_BASE,rd_u16(0xc45940u)); if(difference<0) NEG(base,HPR_BASE);
    CW(w.base,8); if((int16_t)w.base>8) goto clear_cue;
    CW(rd_u16(w.table+0x4a),0x900); if(rd_s16(w.table+0x4a)>0x900) goto clear_cue;
    TB(rd_u8(0xc458b4u)); if(rd_u8(0xc458b4u)) goto shoot;
    TW(rd_u16(0xc459c0u)); if(rd_s16(0xc459c0u)<0) goto clear_cue;
    event_bit(h,4); byte(h,0xc458b4u,1);
shoot:
    w=consume(h,HPR_SHOOT); goto point;
clear_cue:
    TB(rd_u8(0xc458b4u)); if(rd_u8(0xc458b4u)) { byte(h,0xc458b4u,0); event_bit(h,8); }
point:
    load_marker(&w,h); TW(w.value); if((int16_t)w.value<=0) goto finish;
    w=consume(h,HPR_POINT);
display:
    load_marker(&w,h); TW(w.value); if((int16_t)w.value<=0) goto finish;
    AW(base,HPR_BASE,rd_u16(0xc458d8u)); W(control,HPR_CONTROL,0x50); P(registers,HPR_REGISTERS,0xc3494cu); word(h,0xc45954u,10); w=consume(h,HPR_RING);
    TW(rd_u16(0xc459c0u)); if(rd_s16(0xc459c0u)<0) goto finish;
    load_marker(&w,h); AW(base,HPR_BASE,rd_u16(0xc458d8u)); P(registers,HPR_REGISTERS,0xc46184u); P(registers,HPR_REGISTERS,w.registers+(gaddr)(int32_t)rd_s16(0xc458deu)); W(control,HPR_CONTROL,rd_u16(w.registers+0x4a));
    observe(h,HPR_PUSH_LONG,HPR_VALUE,w.registers,0); w.control=(uint16_t)w.control*76u; observe(h,HPR_MULU,HPR_CONTROL,76,0); DU(control,HPR_CONTROL,0x8ca0);
    P(registers,HPR_REGISTERS,0xc34976u); word(h,0xc45954u,13); w=consume(h,HPR_RANGE); observe(h,HPR_POP_POINTER,HPR_REGISTERS,0,0); w=restored(h);
    W(value,HPR_VALUE,rd_u16(w.registers+0x4a)); CW(w.value,0x7f00); if((int16_t)w.value>0x7f00) goto finish;
    TB(rd_u8(0xc457aeu)); if(rd_u8(0xc457aeu)) goto cached_rate;
    old=rd_u8(w.registers+4); wr_u8(w.registers+4,old&0xfeu); observe(h,HPR_BIT_TEST,HPR_VALUE,old,0); if(!(old&1)) goto cached_rate;
    W(base,HPR_BASE,w.value); SW(value,HPR_VALUE,rd_u16(0xc45b44u)); word(h,0xc45b46u,(uint16_t)w.value); word(h,0xc45b44u,(uint16_t)w.base); goto rate;
cached_rate:
    W(value,HPR_VALUE,rd_u16(0xc45b46u));
rate:
    w=consume(h,HPR_RATE);
finish:
    TB(rd_u8(0xc457aeu)); if(!rd_u8(0xc457aeu)) (void)consume(h,HPR_TRANSFORM);
}

/* C332BC: each returned child state and its changed RAM drives the next call. */
void hud_postflight_outer(HudProjectionState w,const HudProjectionHooks *h) {
    (void)w; longword(h,0xc456e6u,0xfffffu); TB(rd_u8(0xc45785u));
    if(rd_u8(0xc45785u)) { byte(h,0xc458b4u,0); return; }
    (void)consume(h,HPR_SETUP); (void)consume(h,HPR_CENTRE); (void)consume(h,HPR_PITCH); (void)consume(h,HPR_ROLL); (void)consume(h,HPR_STATUS);
    TB(rd_u8(0xc457a1u)); if(!rd_u8(0xc457a1u)) return;
    (void)consume(h,HPR_CLOCK); (void)consume(h,HPR_DISPLAY); (void)consume(h,HPR_CUE);
}

/* C32662 delegates only the exact source-owned C32794 glyph tail, whose CPU
 * and original child contracts are independently proven in hud_text_helpers. */
#include "hud_text_helpers.h"
typedef struct { const HudProjectionHooks *parent; } TextParent;
static HudTextState text_state(HudProjectionState w) {
    HudTextState t={w.value,w.base,w.control,w.secondary,w.source,w.stride,w.size,w.offset,w.registers,w.table,w.stream,w.descriptor,w.screen,w.modulo,w.less}; return t;
}
static HudTextState text_child(void *context,enum HudTextChild child) {
    TextParent *p=context; return text_state(consume(p->parent,child==HTH_SMALL_GLYPH?HPR_SMALL_GLYPH:HPR_FAULT));
}
static void text_phase(void *context,enum HudTextPhase phase,enum HudTextField field,uint32_t v,uint32_t operand) {
    TextParent *p=context; observe(p->parent,(enum HudProjectionPhase)phase,(enum HudProjectionField)field,v,operand);
}
static HudTextState text_restored(void *context) { TextParent *p=context; return text_state(restored(p->parent)); }
void hud_conditional_text(HudProjectionState w,const HudProjectionHooks *h) {
    TextParent p={h}; HudTextHooks text={text_child,text_phase,text_restored,&p};
    TB(rd_u8(0xc45785u)); if(rd_u8(0xc45785u)) { TB(rd_u8(0xc45793u)); if(!rd_u8(0xc45793u)) return; }
    hud_text_small_line(text_state(w),&text);
}
