#include "hud_render_parents.h"
#include <stdlib.h>
static void observe(const HudRenderHooks *h,enum HudRenderPhase p,enum HudRenderField f,uint32_t v,uint32_t o) {
    if(h && h->observe) h->observe(h->context,p,f,v,o);
}
static HudRenderState consume(const HudRenderHooks *h,enum HudRenderChild child) {
    if(h && h->consume) return h->consume(h->context,child); abort();
}
static HudRenderState restored(const HudRenderHooks *h) {
    if(h && h->restored) return h->restored(h->context); abort();
}
static uint32_t low_word(uint32_t old,uint16_t v) { return (old&0xffff0000u)|v; }
static uint32_t low_byte(uint32_t old,uint8_t v) { return (old&0xffffff00u)|v; }
#define W(f,id,v) do { w.f=low_word(w.f,(uint16_t)(v)); observe(h,HRR_WORD,id,w.f,0); } while(0)
#define B(f,id,v) do { w.f=low_byte(w.f,(uint8_t)(v)); observe(h,HRR_BYTE,id,w.f,0); } while(0)
#define L(f,id,v) do { w.f=(uint32_t)(v); observe(h,HRR_LONG,id,w.f,0); } while(0)
#define P(f,id,v) do { w.f=(gaddr)(v); observe(h,HRR_POINTER,id,w.f,0); } while(0)
#define AW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f+n_)); observe(h,HRR_ADD_WORD,id,n_,0); } while(0)
#define SW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f-n_)); observe(h,HRR_SUB_WORD,id,n_,0); } while(0)
#define AL(f,id,v) do { uint32_t n_=(uint32_t)(v); w.f+=n_; observe(h,HRR_ADD_LONG,id,n_,0); } while(0)
#define SL(f,id,v) do { uint32_t n_=(uint32_t)(v); w.f-=n_; observe(h,HRR_SUB_LONG,id,n_,0); } while(0)
#define ANDW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f&n_)); observe(h,HRR_AND_WORD,id,n_,0); } while(0)
#define ANDB(f,id,v) do { uint8_t n_=(uint8_t)(v); w.f=low_byte(w.f,(uint8_t)(w.f&n_)); observe(h,HRR_AND_BYTE,id,n_,0); } while(0)
#define ORW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f|n_)); observe(h,HRR_OR_WORD,id,n_,0); } while(0)
#define EL(f,id) do { w.f=(uint32_t)(int32_t)(int16_t)w.f; observe(h,HRR_EXT_LONG,id,0,0); } while(0)
#define NEG(f,id) do { w.f=low_word(w.f,(uint16_t)(0u-w.f)); observe(h,HRR_NEG_WORD,id,0,0); } while(0)
#define SWAP(f,id) do { w.f=(w.f<<16)|(w.f>>16); observe(h,HRR_SWAP,id,0,0); } while(0)
#define ASL(f,id,n) do { w.f=low_word(w.f,(uint16_t)((((unsigned)(n)&63u)<16?w.f<<((unsigned)(n)&63u):0))); observe(h,HRR_ASL_WORD,id,n,0); } while(0)
#define LSL(f,id,n) do { w.f=low_word(w.f,(uint16_t)((((unsigned)(n)&63u)<16?w.f<<((unsigned)(n)&63u):0))); observe(h,HRR_LSL_WORD,id,n,0); } while(0)
#define ASR(f,id,n) do { unsigned n_=(unsigned)(n)&63u; w.f=low_word(w.f,(uint16_t)((int16_t)w.f>>(n_<16?n_:15))); observe(h,HRR_ASR_WORD,id,n_,0); } while(0)
#define CW(v,n) observe(h,HRR_COMPARE_WORD,HRR_VALUE,(uint16_t)(n),(uint16_t)(v))
#define CB(v,n) observe(h,HRR_COMPARE_BYTE,HRR_VALUE,(uint8_t)(n),(uint8_t)(v))
#define TW(v) observe(h,HRR_TEST_WORD,HRR_VALUE,(uint16_t)(v),0)
#define TB(v) observe(h,HRR_TEST_BYTE,HRR_VALUE,(uint8_t)(v),0)
static void word(const HudRenderHooks *h,gaddr a,uint16_t v) { wr_u16(a,v); observe(h,HRR_STORE_WORD,HRR_VALUE,v,0); }
static void byte(const HudRenderHooks *h,gaddr a,uint8_t v) { wr_u8(a,v); observe(h,HRR_STORE_BYTE,HRR_VALUE,v,0); }
static void longword(const HudRenderHooks *h,gaddr a,uint32_t v) { wr_u32(a,v); observe(h,HRR_STORE_LONG,HRR_VALUE,v,0); }
static void decrement_byte(const HudRenderHooks *h,gaddr a) { uint8_t old=rd_u8(a); wr_u8(a,(uint8_t)(old-1)); observe(h,HRR_MEMORY_SUB_BYTE,HRR_VALUE,old,1); }
static int bit(const HudRenderHooks *h,gaddr a,unsigned b) { uint8_t v=rd_u8(a); observe(h,HRR_BIT_TEST,HRR_VALUE,v,b); return (v>>b)&1; }

/* MOVEM transfers retain CCR; individual MOVE transfers above update it. */
#define RAW(f,id,v) do { w.f=(uint32_t)(v); observe(h,HRR_RAW_LONG,id,w.f,0); } while(0)
#define MUL(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=(uint32_t)((int32_t)(int16_t)w.f*(int16_t)n_); observe(h,HRR_MULS,id,n_,0); } while(0)
#define MUP(f,id,ptr,pi) do { MUL(f,id,rd_u16(w.ptr)); P(ptr,pi,w.ptr+2); } while(0)
#define ARL(f,id,n) do { unsigned n_=(unsigned)(n)&63u; w.f=(uint32_t)((int32_t)w.f>>(n_<32?n_:31)); observe(h,HRR_ASR_LONG,id,n_,0); } while(0)
#define DU(f,id,n) do { uint16_t n_=(uint16_t)(n); if(n_ && w.f/n_<=65535u) w.f=(w.f%n_)<<16|(w.f/n_); observe(h,HRR_DIVU,id,n_,0); } while(0)

#define AB(f,id,v) do { uint8_t n_=(uint8_t)(v); w.f=low_byte(w.f,(uint8_t)(w.f+n_)); observe(h,HRR_ADD_BYTE,id,n_,0); } while(0)
#define SB(f,id,v) do { uint8_t n_=(uint8_t)(v); w.f=low_byte(w.f,(uint8_t)(w.f-n_)); observe(h,HRR_SUB_BYTE,id,n_,0); } while(0)
#define EW(f,id) do { w.f=low_word(w.f,(uint16_t)(int16_t)(int8_t)w.f); observe(h,HRR_EXT_WORD,id,0,0); } while(0)
#define NL(f,id) do { w.f=0u-w.f; observe(h,HRR_NEG_LONG,id,0,0); } while(0)
#define CL(v,n) observe(h,HRR_COMPARE_LONG,HRR_VALUE,(uint32_t)(n),(uint32_t)(v))
#define TL(v) observe(h,HRR_TEST_LONG,HRR_VALUE,(uint32_t)(v),0)
static int memory_decrement(const HudRenderHooks *h,gaddr at) {
    uint8_t v=rd_u8(at); wr_u8(at,(uint8_t)(v-1)); observe(h,HRR_MEMORY_SUB_BYTE,HRR_VALUE,v,1); return (int8_t)v-1>=0;
}
static void memory_increment(const HudRenderHooks *h,gaddr at) {
    uint8_t v=rd_u8(at);wr_u8(at,(uint8_t)(v+1));observe(h,HRR_MEMORY_ADD_BYTE,HRR_VALUE,v,1);
}
static HudRenderState load_words(HudRenderState w,const HudRenderHooks *h,gaddr at,uint16_t mask) {
    (void)w;observe(h,HRR_LOAD_WORDS_AT,HRR_VALUE,at,mask);return restored(h);
}
static HudRenderState load_longs(HudRenderState w,const HudRenderHooks *h,gaddr at,uint16_t mask) {
    (void)w;observe(h,HRR_LOAD_LONGS_AT,HRR_VALUE,at,mask);return restored(h);
}
static void store_words(const HudRenderHooks *h,gaddr at,uint16_t mask) { observe(h,HRR_STORE_WORDS_AT,HRR_VALUE,at,mask); }
static void store_longs(const HudRenderHooks *h,gaddr at,uint16_t mask) { observe(h,HRR_STORE_LONGS_AT,HRR_VALUE,at,mask); }
static int decrement_word(const HudRenderHooks *h,gaddr at) {
 uint16_t old=rd_u16(at);wr_u16(at,(uint16_t)(old-1));observe(h,HRR_MEMORY_SUB_WORD,HRR_VALUE,old,1);return (int16_t)old-1;
}
#define ALL(f,id,n) do { unsigned n_=(unsigned)(n)&63u; w.f=n_<32?w.f<<n_:0;observe(h,HRR_ASL_LONG,id,n_,0); } while(0)
static HudRenderState save_longs(HudRenderState w,const HudRenderHooks *h,uint16_t mask) { observe(h,HRR_SAVE_LONGS,HRR_VALUE,mask,0);return restored(h); }
static HudRenderState restore_longs(HudRenderState w,const HudRenderHooks *h,uint16_t mask) { observe(h,HRR_RESTORE_LONGS,HRR_VALUE,mask,0);return restored(h); }

/* Typed transfers keep the source's bus order and operand widths. */
#define S16(v) ((int16_t)(v))
#define S8(v) ((int8_t)(v))
#define SADDR(v) ((gaddr)(int32_t)(int16_t)(v))
#define NB(f,id) do {w.f=low_byte(w.f,(uint8_t)(0u-w.f));observe(h,HRR_NEG_BYTE,id,0,0);}while(0)
#define ANDL(f,id,v) do {uint32_t n_=(uint32_t)(v);w.f&=n_;observe(h,HRR_AND_LONG,id,n_,0);}while(0)
static HudRenderState push_long(HudRenderState w,const HudRenderHooks *h,uint32_t v) {observe(h,HRR_PUSH_LONG,HRR_VALUE,v,0);return restored(h);}
static HudRenderState pop_pointer(HudRenderState w,const HudRenderHooks *h,enum HudRenderField f) {observe(h,HRR_POP_POINTER,f,0,0);return restored(h);}
static HudRenderState push_word(HudRenderState w,const HudRenderHooks *h,uint16_t v) {observe(h,HRR_PUSH_WORD,HRR_VALUE,v,0);return restored(h);}
static HudRenderState pop_word(HudRenderState w,const HudRenderHooks *h,enum HudRenderField f) {observe(h,HRR_POP_WORD,f,0,0);return restored(h);}
static HudRenderState link_frame(HudRenderState w,const HudRenderHooks *h,unsigned bytes) {observe(h,HRR_LINK_FRAME,HRR_VALUE,bytes,0);return restored(h);}
static void unlink_frame(const HudRenderHooks *h) {observe(h,HRR_UNLINK_FRAME,HRR_VALUE,0,0);}
static void increment_word(const HudRenderHooks *h,gaddr at,unsigned n) {uint16_t v=rd_u16(at);wr_u16(at,(uint16_t)(v+n));observe(h,HRR_MEMORY_ADD_WORD,HRR_VALUE,v,n);}
static void logic_long(const HudRenderHooks *h,gaddr at,uint32_t mask,int set) {uint32_t v=rd_u32(at);v=set?v|mask:v&mask;wr_u32(at,v);observe(h,HRR_MEMORY_LOGIC_LONG,HRR_VALUE,v,0);}
static void logic_byte(const HudRenderHooks *h,gaddr at,uint8_t mask,int set) {byte(h,at,set?rd_u8(at)|mask:rd_u8(at)&mask);}
static int dbra(uint32_t *v,const HudRenderHooks *h,enum HudRenderField f) {*v=low_word(*v,(uint16_t)(*v-1));observe(h,HRR_DECREMENT,f,*v,0);return (uint16_t)*v!=0xffff;}
static void blitter_ready(const HudRenderHooks *h,gaddr custom,gaddr source_pc) {observe(h,HRR_WAIT_BLITTER,HRR_VALUE,source_pc,custom);}

/* C098C6: every ground point and all three matrix rows. */
void hud_render_ground_points(HudRenderState w,const HudRenderHooks *h) {
 P(descriptor,HRR_DESCRIPTOR,0xc48390);W(control,HRR_CONTROL,w.offset);ASR(control,HRR_CONTROL,1);AW(control,HRR_CONTROL,w.offset);P(descriptor,HRR_DESCRIPTOR,w.descriptor+SADDR(w.control));
 word(h,w.frame-10,(uint16_t)w.value);P(table,HRR_TABLE,rd_u32(0xc45a32));P(table,HRR_TABLE,w.table+6);P(table,HRR_TABLE,w.table+SADDR(w.offset));P(registers,HRR_REGISTERS,0xc45bd8);P(screen,HRR_SCREEN,w.registers);
 W(value,HRR_VALUE,rd_u16(w.frame-0x86));W(base,HRR_BASE,rd_u16(w.frame-0x82));AW(value,HRR_VALUE,rd_u16(0xc45b2a));AW(base,HRR_BASE,rd_u16(0xc45b2e));w=load_words(w,h,w.frame-0x78,0x48);
 do {
  W(control,HRR_CONTROL,rd_u16(w.table));P(table,HRR_TABLE,w.table+2);W(source,HRR_SOURCE,rd_u16(w.table));P(table,HRR_TABLE,w.table+2);W(offset,HRR_OFFSET,rd_u16(w.frame-8));ASR(control,HRR_CONTROL,w.offset);ASR(source,HRR_SOURCE,w.offset);AW(control,HRR_CONTROL,w.value);AW(source,HRR_SOURCE,w.base);P(registers,HRR_REGISTERS,w.screen);
  W(stride,HRR_STRIDE,w.control);W(offset,HRR_OFFSET,w.source);MUP(stride,HRR_STRIDE,registers,HRR_REGISTERS);P(registers,HRR_REGISTERS,w.registers+2);MUP(offset,HRR_OFFSET,registers,HRR_REGISTERS);AL(offset,HRR_OFFSET,w.stride);ARL(offset,HRR_OFFSET,8);AW(offset,HRR_OFFSET,w.secondary);word(h,w.descriptor,(uint16_t)w.offset);P(descriptor,HRR_DESCRIPTOR,w.descriptor+2);
  W(stride,HRR_STRIDE,w.control);W(offset,HRR_OFFSET,w.source);MUP(stride,HRR_STRIDE,registers,HRR_REGISTERS);P(registers,HRR_REGISTERS,w.registers+2);MUP(offset,HRR_OFFSET,registers,HRR_REGISTERS);AL(offset,HRR_OFFSET,w.stride);ARL(offset,HRR_OFFSET,8);AW(offset,HRR_OFFSET,w.size);word(h,w.descriptor,(uint16_t)w.offset);P(descriptor,HRR_DESCRIPTOR,w.descriptor+2);
  MUP(control,HRR_CONTROL,registers,HRR_REGISTERS);MUL(source,HRR_SOURCE,rd_u16(w.registers+2));AL(source,HRR_SOURCE,w.control);ARL(source,HRR_SOURCE,8);AW(source,HRR_SOURCE,rd_u16(w.frame-0x74));word(h,w.descriptor,(uint16_t)w.source);P(descriptor,HRR_DESCRIPTOR,w.descriptor+2);
 }while(decrement_word(h,w.frame-10)>0);
}

static HudRenderState indexed_corner(HudRenderState w,const HudRenderHooks *h,int first) {
 W(base,HRR_BASE,rd_u16(w.stream));P(stream,HRR_STREAM,w.stream+2);P(screen,HRR_SCREEN,w.descriptor+SADDR(w.base));longword(h,w.registers,rd_u32(w.screen));P(screen,HRR_SCREEN,w.screen+4);P(registers,HRR_REGISTERS,w.registers+4);
 if(first){W(size,HRR_SIZE,rd_u16(w.screen));word(h,w.registers,(uint16_t)w.size);}else word(h,w.registers,rd_u16(w.screen));P(registers,HRR_REGISTERS,w.registers+2);if(!first)ANDW(size,HRR_SIZE,rd_u16(w.screen));return w;
}
void hud_render_indexed_face(HudRenderState w,const HudRenderHooks *h) {
 int count;
 W(offset,HRR_OFFSET,rd_u16(w.stream));P(stream,HRR_STREAM,w.stream+2);P(registers,HRR_REGISTERS,0xc4bf90);word(h,w.registers,0);P(registers,HRR_REGISTERS,w.registers+2);P(descriptor,HRR_DESCRIPTOR,0xc48390);word(h,w.registers,(uint16_t)w.offset);P(registers,HRR_REGISTERS,w.registers+2);SW(offset,HRR_OFFSET,3);
 w=indexed_corner(w,h,1);w=indexed_corner(w,h,0);do {w=indexed_corner(w,h,0);count=S16(w.offset)-1;SW(offset,HRR_OFFSET,1);}while(count>=0);
 word(h,0xc45954,rd_u16(w.stream));P(stream,HRR_STREAM,w.stream+2);TW(w.size);if(S16(w.size)<0){L(value,HRR_VALUE,0);return;}w=push_long(w,h,w.stream);w=consume(h,HRR_CALL_C0999C);w=pop_pointer(w,h,HRR_STREAM);
}
static HudRenderState contiguous_corner(HudRenderState w,const HudRenderHooks *h,int first) {
 longword(h,w.registers,rd_u32(w.descriptor));P(descriptor,HRR_DESCRIPTOR,w.descriptor+4);P(registers,HRR_REGISTERS,w.registers+4);
 if(first){W(size,HRR_SIZE,rd_u16(w.descriptor));P(descriptor,HRR_DESCRIPTOR,w.descriptor+2);word(h,w.registers,(uint16_t)w.size);P(registers,HRR_REGISTERS,w.registers+2);}
 else {word(h,w.registers,rd_u16(w.descriptor));P(registers,HRR_REGISTERS,w.registers+2);ANDW(size,HRR_SIZE,rd_u16(w.descriptor));P(descriptor,HRR_DESCRIPTOR,w.descriptor+2);}return w;
}
static void contiguous_face(HudRenderState w,const HudRenderHooks *h,int outlined) {
 W(offset,HRR_OFFSET,rd_u16(w.stream));P(stream,HRR_STREAM,w.stream+2);P(registers,HRR_REGISTERS,0xc4bf90);word(h,w.registers,0);P(registers,HRR_REGISTERS,w.registers+2);P(descriptor,HRR_DESCRIPTOR,0xc48390);
 if(outlined){P(descriptor,HRR_DESCRIPTOR,w.descriptor+SADDR(rd_u16(w.stream)));P(stream,HRR_STREAM,w.stream+2);}word(h,w.registers,(uint16_t)w.offset);P(registers,HRR_REGISTERS,w.registers+2);SW(offset,HRR_OFFSET,outlined?3:4);
 w=contiguous_corner(w,h,1);w=contiguous_corner(w,h,0);w=contiguous_corner(w,h,0);do {w=contiguous_corner(w,h,0);}while(dbra(&w.offset,h,HRR_OFFSET));
 if(!outlined){word(h,0xc45954,rd_u16(w.stream));P(stream,HRR_STREAM,w.stream+2);}TW(w.size);if(S16(w.size)<0){L(value,HRR_VALUE,0);return;}
 if(outlined){word(h,0xc45954,7);W(base,HRR_BASE,3);W(control,HRR_CONTROL,3);store_words(h,0xc456e6,6);}w=push_long(w,h,w.stream);w=consume(h,outlined?HRR_CALL_C09A5A:HRR_CALL_C099E8);w=pop_pointer(w,h,HRR_STREAM);
 if(outlined){W(base,HRR_BASE,15);W(control,HRR_CONTROL,0xffff);store_words(h,0xc456e6,6);}
}
void hud_render_outlined_face(HudRenderState w,const HudRenderHooks *h) {contiguous_face(w,h,1);}
void hud_render_coloured_face(HudRenderState w,const HudRenderHooks *h) {contiguous_face(w,h,0);}

void hud_render_view_marker(HudRenderState w,const HudRenderHooks *h) {
 W(value,HRR_VALUE,160);AW(value,HRR_VALUE,rd_u16(0xc45988));CW(1,w.value);if(S16(w.value)<=1)return;CW(317,w.value);if(S16(w.value)>=317)return;W(base,HRR_BASE,129);AW(base,HRR_BASE,rd_u16(0xc458d8));word(h,0xc45954,8);
 observe(h,HRR_SAVE_WORDS,HRR_VALUE,0xc000,0);w=restored(h);w=consume(h,HRR_CALL_C3332A);observe(h,HRR_RESTORE_WORDS,HRR_VALUE,3,0);w=restored(h);AW(base,HRR_BASE,1);
 observe(h,HRR_SAVE_WORDS,HRR_VALUE,0xc000,0);w=restored(h);w=consume(h,HRR_CALL_C3333A);observe(h,HRR_RESTORE_WORDS,HRR_VALUE,3,0);w=restored(h);AW(base,HRR_BASE,1);
 observe(h,HRR_SAVE_WORDS,HRR_VALUE,0xc000,0);w=restored(h);w=consume(h,HRR_CALL_C3334A);observe(h,HRR_RESTORE_WORDS,HRR_VALUE,3,0);w=restored(h);SW(value,HRR_VALUE,1);AW(base,HRR_BASE,1);
 observe(h,HRR_SAVE_WORDS,HRR_VALUE,0xc000,0);w=restored(h);w=consume(h,HRR_CALL_C3335C);observe(h,HRR_RESTORE_WORDS,HRR_VALUE,3,0);w=restored(h);AW(value,HRR_VALUE,2);w=consume(h,HRR_CALL_C33368);
}

/* C304FA: ordered lane-blit setup, including both minterms. */
void hud_render_blit_lane(HudRenderState w,const HudRenderHooks *h) {
 P(stream,HRR_STREAM,rd_u32(0xc456b6));L(control,HRR_CONTROL,rd_u32(w.stream+SADDR(w.value)));W(value,HRR_VALUE,rd_u16(0xc4596e));L(base,HRR_BASE,rd_u32(0xc45968));AL(base,HRR_BASE,w.control);L(control,HRR_CONTROL,rd_u32(0xc45964));W(size,HRR_SIZE,rd_u16(0xc45982));SW(size,HRR_SIZE,rd_u16(0xc458d8));SW(size,HRR_SIZE,183);AW(size,HRR_SIZE,w.size);AW(size,HRR_SIZE,w.size);EL(size,HRR_SIZE);L(stride,HRR_STRIDE,0x12adc);AL(stride,HRR_STRIDE,w.size);SL(stride,HRR_STRIDE,2);W(source,HRR_SOURCE,w.value);ANDW(source,HRR_SOURCE,63);SW(source,HRR_SOURCE,3);NEG(source,HRR_SOURCE);CW(1,w.source);
 if(S16(w.source)!=1){W(size,HRR_SIZE,rd_u16(0xc4597c));ASR(size,HRR_SIZE,4);W(offset,HRR_OFFSET,rd_u16(0xc45986));AW(offset,HRR_OFFSET,12);CW(w.offset,w.size);if(S16(w.size)==S16(w.offset))SL(stride,HRR_STRIDE,2);}
 P(registers,HRR_REGISTERS,0xdff000);blitter_ready(h,w.registers,0xc30568);observe(h,HRR_BIT_TEST,HRR_SECONDARY,w.secondary,0);word(h,w.registers+0x40,(w.secondary&1)?0xfec:0xf4c);word(h,w.registers+0x42,2);longword(h,w.registers+0x50,w.control);longword(h,w.registers+0x48,w.stride);word(h,w.registers+0x60,(uint16_t)w.source);longword(h,w.registers+0x4c,w.base);longword(h,w.registers+0x54,w.base);word(h,w.registers+0x58,(uint16_t)w.value);
}

void hud_render_symbol(HudRenderState w,const HudRenderHooks *h) {
 TW(w.source);
 if(!(uint16_t)w.source){CW(88,w.value);if(S16(w.value)<=88)return;CW(230,w.value);if(S16(w.value)>=230)return;CW(39,w.base);if(S16(w.base)<=39)return;CW(141,w.base);if(S16(w.base)>=141)return;P(registers,HRR_REGISTERS,0xc349d0);}
 else {W(control,HRR_CONTROL,rd_u16(0xc458da));ANDW(control,HRR_CONTROL,3);if(!(uint16_t)w.control)return;CW(96,w.value);if(S16(w.value)<=96)return;CW(222,w.value);if(S16(w.value)>=222)return;CW(46,w.base);if(S16(w.base)<=46)return;CW(134,w.base);if(S16(w.base)>=134)return;P(registers,HRR_REGISTERS,0xc349ea);}
 AW(value,HRR_VALUE,rd_u16(0xc45988));CW(10,w.value);if(S16(w.value)<10)return;CW(310,w.value);if(S16(w.value)>310)return;AW(base,HRR_BASE,rd_u16(0xc458d8));w=link_frame(w,h,16);word(h,w.frame-2,(uint16_t)w.value);word(h,w.frame-4,(uint16_t)w.base);
 for(;;){B(value,HRR_VALUE,rd_u8(w.registers));P(registers,HRR_REGISTERS,w.registers+1);B(base,HRR_BASE,rd_u8(w.registers));P(registers,HRR_REGISTERS,w.registers+1);B(control,HRR_CONTROL,w.value);w.control=low_byte(w.control,(uint8_t)(w.control|w.base));observe(h,HRR_OR_BYTE,HRR_CONTROL,w.base,0);if(!(uint8_t)w.control)break;EW(value,HRR_VALUE);EW(base,HRR_BASE);AW(value,HRR_VALUE,rd_u16(w.frame-2));AW(base,HRR_BASE,rd_u16(w.frame-4));w=push_long(w,h,w.registers);w=consume(h,HRR_CALL_C3493C);w=pop_pointer(w,h,HRR_REGISTERS);}
 unlink_frame(h);
}

/* C345A0: forward/backward quarter walks with different unclipped Y bias. */
void hud_render_ring(HudRenderState w,const HudRenderHooks *h) {
 unsigned quarter;int clipped=1;
 static const enum HudRenderChild plain[]={HRR_CALL_C34606,HRR_CALL_C34630,HRR_CALL_C3465A,HRR_CALL_C34684};
 static const enum HudRenderChild window[]={HRR_CALL_C346F8,HRR_CALL_C34744,HRR_CALL_C34792,HRR_CALL_C347E0};
 w=link_frame(w,h,16);word(h,w.frame-2,(uint16_t)w.value);word(h,w.frame-4,(uint16_t)w.base);word(h,w.frame-14,(uint16_t)w.control);
 CW(14,w.value);if(S16(w.value)<=14)goto window_bounds;CW(306,w.value);if(S16(w.value)>=306)goto window_bounds;W(control,HRR_CONTROL,99);AW(control,HRR_CONTROL,rd_u16(0xc45988));CW(w.control,w.value);if(S16(w.value)<=S16(w.control))goto window_bounds;W(control,HRR_CONTROL,219);AW(control,HRR_CONTROL,rd_u16(0xc45988));CW(w.control,w.value);if(S16(w.value)>=S16(w.control))goto window_bounds;CW(57,w.base);if(S16(w.base)<=57)goto window_bounds;CW(132,w.base);if(S16(w.base)>=132)goto window_bounds;clipped=0;goto quarters;
window_bounds:
 W(value,HRR_VALUE,85);AW(value,HRR_VALUE,rd_u16(0xc45988));word(h,w.frame-6,(uint16_t)w.value);W(value,HRR_VALUE,233);AW(value,HRR_VALUE,rd_u16(0xc45988));word(h,w.frame-8,(uint16_t)w.value);word(h,w.frame-10,45);word(h,w.frame-12,144);
quarters:
 for(quarter=0;quarter<4;++quarter) {
  for(;;) {
   if(!(quarter&1)){B(value,HRR_VALUE,rd_u8(w.registers));P(registers,HRR_REGISTERS,w.registers+1);B(base,HRR_BASE,rd_u8(w.registers));P(registers,HRR_REGISTERS,w.registers+1);if(S8(w.base)<0)break;}
   else {P(registers,HRR_REGISTERS,w.registers-1);B(base,HRR_BASE,rd_u8(w.registers));P(registers,HRR_REGISTERS,w.registers-1);B(value,HRR_VALUE,rd_u8(w.registers));if(S8(w.value)<0)break;}
   if(quarter>=2)NB(value,HRR_VALUE);if(quarter==0 || quarter==3)NB(base,HRR_BASE);else if(!clipped)SB(base,HRR_BASE,1);
   EW(value,HRR_VALUE);EW(base,HRR_BASE);AW(value,HRR_VALUE,rd_u16(w.frame-2));AW(base,HRR_BASE,rd_u16(w.frame-4));
   if(clipped){CW(14,w.value);if(S16(w.value)<=14)goto next_point;CW(306,w.value);if(S16(w.value)>=306)goto next_point;CW(rd_u16(w.frame-6),w.value);if(S16(w.value)<=rd_s16(w.frame-6))goto next_point;CW(rd_u16(w.frame-8),w.value);if(S16(w.value)>=rd_s16(w.frame-8))goto next_point;CW(rd_u16(w.frame-10),w.base);if(S16(w.base)<=rd_s16(w.frame-10))goto next_point;CW(rd_u16(w.frame-12),w.base);if(S16(w.base)>=rd_s16(w.frame-12))goto next_point;}
   w=push_long(w,h,w.registers);w=consume(h,clipped?window[quarter]:plain[quarter]);w=pop_pointer(w,h,HRR_REGISTERS);
next_point:if(decrement_word(h,w.frame-14)<0)goto done;
  }
  if(quarter==0 || quarter==2)P(registers,HRR_REGISTERS,w.registers-2);else if(quarter==1)P(registers,HRR_REGISTERS,w.registers+2);
 }
done:unlink_frame(h);
}

/* C347F2: select exactly one point along the same four source quarters. */
void hud_render_ring_point(HudRenderState w,const HudRenderHooks *h) {
 unsigned quarter;
 w=link_frame(w,h,16);word(h,w.frame-2,(uint16_t)w.value);word(h,w.frame-4,(uint16_t)w.base);word(h,w.frame-14,(uint16_t)w.control);W(value,HRR_VALUE,85);AW(value,HRR_VALUE,rd_u16(0xc45988));word(h,w.frame-6,(uint16_t)w.value);W(value,HRR_VALUE,233);AW(value,HRR_VALUE,rd_u16(0xc45988));word(h,w.frame-8,(uint16_t)w.value);word(h,w.frame-10,45);word(h,w.frame-12,144);
 for(quarter=0;quarter<4;++quarter){
  for(;;){
   if(!(quarter&1)){B(value,HRR_VALUE,rd_u8(w.registers));P(registers,HRR_REGISTERS,w.registers+1);B(base,HRR_BASE,rd_u8(w.registers));P(registers,HRR_REGISTERS,w.registers+1);if(S8(w.base)<0)break;}
   else {P(registers,HRR_REGISTERS,w.registers-1);B(base,HRR_BASE,rd_u8(w.registers));P(registers,HRR_REGISTERS,w.registers-1);B(value,HRR_VALUE,rd_u8(w.registers));if(S8(w.value)<0){if(quarter==3)goto done;break;}}
   if(quarter>=2)NB(value,HRR_VALUE);if(quarter==0 || quarter==3)NB(base,HRR_BASE);if(decrement_word(h,w.frame-14)<0)goto selected;
  }
  if(quarter==0){P(registers,HRR_REGISTERS,w.registers-2);decrement_word(h,w.frame-4);}else if(quarter==1)P(registers,HRR_REGISTERS,w.registers+2);else {P(registers,HRR_REGISTERS,w.registers-2);increment_word(h,w.frame-4,1);}
 }
 goto done;
selected:
 EW(value,HRR_VALUE);EW(base,HRR_BASE);AW(value,HRR_VALUE,rd_u16(w.frame-2));AW(base,HRR_BASE,rd_u16(w.frame-4));CW(2,w.value);if(S16(w.value)<=2)goto done;CW(318,w.value);if(S16(w.value)>=318)goto done;CW(rd_u16(w.frame-6),w.value);if(S16(w.value)<=rd_s16(w.frame-6))goto done;CW(rd_u16(w.frame-8),w.value);if(S16(w.value)>=rd_s16(w.frame-8))goto done;CW(rd_u16(w.frame-10),w.base);if(S16(w.base)<=rd_s16(w.frame-10))goto done;CW(rd_u16(w.frame-12),w.base);if(S16(w.base)>=rd_s16(w.frame-12))goto done;w=consume(h,HRR_CALL_C348A6);
done:unlink_frame(h);
}

static HudRenderState dash(HudRenderState w,const HudRenderHooks *h,uint16_t x,uint16_t y,uint16_t first,enum HudRenderChild child) {
 W(value,HRR_VALUE,x);AW(value,HRR_VALUE,rd_u16(0xc45988));CW(first,w.value);if(S16(w.value)<first)return w;W(base,HRR_BASE,y);W(control,HRR_CONTROL,w.value);AW(control,HRR_CONTROL,4);CW(315,w.control);if(S16(w.control)>315)return w;W(secondary,HRR_SECONDARY,w.base);AW(base,HRR_BASE,rd_u16(0xc458d8));AW(secondary,HRR_SECONDARY,rd_u16(0xc458d8));return consume(h,child);
}
/* C34296..C342AA: the actual frame-line right-end clamp. With the original
 * monotone frame table the CLR path is cold; keep its complete behavior. */
HudRenderState hud_render_frame_right(HudRenderState w,const HudRenderHooks *h) {
 int sum=S16(w.control)+rd_s16(0xc45988);AW(control,HRR_CONTROL,rd_u16(0xc45988));if(sum<0)W(control,HRR_CONTROL,0);CW(319,w.control);if(S16(w.control)>319)W(control,HRR_CONTROL,319);return w;
}
void hud_render_hud_marks(HudRenderState w,const HudRenderHooks *h) {
 int sum;
 longword(h,0xc456e6,0x000fffff);word(h,0xc45954,10);W(value,HRR_VALUE,159);W(base,HRR_BASE,71);w=consume(h,HRR_CALL_C34160);
 if(!w.less){AW(base,HRR_BASE,1);w=consume(h,HRR_CALL_C3416A);AW(base,HRR_BASE,2);w=consume(h,HRR_CALL_C34172);AW(base,HRR_BASE,1);w=consume(h,HRR_CALL_C3417A);w=dash(w,h,157,72,4,HRR_CALL_C341AC);}
 word(h,0xc45954,10);w=dash(w,h,141,90,5,HRR_CALL_C341E6);w=dash(w,h,173,90,5,HRR_CALL_C34218);P(registers,HRR_REGISTERS,0xc46184);P(registers,HRR_REGISTERS,w.registers+SADDR(rd_u16(0xc458de)));B(value,HRR_VALUE,rd_u8(w.registers+0x62));CB(16,w.value);if((uint8_t)w.value!=16){CB(17,w.value);if((uint8_t)w.value!=17){CB(18,w.value);if((uint8_t)w.value!=18)return;}}
 word(h,0xc459aa,0);
 do {
  P(registers,HRR_REGISTERS,0xc34560);P(registers,HRR_REGISTERS,w.registers+SADDR(rd_u16(0xc459aa)));w=load_words(w,h,w.registers,15);P(registers,HRR_REGISTERS,0xc34578);P(registers,HRR_REGISTERS,w.registers+SADDR(rd_u16(0xc459aa)));word(h,0xc45954,rd_u16(w.registers));sum=S16(w.value)+rd_s16(0xc45988);AW(value,HRR_VALUE,rd_u16(0xc45988));
  if(sum<0){W(value,HRR_VALUE,0);sum=S16(w.control)+rd_s16(0xc45988);AW(control,HRR_CONTROL,rd_u16(0xc45988));if(sum<0)goto next_frame;goto clamp_right;}
  CW(319,w.value);if(S16(w.value)>319){W(value,HRR_VALUE,319);AW(control,HRR_CONTROL,rd_u16(0xc45988));CW(319,w.control);if(S16(w.control)>319)goto next_frame;goto draw_frame;}
  w=hud_render_frame_right(w,h);goto draw_frame;
clamp_right:CW(319,w.control);if(S16(w.control)>319)W(control,HRR_CONTROL,319);
draw_frame:AW(base,HRR_BASE,rd_u16(0xc458d8));AW(secondary,HRR_SECONDARY,rd_u16(0xc458d8));w=consume(h,HRR_CALL_C342B6);
next_frame:increment_word(h,0xc459aa,4);CW(20,rd_u16(0xc459aa));
 }while(rd_s16(0xc459aa)<20);
}

void hud_render_tick_row(HudRenderState w,const HudRenderHooks *h) {
 int next;
 w=link_frame(w,h,8);word(h,w.frame-4,(uint16_t)w.base);W(value,HRR_VALUE,rd_u16(0xc4598c));w=consume(h,HRR_CALL_C34074);if(!w.less){SW(base,HRR_BASE,1);w=consume(h,HRR_CALL_C3407E);}word(h,w.frame-2,0);
 for(;;){W(base,HRR_BASE,rd_u16(w.frame-4));next=S16(w.value)+10;AW(value,HRR_VALUE,10);if(next<0)break;CW(319,w.value);if(S16(w.value)>319)break;W(control,HRR_CONTROL,185);AW(control,HRR_CONTROL,rd_u16(0xc45988));CW(w.control,w.value);if(S16(w.value)>=S16(w.control))break;w=push_word(w,h,(uint16_t)w.value);CW(1,rd_u16(w.frame-2));if(rd_u16(w.frame-2)!=1){CW(3,rd_u16(w.frame-2));if(rd_u16(w.frame-2)!=3){w=consume(h,HRR_CALL_C340BA);goto right_done;}}
  w=consume(h,HRR_CALL_C340C2);W(base,HRR_BASE,rd_u16(w.frame-4));SW(base,HRR_BASE,1);w=consume(h,HRR_CALL_C340CE);
right_done:w=pop_word(w,h,HRR_VALUE);increment_word(h,w.frame-2,1);
 }
 W(value,HRR_VALUE,rd_u16(0xc4598c));AW(value,HRR_VALUE,rd_u16(0xc45988));word(h,w.frame-2,0);
 for(;;){W(base,HRR_BASE,rd_u16(w.frame-4));next=S16(w.value)-10;SW(value,HRR_VALUE,10);if(next<0)break;CW(319,w.value);if(S16(w.value)>319)break;W(control,HRR_CONTROL,133);AW(control,HRR_CONTROL,rd_u16(0xc45988));CW(w.control,w.value);if(S16(w.value)<=S16(w.control))break;w=push_word(w,h,(uint16_t)w.value);CW(1,rd_u16(w.frame-2));if(rd_u16(w.frame-2)!=1){CW(3,rd_u16(w.frame-2));if(rd_u16(w.frame-2)!=3){w=consume(h,HRR_CALL_C3411E);goto left_done;}}
  w=consume(h,HRR_CALL_C34126);W(base,HRR_BASE,rd_u16(w.frame-4));SW(base,HRR_BASE,1);w=consume(h,HRR_CALL_C34132);
left_done:w=pop_word(w,h,HRR_VALUE);increment_word(h,w.frame-2,1);
 }
 unlink_frame(h);
}

/* C33DA4's branch tail is owned by the enclosing source function. */
static void clear_seeker(HudRenderState w,const HudRenderHooks *h) {
 L(stride,HRR_STRIDE,rd_u32(0xc45b50));ANDL(stride,HRR_STRIDE,0x4200);if(w.stride){logic_long(h,0xc45b50,0xffffbdff,0);logic_long(h,0xc45b54,8,1);}
}
void hud_render_seeker_state(HudRenderState w,const HudRenderHooks *h) {
 int difference;
 TW(rd_u16(0xc459c0));if(rd_s16(0xc459c0)<0){clear_seeker(w,h);return;}P(table,HRR_TABLE,0xc46184);P(table,HRR_TABLE,w.table+SADDR(rd_u16(0xc458de)));B(stride,HRR_STRIDE,rd_u8(w.table+0x63));ANDB(stride,HRR_STRIDE,0xf0);CB(16,w.stride);if((uint8_t)w.stride==16){clear_seeker(w,h);return;}w=load_words(w,h,0xc45942,3);TW(w.value);if(S16(w.value)<0){clear_seeker(w,h);return;}
 CW(96,w.value);if(S16(w.value)<=96)goto outside;CW(222,w.value);if(S16(w.value)>=222)goto outside;CW(46,w.base);if(S16(w.base)<=46)goto outside;CW(134,w.base);if(S16(w.base)>=134)goto outside;
 w=load_words(w,h,0xc4593a,12);difference=S16(w.control)-S16(w.value);SW(control,HRR_CONTROL,w.value);if(difference<0)NEG(control,HRR_CONTROL);CW(5,w.control);if(S16(w.control)>5)goto moved;difference=S16(w.secondary)-S16(w.base);SW(secondary,HRR_SECONDARY,w.base);if(difference<0)NEG(secondary,HRR_SECONDARY);CW(5,w.secondary);if(S16(w.secondary)>5)goto moved;
 L(source,HRR_SOURCE,1);logic_long(h,0xc45b50,0x4000,1);logic_long(h,0xc45b50,0xfffffdff,0);W(offset,HRR_OFFSET,0x2700);CB(0x30,w.stride);if((uint8_t)w.stride!=0x30){W(offset,HRR_OFFSET,0x1800);CB(0x20,w.stride);if((uint8_t)w.stride!=0x20)return;}
 CW(rd_u16(w.table+0x4a),w.offset);if(S16(w.offset)<=rd_s16(w.table+0x4a))goto no_alert;TW(rd_u16(0xc459c0));if(rd_s16(0xc459c0)<0)goto no_alert;CW((uint16_t)-675,rd_u16(0xc45b46));if(rd_s16(0xc45b46)<-675)goto low_alert;
 CB(1,rd_u8(0xc458b4));if(rd_u8(0xc458b4)==1){B(offset,HRR_OFFSET,rd_u8(0xc458db));ANDB(offset,HRR_OFFSET,31);CB(9,w.offset);if((uint8_t)w.offset!=9)goto draw;}
 logic_long(h,0xc45b54,4,1);byte(h,0xc458b4,1);goto draw;
low_alert:CB(2,rd_u8(0xc458b4));if(rd_u8(0xc458b4)==2)goto draw;logic_long(h,0xc45b54,16,1);byte(h,0xc458b4,2);goto draw;
no_alert:byte(h,0xc458b4,0);goto sound_gate;
outside:w=consume(h,HRR_CALL_C33ED8);byte(h,0xc458b4,0);L(source,HRR_SOURCE,0);goto draw;
moved:byte(h,0xc458b4,0);L(source,HRR_SOURCE,0);L(size,HRR_SIZE,rd_u32(0xc45b50));L(stride,HRR_STRIDE,w.size);ANDL(size,HRR_SIZE,0x4000);if(w.size)goto reacquire;ANDL(stride,HRR_STRIDE,0x200);if(!w.stride)goto reacquire;
sound_gate:B(offset,HRR_OFFSET,rd_u8(0xc458db));ANDB(offset,HRR_OFFSET,31);CB(9,w.offset);if((uint8_t)w.offset!=9)goto draw;
reacquire:logic_long(h,0xc45b50,0xffffbfff,0);logic_long(h,0xc45b50,0x200,1);logic_long(h,0xc45b54,0x100,1);
draw:word(h,0xc45954,10);w=consume(h,HRR_CALL_C33F40);TB(rd_u8(0xc458b4));if(rd_u8(0xc458b4))w=consume(h,HRR_CALL_C33F4C);
}

void hud_render_panel_mark(HudRenderState w,const HudRenderHooks *h) {
 int difference;
 W(offset,HRR_OFFSET,12);difference=S16(w.offset)-rd_s16(0xc45986);SW(offset,HRR_OFFSET,rd_u16(0xc45986));if(difference<0)return;W(offset,HRR_OFFSET,12);AW(offset,HRR_OFFSET,2);NEG(offset,HRR_OFFSET);AW(offset,HRR_OFFSET,20);difference=S16(w.offset)-rd_s16(0xc45986);SW(offset,HRR_OFFSET,rd_u16(0xc45986));if(difference<0)return;
 L(base,HRR_BASE,0x1990);P(screen,HRR_SCREEN,2);W(size,HRR_SIZE,0x542);P(modulo,HRR_MODULO,37);W(offset,HRR_OFFSET,12);AL(base,HRR_BASE,rd_u32(0xc45918));w=consume(h,HRR_CALL_C30078);if(w.less)return;TW(w.stride);W(secondary,HRR_SECONDARY,(uint16_t)w.stride?0xffff:0xfff0);TW(w.offset);W(value,HRR_VALUE,(uint16_t)w.offset?0xffff:0x0fff);AW(stride,HRR_STRIDE,w.offset);L(offset,HRR_OFFSET,0x12a88);P(stream,HRR_STREAM,rd_u32(0xc456b6));L(source,HRR_SOURCE,rd_u32(w.stream+4));AL(source,HRR_SOURCE,w.base);SW(size,HRR_SIZE,w.stride);AW(stride,HRR_STRIDE,w.stride);AW(stride,HRR_STRIDE,w.modulo);P(registers,HRR_REGISTERS,0xdff000);W(control,HRR_CONTROL,0x722);blitter_ready(h,w.registers,0xc300c0);
 word(h,w.registers+0x40,(uint16_t)w.control);word(h,w.registers+0x42,0);word(h,w.registers+0x74,0xffff);word(h,w.registers+0x44,(uint16_t)w.value);word(h,w.registers+0x46,(uint16_t)w.secondary);word(h,w.registers+0x62,1);word(h,w.registers+0x60,(uint16_t)w.stride);word(h,w.registers+0x66,(uint16_t)w.stride);longword(h,w.registers+0x4c,w.offset);longword(h,w.registers+0x48,w.source);longword(h,w.registers+0x54,w.source);word(h,w.registers+0x58,(uint16_t)w.size);w=consume(h,HRR_CALL_C30104);
 longword(h,0xc456e6,0x7ffff);W(value,HRR_VALUE,204);AW(value,HRR_VALUE,rd_u16(0xc45988));W(base,HRR_BASE,172);AW(base,HRR_BASE,rd_u16(0xc458d8));W(control,HRR_CONTROL,w.value);AW(control,HRR_CONTROL,10);W(secondary,HRR_SECONDARY,w.base);word(h,0xc45954,2);w=consume(h,HRR_CALL_C30136);W(value,HRR_VALUE,209);W(base,HRR_BASE,171);w=consume(h,HRR_CALL_C30142);W(base,HRR_BASE,172);w=consume(h,HRR_CALL_C3014A);word(h,0xc45954,13);W(value,HRR_VALUE,206);W(base,HRR_BASE,165);w=consume(h,HRR_CALL_C3015E);
 SW(value,HRR_VALUE,2);AW(base,HRR_BASE,1);w=consume(h,HRR_CALL_C30166);SW(value,HRR_VALUE,1);AW(base,HRR_BASE,1);w=consume(h,HRR_CALL_C3016E);SW(value,HRR_VALUE,2);AW(base,HRR_BASE,3);w=consume(h,HRR_CALL_C30176);AW(value,HRR_VALUE,17);AW(base,HRR_BASE,1);w=consume(h,HRR_CALL_C30180);AW(base,HRR_BASE,1);w=consume(h,HRR_CALL_C30186);AW(base,HRR_BASE,1);w=consume(h,HRR_CALL_C3018C);SW(value,HRR_VALUE,1);AW(base,HRR_BASE,2);w=consume(h,HRR_CALL_C30194);
}

/* Source seek step, used for X and Y with the same signed limits. */
#define SEEK_AXIS(f,id,target) do { \
 int diff_=S16(w.f)-S16(target);SW(f,id,target); \
 if(diff_<0){CW((uint16_t)-15,w.f);if(S16(w.f)<-15)ASR(f,id,w.size);else {ASR(f,id,w.offset);CW((uint16_t)-5,w.f);if(S16(w.f)>=-5)AW(stride,HRR_STRIDE,1);}CW(1,w.offset);if(S16(w.offset)!=1){CW((uint16_t)-1,w.f);if(S16(w.f)<-1)L(f,id,0xffffffffu);}else {CW((uint16_t)-2,w.f);if(S16(w.f)<-2)L(f,id,0xfffffffeu);}} \
 else {CW(15,w.f);if(S16(w.f)>15)ASR(f,id,w.size);else {ASR(f,id,w.offset);CW(5,w.f);if(S16(w.f)<=5)AW(stride,HRR_STRIDE,1);}CW(1,w.offset);if(S16(w.offset)!=1){CW(1,w.f);if(S16(w.f)>1)L(f,id,1);}else {CW(2,w.f);if(S16(w.f)>2)L(f,id,2);}} \
 }while(0)
void hud_render_target_box(HudRenderState w,const HudRenderHooks *h) {
 int sum;
 word(h,0xc459aa,0);word(h,0xc45954,10);
 for(;;){
  w=load_words(w,h,0xc45936,3);TB(rd_u8(0xc457ae));if(rd_u8(0xc457ae))goto jitter;
  w=load_words(w,h,0xc45942,12);L(stride,HRR_STRIDE,0);P(table,HRR_TABLE,0xc46184);P(table,HRR_TABLE,w.table+SADDR(rd_u16(0xc458de)));B(source,HRR_SOURCE,rd_u8(w.table+0x63));ANDB(source,HRR_SOURCE,0xf0);if(!(uint8_t)w.source)goto absent;TW(w.value);if(S16(w.value)<=0)goto absent;TW(w.control);if(S16(w.control)<=0){W(source,HRR_SOURCE,159);W(control,HRR_CONTROL,91);goto publish;}
  L(size,HRR_SIZE,rd_u32(0xc45b50));ANDL(size,HRR_SIZE,0x4000);if(!w.size){L(size,HRR_SIZE,4);L(offset,HRR_OFFSET,2);}else {L(size,HRR_SIZE,2);L(offset,HRR_OFFSET,1);}
  W(source,HRR_SOURCE,w.control);SEEK_AXIS(control,HRR_CONTROL,w.value);SW(source,HRR_SOURCE,w.control);W(control,HRR_CONTROL,w.secondary);SEEK_AXIS(secondary,HRR_SECONDARY,w.base);SW(control,HRR_CONTROL,w.secondary);goto publish;
absent:W(source,HRR_SOURCE,0xffff);
publish:word(h,0xc45942,(uint16_t)w.source);word(h,0xc45944,(uint16_t)w.control);
jitter:TW(rd_u16(0xc461da));if(rd_u16(0xc461da)){W(control,HRR_CONTROL,rd_u16(0xc4619a));ANDW(control,HRR_CONTROL,0x1c);P(registers,HRR_REGISTERS,0xc34540);AW(value,HRR_VALUE,rd_u16(w.registers+SADDR(w.control)));AW(base,HRR_BASE,rd_u16(w.registers+2+SADDR(w.control)));}
  AW(value,HRR_VALUE,rd_u16(0xc45988));AW(base,HRR_BASE,rd_u16(0xc458d8));W(control,HRR_CONTROL,w.value);if(S16(w.control)<=0)return;W(secondary,HRR_SECONDARY,w.base);if(S16(w.secondary)<=0)return;P(registers,HRR_REGISTERS,0xc3458c);P(registers,HRR_REGISTERS,w.registers+SADDR(rd_u16(0xc459aa)));
  sum=S16(w.value)+rd_s16(w.registers);AW(value,HRR_VALUE,rd_u16(w.registers));if(sum<0)W(value,HRR_VALUE,0);CW(319,w.value);if(S16(w.value)>319)W(value,HRR_VALUE,319);sum=S16(w.control)+rd_s16(w.registers+4);AW(control,HRR_CONTROL,rd_u16(w.registers+4));if(sum<0)W(control,HRR_CONTROL,0);CW(319,w.control);if(S16(w.control)>319)W(control,HRR_CONTROL,319);AW(base,HRR_BASE,rd_u16(w.registers+2));AW(secondary,HRR_SECONDARY,rd_u16(w.registers+6));AW(base,HRR_BASE,rd_u16(0xc458d8));AW(secondary,HRR_SECONDARY,rd_u16(0xc458d8));
  W(source,HRR_SOURCE,rd_u16(0xc34560));AW(source,HRR_SOURCE,1);AW(source,HRR_SOURCE,rd_u16(0xc45988));CW(319,w.source);if(S16(w.source)>319)goto next_line;CW(w.source,w.value);if(S16(w.value)<S16(w.source)){CW(w.value,w.control);if(S16(w.control)==S16(w.value)){TW(rd_u16(w.registers));if(rd_s16(w.registers)<0)goto next_line;}W(value,HRR_VALUE,w.source);}CW(w.source,w.control);if(S16(w.control)<S16(w.source))W(control,HRR_CONTROL,w.source);
  W(source,HRR_SOURCE,rd_u16(0xc34570));SW(source,HRR_SOURCE,1);sum=S16(w.source)+rd_s16(0xc45988);AW(source,HRR_SOURCE,rd_u16(0xc45988));if(sum<0)goto next_line;CW(w.source,w.value);if(S16(w.value)>S16(w.source)){CW(w.value,w.control);if(S16(w.control)==S16(w.value)){TW(rd_u16(w.registers));if(rd_s16(w.registers)>=0)goto next_line;}W(value,HRR_VALUE,w.source);}CW(w.source,w.control);if(S16(w.control)>S16(w.source))W(control,HRR_CONTROL,w.source);
  W(source,HRR_SOURCE,rd_u16(0xc34566));AW(source,HRR_SOURCE,rd_u16(0xc458d8));CW(w.source,w.base);if(S16(w.base)<S16(w.source)){CW(w.base,w.secondary);if(S16(w.secondary)==S16(w.base)){TW(rd_u16(w.registers+2));if(rd_s16(w.registers+2)<0)goto next_line;}W(base,HRR_BASE,w.source);}CW(w.source,w.secondary);if(S16(w.secondary)<S16(w.source))W(secondary,HRR_SECONDARY,w.source);
  W(source,HRR_SOURCE,rd_u16(0xc34562));AW(source,HRR_SOURCE,rd_u16(0xc458d8));CW(w.source,w.base);if(S16(w.base)>S16(w.source))W(base,HRR_BASE,w.source);CW(w.source,w.secondary);if(S16(w.secondary)>S16(w.source))W(secondary,HRR_SECONDARY,w.source);w=consume(h,HRR_CALL_C34514);
next_line:increment_word(h,0xc459aa,4);CW(16,rd_u16(0xc459aa));if(rd_s16(0xc459aa)>=16)break;
 }
 longword(h,0xc4593a,rd_u32(0xc45936));word(h,0xc45936,0xffff);
}

static HudRenderState copy_string(HudRenderState w,const HudRenderHooks *h) {
 for(;;){B(base,HRR_BASE,rd_u8(w.table));P(table,HRR_TABLE,w.table+1);if(!(uint8_t)w.base)return w;byte(h,w.stream,(uint8_t)w.base);P(stream,HRR_STREAM,w.stream+1);}
}
static HudRenderState copy_message(HudRenderState w,const HudRenderHooks *h) {
 L(base,HRR_BASE,26);do {byte(h,w.stream,rd_u8(w.table));P(table,HRR_TABLE,w.table+1);P(stream,HRR_STREAM,w.stream+1);}while(dbra(&w.base,h,HRR_BASE));return w;
}
/* C32376..C32380: the original information-page delay branch. */
int hud_render_page_delay(const HudRenderHooks *h) {
 uint8_t old=rd_u8(0xc4583c);decrement_byte(h,0xc4583c);return (int8_t)old-1>=0;
}
static void message_text_tail(HudRenderState w,const HudRenderHooks *h,int below) {
 if(below){SWAP(size,HRR_SIZE);W(size,HRR_SIZE,0);L(offset,HRR_OFFSET,0);}
 P(screen,HRR_SCREEN,w.screen+w.offset);W(offset,HRR_OFFSET,322);P(registers,HRR_REGISTERS,rd_u32(0xc456b6));L(base,HRR_BASE,rd_u32(w.registers+SADDR(w.stride)));AW(size,HRR_SIZE,w.size);
 do {
  int sum;
  W(stride,HRR_STRIDE,rd_u16(w.table));P(table,HRR_TABLE,w.table+2);W(secondary,HRR_SECONDARY,rd_u16(w.table));P(table,HRR_TABLE,w.table+2);B(source,HRR_SOURCE,rd_u8(w.stream));P(stream,HRR_STREAM,w.stream+1);W(control,HRR_CONTROL,w.modulo);AW(control,HRR_CONTROL,w.size);sum=S16(w.control)+S16(w.stride);AW(control,HRR_CONTROL,w.stride);if(sum<0)goto next_cell;CW(40,w.control);if(S16(w.control)>=40)goto next_cell;
  AW(stride,HRR_STRIDE,w.size);EL(stride,HRR_STRIDE);AL(stride,HRR_STRIDE,w.screen);AL(stride,HRR_STRIDE,w.base);ANDW(source,HRR_SOURCE,255);SW(source,HRR_SOURCE,32);AW(source,HRR_SOURCE,w.source);P(descriptor,HRR_DESCRIPTOR,0xc3d790);P(descriptor,HRR_DESCRIPTOR,w.descriptor+SADDR(rd_u16(w.descriptor+SADDR(w.source))));L(source,HRR_SOURCE,w.descriptor);SWAP(size,HRR_SIZE);W(control,HRR_CONTROL,w.size);SWAP(size,HRR_SIZE);ORW(control,HRR_CONTROL,w.secondary);observe(h,HRR_BIT_TEST,HRR_STRIDE,w.stride,0);
  if(!(w.stride&1)){w=push_long(w,h,w.base);L(base,HRR_BASE,w.stride);w=consume(h,HRR_CALL_C327EA);observe(h,HRR_POP_LONG,HRR_BASE,0,0);w=restored(h);}
  else {word(h,0xc4599e,70);w=consume(h,HRR_CALL_C327FE);}
next_cell:;
 }while(dbra(&w.value,h,HRR_VALUE));
}

/* C322EE: complete information pages, message copies and text-plane tails. */
void hud_render_message_line(HudRenderState w,const HudRenderHooks *h) {
 int difference;uint32_t oldbit;
 W(control,HRR_CONTROL,rd_u16(0xc459c0));if(S16(w.control)<0)goto message;W(secondary,HRR_SECONDARY,rd_u16(0xc458cc));ANDW(secondary,HRR_SECONDARY,0x81);if((uint16_t)w.secondary)goto message;
 {uint8_t old=rd_u8(0xc45887);decrement_byte(h,0xc45887);if((int8_t)old-1>=0)goto message;}byte(h,0xc45887,0xff);P(table,HRR_TABLE,0xc46184);TB(rd_u8(0xc4583c));if(rd_s8(0xc4583c)>0)goto page;
 TB(rd_u8(0xc45886));if(rd_s8(0xc45886)<0){byte(h,0xc45886,1);W(value,HRR_VALUE,rd_u16(0xc459c4));AW(value,HRR_VALUE,1);CW(3,w.value);if(S16(w.value)>3)L(value,HRR_VALUE,1);}
 else {if(!bit(h,0xc45886,0))return;W(value,HRR_VALUE,rd_u16(0xc459c4));}
 observe(h,HRR_BIT_SET,HRR_VALUE,w.value,15);w.value|=0x8000;word(h,0xc459c4,(uint16_t)w.value);byte(h,0xc4583c,2);
page:W(value,HRR_VALUE,rd_u16(0xc459c4));oldbit=w.value&0x8000;observe(h,HRR_BIT_CLEAR,HRR_VALUE,w.value,15);w.value&=~0x8000u;
 if(!oldbit){if(hud_render_page_delay(h))goto draw;return;}
 word(h,0xc459c4,(uint16_t)w.value);P(stream,HRR_STREAM,0xc4580a);P(table,HRR_TABLE,0xc326b8);w=copy_string(w,h);logic_byte(h,0xc45862,0x3f,0);logic_byte(h,0xc45862,0x80,1);P(screen,HRR_SCREEN,0xc46184);P(screen,HRR_SCREEN,w.screen+SADDR(w.control));
 if(!bit(h,w.screen+0x20,6)){P(table,HRR_TABLE,0xc32705);logic_byte(h,0xc45862,0x3f,0);goto record_text;}
 B(base,HRR_BASE,rd_u8(w.screen+0x62));CB(18,w.base);if((uint8_t)w.base==18){P(table,HRR_TABLE,0xc326e2);goto record_text;}CB(19,w.base);if((uint8_t)w.base==19){P(table,HRR_TABLE,0xc326e9);goto record_text;}CB(20,w.base);if((uint8_t)w.base==20){P(table,HRR_TABLE,0xc326f0);goto type40;}CB(22,w.base);if((uint8_t)w.base==22){P(table,HRR_TABLE,0xc3270c);goto type40;}CB(23,w.base);if((uint8_t)w.base==23){P(table,HRR_TABLE,0xc32713);goto type40;}CB(16,w.base);if((uint8_t)w.base==16){P(table,HRR_TABLE,0xc326f7);goto type40;}CB(21,w.base);if((uint8_t)w.base!=21)goto number;P(table,HRR_TABLE,0xc326fe);logic_byte(h,0xc45862,0x3f,0);logic_byte(h,0xc45862,0x80,1);goto record_text;
type40:logic_byte(h,0xc45862,0x3f,0);logic_byte(h,0xc45862,0x40,1);
record_text:P(stream,HRR_STREAM,0xc4580e);w=copy_string(w,h);
number:P(stream,HRR_STREAM,0xc45817);SW(value,HRR_VALUE,1);if((uint16_t)w.value)goto number2;P(table,HRR_TABLE,0xc326d3);w=copy_string(w,h);L(value,HRR_VALUE,rd_u32(w.screen+0x18));ARL(value,HRR_VALUE,7);ARL(value,HRR_VALUE,3);L(base,HRR_BASE,w.value);AL(value,HRR_VALUE,w.value);AL(value,HRR_VALUE,w.value);AL(value,HRR_VALUE,w.base);P(stream,HRR_STREAM,0xc45821);L(control,HRR_CONTROL,4);L(source,HRR_SOURCE,0);w=consume(h,HRR_CALL_C32496);goto draw;
number2:SW(value,HRR_VALUE,1);if((uint16_t)w.value)goto number3;P(table,HRR_TABLE,0xc326d8);w=copy_string(w,h);W(value,HRR_VALUE,rd_u16(w.screen+0x68));EL(value,HRR_VALUE);ARL(value,HRR_VALUE,3);DU(value,HRR_VALUE,10);EL(value,HRR_VALUE);P(stream,HRR_STREAM,0xc45820);L(control,HRR_CONTROL,2);L(source,HRR_SOURCE,1);w=consume(h,HRR_CALL_C324C8);goto draw;
number3:SW(value,HRR_VALUE,1);if((uint16_t)w.value)goto draw;P(table,HRR_TABLE,0xc326dd);w=copy_string(w,h);L(value,HRR_VALUE,0);if(!bit(h,w.screen,7)){W(value,HRR_VALUE,rd_u16(w.screen+0x6e));if(S16(w.value)<0)NEG(value,HRR_VALUE);}EL(value,HRR_VALUE);DU(value,HRR_VALUE,12);EL(value,HRR_VALUE);P(stream,HRR_STREAM,0xc45820);L(control,HRR_CONTROL,3);L(source,HRR_SOURCE,0);w=consume(h,HRR_CALL_C32508);goto draw;
message:TW(rd_u16(0xc459c4));if(rd_s16(0xc459c4)>0){word(h,0xc459c4,0xffff);goto empty_message;}if(rd_s16(0xc459c4)<0){word(h,0xc459c4,0);goto empty_message;}
 W(value,HRR_VALUE,rd_u16(0xc45ade));CW(rd_u16(0xc45ae4),w.value);if((uint16_t)w.value==rd_u16(0xc45ae4)){TB(rd_u8(0xc4583c));if(rd_s8(0xc4583c)<=0)return;decrement_byte(h,0xc4583c);goto draw;}
 word(h,0xc45ae4,(uint16_t)w.value);ANDW(value,HRR_VALUE,255);AW(value,HRR_VALUE,w.value);AW(value,HRR_VALUE,w.value);W(base,HRR_BASE,w.value);AW(value,HRR_VALUE,w.value);AW(base,HRR_BASE,w.value);AW(value,HRR_VALUE,w.value);AW(base,HRR_BASE,w.value);AW(base,HRR_BASE,1);P(table,HRR_TABLE,0xc3d0a0);P(table,HRR_TABLE,w.table+SADDR(w.base));P(stream,HRR_STREAM,0xc4580a);w=copy_message(w,h);byte(h,0xc4583c,2);goto draw;
empty_message:P(table,HRR_TABLE,0xc3d0a0);P(table,HRR_TABLE,w.table+1);P(stream,HRR_STREAM,0xc4580a);w=copy_message(w,h);
draw:P(stream,HRR_STREAM,0xc4580a);P(table,HRR_TABLE,0xc31998);P(screen,HRR_SCREEN,0x1e0c);P(modulo,HRR_MODULO,12);L(value,HRR_VALUE,25);W(size,HRR_SIZE,0xfca);SWAP(size,HRR_SIZE);W(size,HRR_SIZE,rd_u16(0xc45986));L(offset,HRR_OFFSET,rd_u32(0xc45918));w=save_longs(w,h,0x836c);
 B(secondary,HRR_SECONDARY,rd_u8(0xc45862));{uint8_t b=(uint8_t)w.secondary;w.secondary=low_byte(w.secondary,(uint8_t)((b<<2)|(b>>6)));observe(h,HRR_ROL_BYTE,HRR_SECONDARY,2,0);}ANDB(secondary,HRR_SECONDARY,3);if(!(uint8_t)w.secondary){W(stride,HRR_STRIDE,0);w=consume(h,HRR_CALL_C325F8);W(stride,HRR_STRIDE,12);W(secondary,HRR_SECONDARY,4);}
 else {SB(secondary,HRR_SECONDARY,1);if(!(uint8_t)w.secondary){W(stride,HRR_STRIDE,4);w=consume(h,HRR_CALL_C3260A);W(stride,HRR_STRIDE,12);W(secondary,HRR_SECONDARY,0);}else {W(stride,HRR_STRIDE,12);w=consume(h,HRR_CALL_C325E6);W(stride,HRR_STRIDE,4);W(secondary,HRR_SECONDARY,0);}}
 w=restore_longs(w,h,0x36c1);TB(rd_u8(0xc45785));if(rd_u8(0xc45785)){TB(rd_u8(0xc45793));if(!rd_u8(0xc45793))return;P(screen,HRR_SCREEN,0x1b14);P(modulo,HRR_MODULO,12);W(stride,HRR_STRIDE,12);W(size,HRR_SIZE,0xf3a);message_text_tail(w,h,1);return;}
 TB(rd_u8(0xc45861));if(rd_s8(0xc45861)<0)return;decrement_byte(h,0xc45861);SWAP(size,HRR_SIZE);W(size,HRR_SIZE,0xf0a);SWAP(size,HRR_SIZE);w=save_longs(w,h,0x936c);w=consume(h,HRR_CALL_C32658);w=restore_longs(w,h,0x36c9);W(stride,HRR_STRIDE,w.secondary);TB(rd_u8(0xc45785));if(rd_u8(0xc45785)){TB(rd_u8(0xc45793));if(!rd_u8(0xc45793))return;}message_text_tail(w,h,0);
 (void)difference;
}
