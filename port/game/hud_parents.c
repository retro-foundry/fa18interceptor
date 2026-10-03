/* Original counter-gated HUD streams, status fields and shared digit tails. */
#include "hud_parents.h"
#include <stdlib.h>
static void observe(const HudParentHooks *h,enum HudParentPhase p,enum HudParentField f,uint32_t v,uint32_t o) {
    if(h && h->observe) h->observe(h->context,p,f,v,o);
}
static HudParentState consume(const HudParentHooks *h,enum HudParentChild child) {
    if(h && h->consume) return h->consume(h->context,child); abort();
}
static HudParentState restored(const HudParentHooks *h) {
    if(h && h->restored) return h->restored(h->context); abort();
}
static uint32_t low_word(uint32_t old,uint16_t v) { return (old&0xffff0000u)|v; }
static uint32_t low_byte(uint32_t old,uint8_t v) { return (old&0xffffff00u)|v; }
#define W(f,id,v) do { w.f=low_word(w.f,(uint16_t)(v)); observe(h,HP_WORD,id,w.f,0); } while(0)
#define B(f,id,v) do { w.f=low_byte(w.f,(uint8_t)(v)); observe(h,HP_BYTE,id,w.f,0); } while(0)
#define L(f,id,v) do { w.f=(uint32_t)(v); observe(h,HP_LONG,id,w.f,0); } while(0)
#define P(f,id,v) do { w.f=(gaddr)(v); observe(h,HP_POINTER,id,w.f,0); } while(0)
#define AW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f+n_)); observe(h,HP_ADD_WORD,id,n_,0); } while(0)
#define SW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f-n_)); observe(h,HP_SUB_WORD,id,n_,0); } while(0)
#define AL(f,id,v) do { uint32_t n_=(uint32_t)(v); w.f+=n_; observe(h,HP_ADD_LONG,id,n_,0); } while(0)
#define SL(f,id,v) do { uint32_t n_=(uint32_t)(v); w.f-=n_; observe(h,HP_SUB_LONG,id,n_,0); } while(0)
#define ANDW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f&n_)); observe(h,HP_AND_WORD,id,n_,0); } while(0)
#define ANDB(f,id,v) do { uint8_t n_=(uint8_t)(v); w.f=low_byte(w.f,(uint8_t)(w.f&n_)); observe(h,HP_AND_BYTE,id,n_,0); } while(0)
#define ORW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f|n_)); observe(h,HP_OR_WORD,id,n_,0); } while(0)
#define EL(f,id) do { w.f=(uint32_t)(int32_t)(int16_t)w.f; observe(h,HP_EXT_LONG,id,0,0); } while(0)
#define NEG(f,id) do { w.f=low_word(w.f,(uint16_t)(0u-w.f)); observe(h,HP_NEG_WORD,id,0,0); } while(0)
#define SWAP(f,id) do { w.f=(w.f<<16)|(w.f>>16); observe(h,HP_SWAP,id,0,0); } while(0)
#define ASL(f,id,n) do { w.f=low_word(w.f,(uint16_t)(w.f<<(n))); observe(h,HP_ASL_WORD,id,n,0); } while(0)
#define LSL(f,id,n) do { w.f=low_word(w.f,(uint16_t)(w.f<<(n))); observe(h,HP_LSL_WORD,id,n,0); } while(0)
#define ASR(f,id,n) do { w.f=low_word(w.f,(uint16_t)((int16_t)w.f>>(n))); observe(h,HP_ASR_WORD,id,n,0); } while(0)
#define CW(v,n) observe(h,HP_COMPARE_WORD,HP_VALUE,(uint16_t)(v),(uint16_t)(n))
#define CB(v,n) observe(h,HP_COMPARE_BYTE,HP_VALUE,(uint8_t)(v),(uint8_t)(n))
#define TW(v) observe(h,HP_TEST_WORD,HP_VALUE,(uint16_t)(v),0)
#define TB(v) observe(h,HP_TEST_BYTE,HP_VALUE,(uint8_t)(v),0)
static void word(const HudParentHooks *h,gaddr a,uint16_t v) { wr_u16(a,v); observe(h,HP_STORE_WORD,HP_VALUE,v,0); }
static void byte(const HudParentHooks *h,gaddr a,uint8_t v) { wr_u8(a,v); observe(h,HP_STORE_BYTE,HP_VALUE,v,0); }
static void longword(const HudParentHooks *h,gaddr a,uint32_t v) { wr_u32(a,v); observe(h,HP_STORE_LONG,HP_VALUE,v,0); }
static void decrement_byte(const HudParentHooks *h,gaddr a) { uint8_t old=rd_u8(a); wr_u8(a,(uint8_t)(old-1)); observe(h,HP_MEMORY_SUB_BYTE,HP_VALUE,old,1); }
static int bit(const HudParentHooks *h,gaddr a,unsigned b) { uint8_t v=rd_u8(a); observe(h,HP_BIT_TEST,HP_VALUE,v,b); return (v>>b)&1; }

void draw_table_stream_display(HudParentState w,const HudParentHooks *h) {
    P(table,HP_TABLE,0xc309a6u); P(descriptor,HP_DESCRIPTOR,0xc309a2u); W(control,HP_CONTROL,0x0fce);
    P(descriptor,HP_DESCRIPTOR,rd_u32(w.descriptor)); L(value,HP_VALUE,rd_u32(w.descriptor)); L(base,HP_BASE,0x14ca);
    P(screen,HP_SCREEN,18); W(size,HP_SIZE,0x312); P(modulo,HP_MODULO,5); W(offset,HP_OFFSET,1);
    (void)consume(h,HP_TABLE_RENDER);
}
void draw_counter_stream_display(HudParentState w,const HudParentHooks *h) {
    int16_t height;
    TB(rd_u8(0xc45836u)); if(rd_s8(0xc45836u)<=0) return;
    decrement_byte(h,0xc45836u); P(registers,HP_REGISTERS,0xdff000u); P(stream,HP_STREAM,rd_u32(0xc456b6u));
    P(table,HP_TABLE,0xc30752u); W(control,HP_CONTROL,0x9f0); L(base,HP_BASE,0x16a8); P(screen,HP_SCREEN,20); W(size,HP_SIZE,55);
    TW(rd_u16(0xc458d8u)); if(rd_s16(0xc458d8u)>0) SW(size,HP_SIZE,rd_u16(0xc458d8u));
    ASL(size,HP_SIZE,6); AW(size,HP_SIZE,20); P(modulo,HP_MODULO,1); W(offset,HP_OFFSET,0); AL(base,HP_BASE,rd_u32(0xc45918u));
    w=consume(h,HP_COUNTER_CLIP);
    if(!w.less) {
        AW(stride,HP_STRIDE,w.offset); AW(offset,HP_OFFSET,w.offset); EL(offset,HP_OFFSET);
        L(source,HP_SOURCE,rd_u32(w.stream)); P(stream,HP_STREAM,w.stream+4); AL(source,HP_SOURCE,w.base);
        SW(size,HP_SIZE,w.stride); AW(stride,HP_STRIDE,w.stride);
        P(descriptor,HP_DESCRIPTOR,rd_u32(w.table)); P(table,HP_TABLE,w.table+4); L(value,HP_VALUE,rd_u32(w.descriptor)); AL(value,HP_VALUE,w.offset);
        AW(stride,HP_STRIDE,1); w=consume(h,HP_COUNTER_WAIT);
        word(h,w.registers+0x42,0); word(h,w.registers+0x44,0xffff); word(h,w.registers+0x46,0xffff); word(h,w.registers+0x64,(uint16_t)w.stride);
        SW(stride,HP_STRIDE,1); AW(stride,HP_STRIDE,w.modulo); word(h,w.registers+0x66,(uint16_t)w.stride);
        w=consume(h,HP_COUNTER_STORE); w=consume(h,HP_COUNTER_TABLE_FIRST); w=consume(h,HP_COUNTER_TABLE_SECOND); w=consume(h,HP_COUNTER_TABLE_THIRD);
    }
    TW(rd_u16(0xc45986u)); if(!rd_u16(0xc45986u)) return;
    W(value,HP_VALUE,rd_u16(0xc45984u)); SW(value,HP_VALUE,144); W(offset,HP_OFFSET,w.value); ASL(value,HP_VALUE,3);
    W(base,HP_BASE,w.value); AW(value,HP_VALUE,w.value); AW(value,HP_VALUE,w.value); AW(value,HP_VALUE,w.base); EL(value,HP_VALUE);
    L(base,HP_BASE,0x16a8); W(size,HP_SIZE,rd_u16(0xc45986u));
    if((int16_t)w.size<=0) {
        CW(w.size,(uint16_t)-20);
        if((int16_t)w.size<=-20) W(size,HP_SIZE,20);
        else { W(source,HP_SOURCE,w.size); NEG(size,HP_SIZE); AW(source,HP_SOURCE,w.source); AW(source,HP_SOURCE,40); EL(source,HP_SOURCE); AL(base,HP_BASE,w.source); }
    } else { CW(w.size,20); if((int16_t)w.size>=20) W(size,HP_SIZE,20); }
    W(stride,HP_STRIDE,w.size); SW(stride,HP_STRIDE,20); NEG(stride,HP_STRIDE); AW(stride,HP_STRIDE,w.stride); AW(stride,HP_STRIDE,1); AW(size,HP_SIZE,0xdc0);
    height=rd_s16(0xc458d8u); TW((uint16_t)height);
    if(height>0) { W(source,HP_SOURCE,(uint16_t)height); LSL(source,HP_SOURCE,6); SW(size,HP_SIZE,w.source); }
    P(stream,HP_STREAM,rd_u32(0xc456b6u)); L(source,HP_SOURCE,rd_u32(w.stream)); AL(source,HP_SOURCE,w.base); L(secondary,HP_SECONDARY,0xffffffffu);
    AL(base,HP_BASE,w.value); ASL(offset,HP_OFFSET,6); SW(size,HP_SIZE,w.offset); W(control,HP_CONTROL,0x30a);
    L(source,HP_SOURCE,rd_u32(w.stream)); P(stream,HP_STREAM,w.stream+4); AL(source,HP_SOURCE,w.base); w=consume(h,HP_COUNTER_WAIT_OTHER);
    word(h,w.registers+0x42,0); word(h,w.registers+0x74,(uint16_t)w.secondary); word(h,w.registers+0x44,(uint16_t)w.secondary); word(h,w.registers+0x46,(uint16_t)w.secondary);
    word(h,w.registers+0x60,(uint16_t)w.stride); word(h,w.registers+0x66,(uint16_t)w.stride); w=consume(h,HP_COUNTER_STORE_OTHER);
    W(control,HP_CONTROL,0x30a); w=consume(h,HP_COUNTER_NEXT_FIRST); W(control,HP_CONTROL,0x3fa); w=consume(h,HP_COUNTER_NEXT_SECOND);
    W(control,HP_CONTROL,0x3fa); (void)consume(h,HP_COUNTER_NEXT_THIRD);
}
/* The third status path falls through the original C30CC4 shared tail. */
static HudParentState status_blit_tail(HudParentState w,const HudParentHooks *h) {
    P(stream,HP_STREAM,rd_u32(0xc456b6u)); L(source,HP_SOURCE,rd_u32(w.stream+(gaddr)(int32_t)(int16_t)w.source));
    TW(w.stride); if((uint16_t)w.stride) W(secondary,HP_SECONDARY,0xffff);
    TW(w.offset); if((uint16_t)w.offset) W(value,HP_VALUE,0xffff);
    AW(stride,HP_STRIDE,w.offset); AL(source,HP_SOURCE,w.base); SW(size,HP_SIZE,w.stride); AW(stride,HP_STRIDE,w.stride); AW(stride,HP_STRIDE,w.modulo);
    P(registers,HP_REGISTERS,0xdff000u); w=consume(h,HP_STATUS_WAIT);
    word(h,w.registers+0x40,(uint16_t)w.control); word(h,w.registers+0x42,0); word(h,w.registers+0x74,0xffff);
    word(h,w.registers+0x44,(uint16_t)w.value); word(h,w.registers+0x46,(uint16_t)w.secondary); word(h,w.registers+0x60,(uint16_t)w.stride); word(h,w.registers+0x66,(uint16_t)w.stride);
    longword(h,w.registers+0x48,w.source); longword(h,w.registers+0x54,w.source); word(h,w.registers+0x58,(uint16_t)w.size); return w;
}
void draw_status_stream_display(HudParentState w,const HudParentHooks *h) {
    int32_t sum;
    TB(rd_u8(0xc4583eu)); if(rd_s8(0xc4583eu)>0) {
        decrement_byte(h,0xc4583eu); W(value,HP_VALUE,241); sum=(int16_t)w.value+rd_s16(0xc45988u); AW(value,HP_VALUE,rd_u16(0xc45988u));
        if(sum>=0) { CW(w.value,311); if((int16_t)w.value<=311) {
            W(base,HP_BASE,180); AW(base,HP_BASE,rd_u16(0xc458d8u)); W(control,HP_CONTROL,w.value); AW(control,HP_CONTROL,9); W(secondary,HP_SECONDARY,w.base);
            longword(h,0xc456e6u,0xfffff); P(stream,HP_STREAM,0xc46184u); P(stream,HP_STREAM,w.stream+(gaddr)(int32_t)rd_s16(0xc458deu));
            word(h,0xc45954u,bit(h,w.stream+3,3)?9:0); w=consume(h,HP_STATUS_LINE);
        } }
    }
    TB(rd_u8(0xc4583fu)); if(rd_s8(0xc4583fu)>0) {
        decrement_byte(h,0xc4583fu); L(base,HP_BASE,0x1a68); P(screen,HP_SCREEN,3); W(size,HP_SIZE,0x1c3); P(modulo,HP_MODULO,35); W(offset,HP_OFFSET,0); AL(base,HP_BASE,rd_u32(0xc45918u));
        w=consume(h,HP_STATUS_CLIP_FIRST); if(w.less) goto third;
        W(control,HP_CONTROL,0x30a); P(registers,HP_REGISTERS,0xc46184u); W(value,HP_VALUE,rd_u16(0xc458deu));
        W(value,HP_VALUE,rd_u16(w.registers+(gaddr)(int32_t)(int16_t)w.value)); ANDW(value,HP_VALUE,0x800); if((uint16_t)w.value) W(control,HP_CONTROL,0x3fa);
        L(source,HP_SOURCE,12); W(value,HP_VALUE,0x7f); W(secondary,HP_SECONDARY,0x8000); w=consume(h,HP_STATUS_BLIT_FIRST);
    }
    TB(rd_u8(0xc45840u)); if(rd_s8(0xc45840u)>0) {
        decrement_byte(h,0xc45840u); L(base,HP_BASE,0x1a14); P(screen,HP_SCREEN,2); W(size,HP_SIZE,0x1c2); P(modulo,HP_MODULO,37); W(offset,HP_OFFSET,18); AL(base,HP_BASE,rd_u32(0xc45918u));
        w=consume(h,HP_STATUS_CLIP_SECOND); if(w.less) goto third;
        W(control,HP_CONTROL,0x30a); TB(rd_u8(0xc458b5u)); if(rd_u8(0xc458b5u)) W(control,HP_CONTROL,0x3fa);
        L(source,HP_SOURCE,12); W(value,HP_VALUE,0xfff); W(secondary,HP_SECONDARY,0xfc00); (void)consume(h,HP_STATUS_BLIT_SECOND); return;
    }
third:
    TB(rd_u8(0xc45842u)); if(rd_s8(0xc45842u)<=0) return;
    decrement_byte(h,0xc45842u); L(base,HP_BASE,0x1888); P(screen,HP_SCREEN,2); W(size,HP_SIZE,0x202); P(modulo,HP_MODULO,37); W(offset,HP_OFFSET,0); AL(base,HP_BASE,rd_u32(0xc45918u));
    w=consume(h,HP_STATUS_CLIP_THIRD); if(w.less) return;
    W(control,HP_CONTROL,0x30a); TB(rd_u8(0xc457abu)); if(rd_u8(0xc457abu) && bit(h,0xc45842u,0)) W(control,HP_CONTROL,0x3fa);
    L(source,HP_SOURCE,12); W(value,HP_VALUE,0x7f); W(secondary,HP_SECONDARY,0xffff); (void)status_blit_tail(w,h);
}
void draw_record_stream_display(HudParentState w,const HudParentHooks *h) {
    int32_t sum;
    L(base,HP_BASE,0x1c70); P(screen,HP_SCREEN,2); W(size,HP_SIZE,0x202); P(modulo,HP_MODULO,37); W(offset,HP_OFFSET,0); AL(base,HP_BASE,rd_u32(0xc45918u));
    w=consume(h,HP_RECORD_CLIP_FIRST); if(!w.less) {
        W(control,HP_CONTROL,0x30a); TB(rd_u8(0xc45846u)); if(rd_s8(0xc45846u)>0 && bit(h,0xc458dbu,1)) W(control,HP_CONTROL,0x3fa);
        L(source,HP_SOURCE,12); W(value,HP_VALUE,0x3f); W(secondary,HP_SECONDARY,0xfffe); w=consume(h,HP_RECORD_BLIT);
    }
    TB(rd_u8(0xc45845u)); if(rd_s8(0xc45845u)<=0) return;
    decrement_byte(h,0xc45845u); P(table,HP_TABLE,0xc46184u); P(table,HP_TABLE,w.table+(gaddr)(int32_t)rd_s16(0xc458deu)); B(value,HP_VALUE,rd_u8(w.table+0x7c));
    P(table,HP_TABLE,0xc30d22u); observe(h,HP_BIT_CLEAR,HP_VALUE,w.value,7); w.value&=~128u;
    w.value=low_byte(w.value,(uint8_t)((int8_t)w.value>>5)); observe(h,HP_ASR_BYTE,HP_VALUE,5,0);
    w.value=low_word(w.value,(uint16_t)(int16_t)(int8_t)w.value); observe(h,HP_EXT_WORD,HP_VALUE,0,0);
    AW(value,HP_VALUE,w.value); AW(value,HP_VALUE,w.value); P(table,HP_TABLE,w.table+(gaddr)(int32_t)(int16_t)w.value);
    L(value,HP_VALUE,0x129fc); L(base,HP_BASE,0x1d88); W(offset,HP_OFFSET,0); P(screen,HP_SCREEN,2); W(size,HP_SIZE,0x1c2); P(modulo,HP_MODULO,37);
    P(stream,HP_STREAM,rd_u32(0xc456b6u)); AL(base,HP_BASE,rd_u32(0xc45918u)); w=consume(h,HP_RECORD_CLIP_SECOND); if(w.less) return;
    AW(stride,HP_STRIDE,w.offset); AW(offset,HP_OFFSET,w.offset); EL(offset,HP_OFFSET); SW(size,HP_SIZE,w.stride); AW(stride,HP_STRIDE,w.stride); AW(stride,HP_STRIDE,1); EL(stride,HP_STRIDE);
    L(source,HP_SOURCE,rd_u32(w.stream)); AL(source,HP_SOURCE,w.base); L(secondary,HP_SECONDARY,rd_u32(w.table)); P(table,HP_TABLE,w.table+4); AL(secondary,HP_SECONDARY,w.offset); AL(value,HP_VALUE,w.offset);
    W(control,HP_CONTROL,0xf3a); P(registers,HP_REGISTERS,0xdff000u); w=consume(h,HP_RECORD_WAIT);
    word(h,w.registers+0x40,(uint16_t)w.control); word(h,w.registers+0x42,0); word(h,w.registers+0x44,0xffff); word(h,w.registers+0x46,0xffff);
    word(h,w.registers+0x64,(uint16_t)w.stride); word(h,w.registers+0x62,(uint16_t)w.stride); SW(stride,HP_STRIDE,1); AW(stride,HP_STRIDE,w.modulo);
    word(h,w.registers+0x60,(uint16_t)w.stride); word(h,w.registers+0x66,(uint16_t)w.stride); longword(h,w.registers+0x50,w.value); longword(h,w.registers+0x4c,w.secondary);
    longword(h,w.registers+0x48,w.source); longword(h,w.registers+0x54,w.source); word(h,w.registers+0x58,(uint16_t)w.size);
    P(table,HP_TABLE,0xc46184u); P(table,HP_TABLE,w.table+(gaddr)(int32_t)rd_s16(0xc458deu)); if(!bit(h,w.table+2,7)) return;
    W(value,HP_VALUE,14); W(base,HP_BASE,192); W(control,HP_CONTROL,12); W(secondary,HP_SECONDARY,195);
    sum=(int16_t)w.value+rd_s16(0xc45988u); AW(value,HP_VALUE,rd_u16(0xc45988u)); if(sum<0) return; CW(w.value,320); if((int16_t)w.value>=320) return;
    sum=(int16_t)w.control+rd_s16(0xc45988u); AW(control,HP_CONTROL,rd_u16(0xc45988u)); if(sum<0) return; CW(w.control,320); if((int16_t)w.control>=320) return;
    AW(base,HP_BASE,rd_u16(0xc458d8u)); AW(secondary,HP_SECONDARY,rd_u16(0xc458d8u)); longword(h,0xc456e6u,0xfffff); word(h,0xc45954u,0); (void)consume(h,HP_RECORD_LINE);
}
void draw_cached_stream_display(HudParentState w,const HudParentHooks *h) {
    int32_t sum;
    w=consume(h,HP_CACHED_UPDATE); W(value,HP_VALUE,rd_u16(0xc458c4u)); TB(rd_u8(0xc45837u));
    if(rd_s8(0xc45837u)<=0) {
        W(control,HP_CONTROL,rd_u16(0xc459a2u));
        if((int16_t)w.control>=0) { CW(w.value,w.control); if((uint16_t)w.value==(uint16_t)w.control) return; if(bit(h,0xc458dbu,0)) return; }
        else { word(h,0xc459a2u,(uint16_t)(rd_u16(0xc459a2u)&0x7fff)); ANDW(control,HP_CONTROL,0x7fff); W(value,HP_VALUE,w.control); goto draw; }
    }
    word(h,0xc459a2u,(uint16_t)w.value); word(h,0xc459a2u,(uint16_t)(rd_u16(0xc459a2u)|0x8000));
draw:
    P(registers,HP_REGISTERS,0xdff000u); P(stream,HP_STREAM,rd_u32(0xc456b6u)); L(base,HP_BASE,0x17b0); P(screen,HP_SCREEN,2); P(modulo,HP_MODULO,35); W(secondary,HP_SECONDARY,0x1c3); W(offset,HP_OFFSET,12);
    AL(base,HP_BASE,rd_u32(0xc45918u)); w=consume(h,HP_CACHED_CLIP); if(w.less) return;
    AW(stride,HP_STRIDE,w.offset); AL(base,HP_BASE,rd_u32(w.stream+12)); SW(secondary,HP_SECONDARY,w.stride); AW(stride,HP_STRIDE,w.stride); P(screen,HP_SCREEN,(gaddr)(int32_t)(int16_t)w.stride);
    L(size,HP_SIZE,0xffff0000u); ASR(value,HP_VALUE,1); W(source,HP_SOURCE,w.value); ANDW(value,HP_VALUE,15);
    if((uint16_t)w.value) {
        SL(base,HP_BASE,2); SWAP(size,HP_SIZE); NEG(value,HP_VALUE); AW(value,HP_VALUE,16);
        w.value=low_word(w.value,(uint16_t)(((uint16_t)w.value>>4)|((uint16_t)w.value<<12))); observe(h,HP_ROR_WORD,HP_VALUE,4,0);
    }
    ANDW(source,HP_SOURCE,0xf0); ASR(source,HP_SOURCE,3); EL(source,HP_SOURCE); AL(source,HP_SOURCE,0x12afc); AW(offset,HP_OFFSET,w.offset); EL(offset,HP_OFFSET); AL(source,HP_SOURCE,w.offset);
    W(control,HP_CONTROL,0x73a); W(stride,HP_STRIDE,w.screen); AW(stride,HP_STRIDE,7); w=consume(h,HP_CACHED_WAIT);
    word(h,w.registers+0x40,(uint16_t)w.control); word(h,w.registers+0x42,(uint16_t)w.value); word(h,w.registers+0x74,0xffff); word(h,w.registers+0x46,(uint16_t)w.size);
    SWAP(size,HP_SIZE); word(h,w.registers+0x44,(uint16_t)w.size); word(h,w.registers+0x62,(uint16_t)w.stride); SW(stride,HP_STRIDE,7); AW(stride,HP_STRIDE,w.modulo);
    word(h,w.registers+0x60,(uint16_t)w.stride); word(h,w.registers+0x66,(uint16_t)w.stride); longword(h,w.registers+0x4c,w.source); longword(h,w.registers+0x48,w.base); longword(h,w.registers+0x54,w.base); word(h,w.registers+0x58,(uint16_t)w.secondary);
    longword(h,0xc456e6u,0xfffff); W(value,HP_VALUE,208); sum=(int16_t)w.value+rd_s16(0xc45988u); AW(value,HP_VALUE,rd_u16(0xc45988u));
    if(sum<0) return; CW(w.value,320); if((int16_t)w.value>=320) return;
    W(base,HP_BASE,150); W(control,HP_CONTROL,w.value); W(secondary,HP_SECONDARY,w.base); AW(secondary,HP_SECONDARY,7); word(h,0xc45954u,1); (void)consume(h,HP_CACHED_LINE);
}
void draw_bit_selected_points(HudParentState w,const HudParentHooks *h) {
    longword(h,0xc456e6u,0xfffff); W(control,HP_CONTROL,bit(h,0xc4586eu,2)?1:3); word(h,0xc45954u,(uint16_t)w.control);
    W(value,HP_VALUE,293); W(base,HP_BASE,156); w=consume(h,HP_BITS_POINT); if(w.less) return;
    AW(value,HP_VALUE,2); w=consume(h,HP_BITS_FIRST);
    W(control,HP_CONTROL,bit(h,0xc4586eu,1)?4:3); word(h,0xc45954u,(uint16_t)w.control);
    AW(value,HP_VALUE,3); w=consume(h,HP_BITS_SECOND); AW(value,HP_VALUE,2); w=consume(h,HP_BITS_THIRD);
    W(control,HP_CONTROL,bit(h,0xc4586eu,4)?2:3); word(h,0xc45954u,(uint16_t)w.control);
    SW(value,HP_VALUE,2); AW(base,HP_BASE,3); w=consume(h,HP_BITS_FOURTH); AW(value,HP_VALUE,2); w=consume(h,HP_BITS_FIFTH);
    W(control,HP_CONTROL,bit(h,0xc4586eu,5)?2:3); word(h,0xc45954u,(uint16_t)w.control);
    SW(value,HP_VALUE,7); w=consume(h,HP_BITS_SIXTH); AW(value,HP_VALUE,2); w=consume(h,HP_BITS_SEVENTH);
    P(registers,HP_REGISTERS,0xc46184u); P(registers,HP_REGISTERS,w.registers+(gaddr)(int32_t)rd_s16(0xc458deu)); W(control,HP_CONTROL,3);
    if(bit(h,w.registers+0x20,2) && bit(h,0xc458dbu,1)) W(control,HP_CONTROL,1);
    word(h,0xc45954u,(uint16_t)w.control); AW(value,HP_VALUE,8); w=consume(h,HP_BITS_EIGHTH); AW(value,HP_VALUE,2); (void)consume(h,HP_BITS_NINTH);
}
/* Original common C32740 tail; C31A64 also owns its C3273C control word. */
static void draw_digits(HudParentState w,const HudParentHooks *h) {
    int32_t sum;
    SWAP(size,HP_SIZE); W(size,HP_SIZE,rd_u16(0xc45986u)); L(offset,HP_OFFSET,rd_u32(0xc45918u)); W(control,HP_CONTROL,w.value); L(source,HP_SOURCE,0); L(secondary,HP_SECONDARY,rd_u32(0xc45b22u));
    do {
        W(base,HP_BASE,w.secondary); ANDW(base,HP_BASE,15); AW(base,HP_BASE,48); CW(w.base,57); if((int16_t)w.base>57) AW(base,HP_BASE,7);
        P(registers,HP_REGISTERS,w.registers-1); byte(h,w.registers,(uint8_t)w.base); w.secondary>>=4; observe(h,HP_LSR_LONG,HP_SECONDARY,4,0);
        w.control=low_word(w.control,(uint16_t)(w.control-1)); observe(h,HP_DECREMENT,HP_CONTROL,w.control,0);
    } while((uint16_t)w.control!=0xffff);
    TB(w.source);
    if(!(uint8_t)w.source) {
        W(control,HP_CONTROL,w.value); SW(control,HP_CONTROL,1);
        do {
            uint8_t digit=rd_u8(w.registers); P(registers,HP_REGISTERS,w.registers+1); CB(digit,48); if(digit!=48) break;
            byte(h,w.registers-1,32); w.control=low_word(w.control,(uint16_t)(w.control-1)); observe(h,HP_DECREMENT,HP_CONTROL,w.control,0);
        } while((uint16_t)w.control!=0xffff);
    }
    P(screen,HP_SCREEN,w.screen+w.offset); W(offset,HP_OFFSET,0x142); P(registers,HP_REGISTERS,rd_u32(0xc456b6u));
    L(base,HP_BASE,rd_u32(w.registers+(gaddr)(int32_t)(int16_t)w.stride)); AW(size,HP_SIZE,w.size);
    do {
        W(stride,HP_STRIDE,rd_u16(w.table)); P(table,HP_TABLE,w.table+2); W(secondary,HP_SECONDARY,rd_u16(w.table)); P(table,HP_TABLE,w.table+2);
        B(source,HP_SOURCE,rd_u8(w.stream)); P(stream,HP_STREAM,w.stream+1); W(control,HP_CONTROL,w.modulo); AW(control,HP_CONTROL,w.size);
        sum=(int16_t)w.control+(int16_t)w.stride; AW(control,HP_CONTROL,w.stride);
        if(sum>=0) {
            CW(w.control,40); if((int16_t)w.control<40) {
                AW(stride,HP_STRIDE,w.size); EL(stride,HP_STRIDE); AL(stride,HP_STRIDE,w.screen); AL(stride,HP_STRIDE,w.base);
                ANDW(source,HP_SOURCE,255); SW(source,HP_SOURCE,32); AW(source,HP_SOURCE,w.source); P(descriptor,HP_DESCRIPTOR,0xc3d790u);
                P(descriptor,HP_DESCRIPTOR,w.descriptor+(gaddr)(int32_t)rd_s16(w.descriptor+(gaddr)(int32_t)(int16_t)w.source));
                L(source,HP_SOURCE,w.descriptor); SWAP(size,HP_SIZE); W(control,HP_CONTROL,w.size); SWAP(size,HP_SIZE); ORW(control,HP_CONTROL,w.secondary);
                observe(h,HP_BIT_TEST,HP_STRIDE,w.stride,0);
                if(!(w.stride&1)) { observe(h,HP_PUSH_LONG,HP_BASE,w.base,0); L(base,HP_BASE,w.stride); w=consume(h,HP_GLYPH); observe(h,HP_POP_LONG,HP_BASE,0,0); w=restored(h); }
                else { word(h,0xc4599eu,70); w=consume(h,HP_FAULT); }
            }
        }
        w.value=low_word(w.value,(uint16_t)(w.value-1)); observe(h,HP_DECREMENT,HP_VALUE,w.value,0);
    } while((uint16_t)w.value!=0xffff);
}
void draw_record_class_digits(HudParentState w,const HudParentHooks *h) {
    TB(rd_u8(0xc4583bu)); if(rd_s8(0xc4583bu)<=0) return;
    decrement_byte(h,0xc4583bu); P(table,HP_TABLE,0xc46184u); P(table,HP_TABLE,w.table+(gaddr)(int32_t)rd_s16(0xc458deu)); B(value,HP_VALUE,rd_u8(w.table+0x63)); ANDB(value,HP_VALUE,15);
    CB(w.value,9); if((uint8_t)w.value==9) L(value,HP_VALUE,2);
    else { CB(w.value,11); L(value,HP_VALUE,(uint8_t)w.value==11?10:40); }
    longword(h,0xc45b1eu,w.value); w=consume(h,HP_CLASS_BCD); P(table,HP_TABLE,0xc3198cu); P(stream,HP_STREAM,0xc457fau); P(registers,HP_REGISTERS,w.stream+3);
    P(screen,HP_SCREEN,0x1cd2); P(modulo,HP_MODULO,18); W(stride,HP_STRIDE,4); L(value,HP_VALUE,2); W(size,HP_SIZE,0xfca); draw_digits(w,h);
}
void draw_record_scale_digits(HudParentState w,const HudParentHooks *h) {
    TB(rd_u8(0xc4583du)); if(rd_s8(0xc4583du)<=0) return;
    decrement_byte(h,0xc4583du); W(value,HP_VALUE,rd_u16(0xc45a42u)); CW(w.value,128);
    if((uint16_t)w.value==128) L(value,HP_VALUE,10);
    else {
        CW(w.value,64); L(value,HP_VALUE,(uint16_t)w.value==64?20:40); byte(h,0xc4583du,1);
        if(bit(h,0xc45884u,0)) { W(size,HP_SIZE,0xf0a); W(stride,HP_STRIDE,4); word(h,0xc45954u,0); goto point; }
    }
    W(size,HP_SIZE,0xfca); W(stride,HP_STRIDE,4); word(h,0xc45954u,4);
point:
    observe(h,HP_SAVE_WORDS,HP_VALUE,0,0); W(value,HP_VALUE,246); W(base,HP_BASE,188); longword(h,0xc456e6u,0xfffff); w=consume(h,HP_SCALE_POINT);
    observe(h,HP_RESTORE_WORDS,HP_VALUE,0,0); w=restored(h); longword(h,0xc45b1eu,w.value); w=consume(h,HP_SCALE_BCD);
    P(table,HP_TABLE,0xc31a1cu); P(stream,HP_STREAM,0xc457fau); P(registers,HP_REGISTERS,w.stream+2); P(screen,HP_SCREEN,0x1cde); P(modulo,HP_MODULO,30); L(value,HP_VALUE,1); draw_digits(w,h);
}
