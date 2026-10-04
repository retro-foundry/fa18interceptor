#include "render_parents.h"
#include <stdlib.h>
static void observe(const RenderParentHooks *h,enum RenderParentPhase p,enum RenderParentField f,uint32_t v,uint32_t o) {
    if(h && h->observe) h->observe(h->context,p,f,v,o);
}
static RenderParentState consume(const RenderParentHooks *h,enum RenderParentChild child) {
    if(h && h->consume) return h->consume(h->context,child); abort();
}
static RenderParentState restored(const RenderParentHooks *h) {
    if(h && h->restored) return h->restored(h->context); abort();
}
static uint32_t low_word(uint32_t old,uint16_t v) { return (old&0xffff0000u)|v; }
static uint32_t low_byte(uint32_t old,uint8_t v) { return (old&0xffffff00u)|v; }
#define W(f,id,v) do { w.f=low_word(w.f,(uint16_t)(v)); observe(h,RDP_WORD,id,w.f,0); } while(0)
#define B(f,id,v) do { w.f=low_byte(w.f,(uint8_t)(v)); observe(h,RDP_BYTE,id,w.f,0); } while(0)
#define L(f,id,v) do { w.f=(uint32_t)(v); observe(h,RDP_LONG,id,w.f,0); } while(0)
#define P(f,id,v) do { w.f=(gaddr)(v); observe(h,RDP_POINTER,id,w.f,0); } while(0)
#define AW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f+n_)); observe(h,RDP_ADD_WORD,id,n_,0); } while(0)
#define SW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f-n_)); observe(h,RDP_SUB_WORD,id,n_,0); } while(0)
#define AL(f,id,v) do { uint32_t n_=(uint32_t)(v); w.f+=n_; observe(h,RDP_ADD_LONG,id,n_,0); } while(0)
#define SL(f,id,v) do { uint32_t n_=(uint32_t)(v); w.f-=n_; observe(h,RDP_SUB_LONG,id,n_,0); } while(0)
#define ANDW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f&n_)); observe(h,RDP_AND_WORD,id,n_,0); } while(0)
#define ANDB(f,id,v) do { uint8_t n_=(uint8_t)(v); w.f=low_byte(w.f,(uint8_t)(w.f&n_)); observe(h,RDP_AND_BYTE,id,n_,0); } while(0)
#define ORW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f|n_)); observe(h,RDP_OR_WORD,id,n_,0); } while(0)
#define EL(f,id) do { w.f=(uint32_t)(int32_t)(int16_t)w.f; observe(h,RDP_EXT_LONG,id,0,0); } while(0)
#define NEG(f,id) do { w.f=low_word(w.f,(uint16_t)(0u-w.f)); observe(h,RDP_NEG_WORD,id,0,0); } while(0)
#define SWAP(f,id) do { w.f=(w.f<<16)|(w.f>>16); observe(h,RDP_SWAP,id,0,0); } while(0)
#define ASL(f,id,n) do { w.f=low_word(w.f,(uint16_t)((((unsigned)(n)&63u)<16?w.f<<((unsigned)(n)&63u):0))); observe(h,RDP_ASL_WORD,id,n,0); } while(0)
#define LSL(f,id,n) do { w.f=low_word(w.f,(uint16_t)((((unsigned)(n)&63u)<16?w.f<<((unsigned)(n)&63u):0))); observe(h,RDP_LSL_WORD,id,n,0); } while(0)
#define ASR(f,id,n) do { unsigned n_=(unsigned)(n)&63u; w.f=low_word(w.f,(uint16_t)((int16_t)w.f>>(n_<16?n_:15))); observe(h,RDP_ASR_WORD,id,n_,0); } while(0)
#define CW(v,n) observe(h,RDP_COMPARE_WORD,RDP_VALUE,(uint16_t)(n),(uint16_t)(v))
#define CB(v,n) observe(h,RDP_COMPARE_BYTE,RDP_VALUE,(uint8_t)(n),(uint8_t)(v))
#define TW(v) observe(h,RDP_TEST_WORD,RDP_VALUE,(uint16_t)(v),0)
#define TB(v) observe(h,RDP_TEST_BYTE,RDP_VALUE,(uint8_t)(v),0)
static void word(const RenderParentHooks *h,gaddr a,uint16_t v) { wr_u16(a,v); observe(h,RDP_STORE_WORD,RDP_VALUE,v,0); }
static void byte(const RenderParentHooks *h,gaddr a,uint8_t v) { wr_u8(a,v); observe(h,RDP_STORE_BYTE,RDP_VALUE,v,0); }
static void longword(const RenderParentHooks *h,gaddr a,uint32_t v) { wr_u32(a,v); observe(h,RDP_STORE_LONG,RDP_VALUE,v,0); }
static void decrement_byte(const RenderParentHooks *h,gaddr a) { uint8_t old=rd_u8(a); wr_u8(a,(uint8_t)(old-1)); observe(h,RDP_MEMORY_SUB_BYTE,RDP_VALUE,old,1); }
static int bit(const RenderParentHooks *h,gaddr a,unsigned b) { uint8_t v=rd_u8(a); observe(h,RDP_BIT_TEST,RDP_VALUE,v,b); return (v>>b)&1; }

/* MOVEM transfers retain CCR; individual MOVE transfers above update it. */
#define RAW(f,id,v) do { w.f=(uint32_t)(v); observe(h,RDP_RAW_LONG,id,w.f,0); } while(0)
#define MUL(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=(uint32_t)((int32_t)(int16_t)w.f*(int16_t)n_); observe(h,RDP_MULS,id,n_,0); } while(0)
#define MUP(f,id,ptr,pi) do { MUL(f,id,rd_u16(w.ptr)); P(ptr,pi,w.ptr+2); } while(0)
#define ARL(f,id,n) do { unsigned n_=(unsigned)(n)&63u; w.f=(uint32_t)((int32_t)w.f>>(n_<32?n_:31)); observe(h,RDP_ASR_LONG,id,n_,0); } while(0)
#define DU(f,id,n) do { uint16_t n_=(uint16_t)(n); if(n_ && w.f/n_<=65535u) w.f=(w.f%n_)<<16|(w.f/n_); observe(h,RDP_DIVU,id,n_,0); } while(0)

#define AB(f,id,v) do { uint8_t n_=(uint8_t)(v); w.f=low_byte(w.f,(uint8_t)(w.f+n_)); observe(h,RDP_ADD_BYTE,id,n_,0); } while(0)
#define SB(f,id,v) do { uint8_t n_=(uint8_t)(v); w.f=low_byte(w.f,(uint8_t)(w.f-n_)); observe(h,RDP_SUB_BYTE,id,n_,0); } while(0)
#define EW(f,id) do { w.f=low_word(w.f,(uint16_t)(int16_t)(int8_t)w.f); observe(h,RDP_EXT_WORD,id,0,0); } while(0)
#define NL(f,id) do { w.f=0u-w.f; observe(h,RDP_NEG_LONG,id,0,0); } while(0)
#define CL(v,n) observe(h,RDP_COMPARE_LONG,RDP_VALUE,(uint32_t)(n),(uint32_t)(v))
#define TL(v) observe(h,RDP_TEST_LONG,RDP_VALUE,(uint32_t)(v),0)
static int memory_decrement(const RenderParentHooks *h,gaddr at) {
    uint8_t v=rd_u8(at); wr_u8(at,(uint8_t)(v-1)); observe(h,RDP_MEMORY_SUB_BYTE,RDP_VALUE,v,1); return (int8_t)v-1>=0;
}
static void memory_increment(const RenderParentHooks *h,gaddr at) {
    uint8_t v=rd_u8(at);wr_u8(at,(uint8_t)(v+1));observe(h,RDP_MEMORY_ADD_BYTE,RDP_VALUE,v,1);
}
static RenderParentState load_words(RenderParentState w,const RenderParentHooks *h,gaddr at,uint16_t mask) {
    (void)w;observe(h,RDP_LOAD_WORDS_AT,RDP_VALUE,at,mask);return restored(h);
}
static RenderParentState load_longs(RenderParentState w,const RenderParentHooks *h,gaddr at,uint16_t mask) {
    (void)w;observe(h,RDP_LOAD_LONGS_AT,RDP_VALUE,at,mask);return restored(h);
}
static void store_words(const RenderParentHooks *h,gaddr at,uint16_t mask) { observe(h,RDP_STORE_WORDS_AT,RDP_VALUE,at,mask); }
static void store_longs(const RenderParentHooks *h,gaddr at,uint16_t mask) { observe(h,RDP_STORE_LONGS_AT,RDP_VALUE,at,mask); }
static int decrement_word(const RenderParentHooks *h,gaddr at) {
 uint16_t old=rd_u16(at);wr_u16(at,(uint16_t)(old-1));observe(h,RDP_MEMORY_SUB_WORD,RDP_VALUE,old,1);return (int16_t)old-1;
}
#define ALL(f,id,n) do { unsigned n_=(unsigned)(n)&63u; w.f=n_<32?w.f<<n_:0;observe(h,RDP_ASL_LONG,id,n_,0); } while(0)
static RenderParentState save_longs(RenderParentState w,const RenderParentHooks *h,uint16_t mask) { observe(h,RDP_SAVE_LONGS,RDP_VALUE,mask,0);return restored(h); }
static RenderParentState restore_longs(RenderParentState w,const RenderParentHooks *h,uint16_t mask) { observe(h,RDP_RESTORE_LONGS,RDP_VALUE,mask,0);return restored(h); }
static RenderParentState matrix_row(RenderParentState w,const RenderParentHooks *h,gaddr row) {
 W(stride,RDP_STRIDE,w.control);W(size,RDP_SIZE,w.secondary);W(offset,RDP_OFFSET,w.source);
 MUL(stride,RDP_STRIDE,rd_u16(row));MUL(size,RDP_SIZE,rd_u16(row+2));MUL(offset,RDP_OFFSET,rd_u16(row+4));
 AL(offset,RDP_OFFSET,w.size);AL(offset,RDP_OFFSET,w.stride);ARL(offset,RDP_OFFSET,8);return w;
}

/* C1FB82: original face side selector, vector plane and axis-plane tests. */
void render_face_side(RenderParentState w,const RenderParentHooks *h) {
 int64_t sum;int smaller,axis;
 W(base,RDP_BASE,w.offset);ANDW(base,RDP_BASE,0x0c00);
 if(!(uint16_t)w.base) {
  ANDW(offset,RDP_OFFSET,0x3000);
  if((uint16_t)w.offset) {
   P(registers,RDP_REGISTERS,rd_u32(w.frame-0x2c));P(registers,RDP_REGISTERS,w.registers+(gaddr)(int32_t)rd_s16(w.stream));P(stream,RDP_STREAM,w.stream+2);
   w=load_words(w,h,w.registers,0x003f);W(offset,RDP_OFFSET,rd_u16(0xc45ab8));
   ASR(value,RDP_VALUE,w.offset);ASR(base,RDP_BASE,w.offset);ASR(control,RDP_CONTROL,w.offset);
   AW(value,RDP_VALUE,rd_u16(0xc45b2a));AW(control,RDP_CONTROL,rd_u16(0xc45b2e));
   SW(value,RDP_VALUE,rd_u16(w.frame-0x26));SW(base,RDP_BASE,rd_u16(w.frame-0x24));SW(control,RDP_CONTROL,rd_u16(w.frame-0x22));
   MUL(value,RDP_VALUE,w.secondary);MUL(base,RDP_BASE,w.source);MUL(control,RDP_CONTROL,w.stride);
   AL(control,RDP_CONTROL,w.value);sum=(int64_t)(int32_t)w.control+(int32_t)w.base;AL(control,RDP_CONTROL,w.base);
   if(sum<0)W(offset,RDP_OFFSET,0);else L(offset,RDP_OFFSET,1);return;
  }
  P(registers,RDP_REGISTERS,0xc4bf94);w=load_words(w,h,w.registers,0x003f);
  SW(secondary,RDP_SECONDARY,w.value);SW(source,RDP_SOURCE,w.base);SW(stride,RDP_STRIDE,w.control);
  w=load_words(w,h,w.registers+12,0x00c0);SW(size,RDP_SIZE,w.value);SW(offset,RDP_OFFSET,w.base);
  W(value,RDP_VALUE,rd_u16(w.registers+16));SW(value,RDP_VALUE,w.control);
  W(base,RDP_BASE,w.descriptor);ASR(base,RDP_BASE,7);ANDW(base,RDP_BASE,7);
  if((uint16_t)w.base) {ASL(secondary,RDP_SECONDARY,w.base);ASL(source,RDP_SOURCE,w.base);ASL(stride,RDP_STRIDE,w.base);ASL(size,RDP_SIZE,w.base);ASL(offset,RDP_OFFSET,w.base);ASL(value,RDP_VALUE,w.base);}
  W(base,RDP_BASE,w.stride);W(control,RDP_CONTROL,w.value);MUL(value,RDP_VALUE,w.source);MUL(stride,RDP_STRIDE,w.offset);SL(value,RDP_VALUE,w.stride);ARL(value,RDP_VALUE,8);
  MUL(control,RDP_CONTROL,w.secondary);MUL(base,RDP_BASE,w.size);SL(base,RDP_BASE,w.control);ARL(base,RDP_BASE,8);
  MUL(offset,RDP_OFFSET,w.secondary);MUL(size,RDP_SIZE,w.source);SL(offset,RDP_OFFSET,w.size);ARL(offset,RDP_OFFSET,8);
  MUP(value,RDP_VALUE,registers,RDP_REGISTERS);MUP(base,RDP_BASE,registers,RDP_REGISTERS);MUL(offset,RDP_OFFSET,rd_u16(w.registers));
  AL(offset,RDP_OFFSET,w.value);sum=(int64_t)(int32_t)w.offset+(int32_t)w.base;AL(offset,RDP_OFFSET,w.base);
  if(sum<0)W(offset,RDP_OFFSET,0);else L(offset,RDP_OFFSET,1);return;
 }
 W(value,RDP_VALUE,rd_u16(w.stream-4));ANDW(value,RDP_VALUE,0x3fff);P(registers,RDP_REGISTERS,rd_u32(0xc45a32));
 W(base,RDP_BASE,w.offset);ANDW(offset,RDP_OFFSET,0x0c00);ASR(offset,RDP_OFFSET,8);ASR(offset,RDP_OFFSET,2);SW(offset,RDP_OFFSET,1);
 if(!(uint16_t)w.offset) {
  B(stride,RDP_STRIDE,rd_u8(w.registers+6));ANDW(stride,RDP_STRIDE,15);W(control,RDP_CONTROL,rd_u16(w.registers+12+(gaddr)(int32_t)(int16_t)w.value));ASR(control,RDP_CONTROL,w.stride);
  L(offset,RDP_OFFSET,rd_u32(0xc45a78));NL(offset,RDP_OFFSET);EL(control,RDP_CONTROL);CL(w.control,w.offset);smaller=(int32_t)w.offset<(int32_t)w.control;
 } else {
  SW(offset,RDP_OFFSET,1);axis=(uint16_t)w.offset!=0;
  B(stride,RDP_STRIDE,rd_u8(w.registers+6));ANDW(stride,RDP_STRIDE,15);
  W(control,RDP_CONTROL,rd_u16(w.registers+(axis?14u:10u)+(gaddr)(int32_t)(int16_t)w.value));ASR(control,RDP_CONTROL,w.stride);
  W(offset,RDP_OFFSET,rd_u16(axis?0xc45a76u:0xc45a72u));NEG(offset,RDP_OFFSET);
  W(size,RDP_SIZE,rd_u16(axis?0xc45b2eu:0xc45b2au));
  /* The axis selection above is decided before MOVE replaces D7. */
  W(stride,RDP_STRIDE,rd_u16(0xc45ab8));ASL(size,RDP_SIZE,w.stride);AW(control,RDP_CONTROL,w.size);
  CW(w.control,w.offset);smaller=(int16_t)w.offset<(int16_t)w.control;
 }
 observe(h,RDP_BIT_TEST,RDP_BASE,w.base,12);
 if(smaller) {if(w.base&0x1000u)W(offset,RDP_OFFSET,0);else L(offset,RDP_OFFSET,1);}
}

/* C3019C: rescale every marker vertex, then consume all three children. */
void render_marker_polygon(RenderParentState w,const RenderParentHooks *h) {
 P(registers,RDP_REGISTERS,0xc4b432);P(screen,RDP_SCREEN,0xc4b390);W(offset,RDP_OFFSET,rd_u16(w.registers));P(registers,RDP_REGISTERS,w.registers+2);
 if((int16_t)w.offset<=0)return;
 word(h,w.screen,(uint16_t)w.offset);P(screen,RDP_SCREEN,w.screen+2);SW(offset,RDP_OFFSET,1);
 for(;;) {
  W(value,RDP_VALUE,rd_u16(w.registers));P(registers,RDP_REGISTERS,w.registers+2);w.value=(uint16_t)w.value*24u;observe(h,RDP_MULU,RDP_VALUE,24,0);ARL(value,RDP_VALUE,8);AW(value,RDP_VALUE,193);AW(value,RDP_VALUE,rd_u16(0xc45988));word(h,w.screen,(uint16_t)w.value);P(screen,RDP_SCREEN,w.screen+2);
  W(value,RDP_VALUE,rd_u16(w.registers));P(registers,RDP_REGISTERS,w.registers+2);w.value=(uint16_t)w.value*31u;observe(h,RDP_MULU,RDP_VALUE,31,0);ARL(value,RDP_VALUE,8);AW(value,RDP_VALUE,162);AW(value,RDP_VALUE,rd_u16(0xc458d8));word(h,w.screen,(uint16_t)w.value);P(screen,RDP_SCREEN,w.screen+2);
  w.offset=low_word(w.offset,(uint16_t)(w.offset-1));observe(h,RDP_DECREMENT,RDP_OFFSET,w.offset,0);if((uint16_t)w.offset==0xffffu)break;
 }
 w=consume(h,RDP_MARKER_CLASSIFY);if(!w.zero)return;
 L(value,RDP_VALUE,4);L(secondary,RDP_SECONDARY,1);w=consume(h,RDP_MARKER_FILL);w=consume(h,RDP_MARKER_OUTLINE);
}

/* C2122A: consume every offset segment; source frame count/status and
 * MOVEM.W stack transfers survive each actual returned child state. */
void render_offset_run(RenderParentState w,const RenderParentHooks *h) {
 W(value,RDP_VALUE,rd_u16(w.stream));P(stream,RDP_STREAM,w.stream+2);W(base,RDP_BASE,w.value);ANDW(base,RDP_BASE,63);word(h,0xc45954,(uint16_t)w.base);
 w.value=low_word(w.value,(uint16_t)w.value>>8);observe(h,RDP_LSR_WORD,RDP_VALUE,8,0);word(h,w.frame-0x30,(uint16_t)w.value);
 W(value,RDP_VALUE,rd_u16(w.stream));P(stream,RDP_STREAM,w.stream+2);P(descriptor,RDP_DESCRIPTOR,0xc48390);w=load_words(w,h,w.descriptor+(gaddr)(int32_t)(int16_t)w.value,0x00fc);
 SW(stride,RDP_STRIDE,w.control);SW(size,RDP_SIZE,w.secondary);SW(offset,RDP_OFFSET,w.source);P(screen,RDP_SCREEN,(gaddr)(int32_t)(int16_t)w.stride);
 P(descriptor,RDP_DESCRIPTOR,w.descriptor+(gaddr)(int32_t)rd_s16(w.stream));P(stream,RDP_STREAM,w.stream+2);w=save_longs(w,h,0x0064);word(h,w.frame-0x7e,0);
 do {
  w=load_words(w,h,w.descriptor,0x003f);P(descriptor,RDP_DESCRIPTOR,w.descriptor+12);
  SW(value,RDP_VALUE,w.screen);SW(base,RDP_BASE,w.size);SW(control,RDP_CONTROL,w.offset);SW(secondary,RDP_SECONDARY,w.screen);SW(source,RDP_SOURCE,w.size);SW(stride,RDP_STRIDE,w.offset);
  store_words(h,0xc4c592,0x003f);observe(h,RDP_PUSH_LONG,RDP_VALUE,w.descriptor,0);w=restored(h);observe(h,RDP_SAVE_WORDS,RDP_VALUE,0x0308,0);w=restored(h);
  w=consume(h,RDP_OFFSET_SEGMENT);word(h,w.frame-0x7e,(uint16_t)(rd_u16(w.frame-0x7e)|w.value));observe(h,RDP_RESTORE_WORDS,RDP_VALUE,0x10c0,0);w=restored(h);observe(h,RDP_POP_POINTER,RDP_DESCRIPTOR,0,0);w=restored(h);
 }while(decrement_word(h,w.frame-0x30)>0);
 w=restore_longs(w,h,0x2600);W(value,RDP_VALUE,rd_u16(w.frame-0x7e));
}

/* C21500: complete four-point block face, rejection and clip child. */
void render_block_face(RenderParentState w,const RenderParentHooks *h) {
 word(h,0xc45954,rd_u16(w.stream));P(stream,RDP_STREAM,w.stream+2);P(descriptor,RDP_DESCRIPTOR,0xc48390);P(descriptor,RDP_DESCRIPTOR,w.descriptor+(gaddr)(int32_t)rd_s16(w.stream));P(stream,RDP_STREAM,w.stream+2);
 CL((uint32_t)-128,rd_u32(0xc45a78));if(rd_s32(0xc45a78)<-128) {L(value,RDP_VALUE,0);return;}
 w=save_longs(w,h,0x0064);P(registers,RDP_REGISTERS,0xc4bf90);word(h,w.registers,0);P(registers,RDP_REGISTERS,w.registers+2);word(h,w.registers,4);P(registers,RDP_REGISTERS,w.registers+2);
 w=load_words(w,h,w.descriptor,0x00fc);SW(stride,RDP_STRIDE,w.control);SW(size,RDP_SIZE,w.secondary);SW(offset,RDP_OFFSET,w.source);P(table,RDP_TABLE,(gaddr)(int32_t)(int16_t)w.stride);P(screen,RDP_SCREEN,(gaddr)(int32_t)(int16_t)w.size);P(modulo,RDP_MODULO,(gaddr)(int32_t)(int16_t)w.offset);
 w=load_words(w,h,w.descriptor+18,7);
 word(h,w.registers,(uint16_t)w.value);P(registers,RDP_REGISTERS,w.registers+2);word(h,w.registers,(uint16_t)w.base);P(registers,RDP_REGISTERS,w.registers+2);word(h,w.registers,(uint16_t)w.control);P(registers,RDP_REGISTERS,w.registers+2);
 AW(value,RDP_VALUE,w.stride);AW(base,RDP_BASE,w.size);AW(control,RDP_CONTROL,w.offset);
 word(h,w.registers,(uint16_t)w.value);P(registers,RDP_REGISTERS,w.registers+2);word(h,w.registers,(uint16_t)w.base);P(registers,RDP_REGISTERS,w.registers+2);word(h,w.registers,(uint16_t)w.control);P(registers,RDP_REGISTERS,w.registers+2);
 P(stream,RDP_STREAM,(gaddr)(int32_t)(int16_t)w.control);w=load_words(w,h,w.descriptor+6,0x00fc);SW(stride,RDP_STRIDE,w.control);SW(size,RDP_SIZE,w.secondary);SW(offset,RDP_OFFSET,w.source);W(control,RDP_CONTROL,w.stream);
 AW(value,RDP_VALUE,w.stride);AW(base,RDP_BASE,w.size);AW(control,RDP_CONTROL,w.offset);
 word(h,w.registers,(uint16_t)w.value);P(registers,RDP_REGISTERS,w.registers+2);word(h,w.registers,(uint16_t)w.base);P(registers,RDP_REGISTERS,w.registers+2);word(h,w.registers,(uint16_t)w.control);P(registers,RDP_REGISTERS,w.registers+2);
 SW(value,RDP_VALUE,w.table);SW(base,RDP_BASE,w.screen);SW(control,RDP_CONTROL,w.modulo);store_words(h,w.registers,7);
 W(offset,RDP_OFFSET,rd_u16(w.registers-14));ANDW(offset,RDP_OFFSET,rd_u16(w.registers-8));ANDW(offset,RDP_OFFSET,rd_u16(w.registers-2));ANDW(offset,RDP_OFFSET,rd_u16(w.registers+4));
 if((int16_t)w.offset<0) {w=restore_longs(w,h,0x2600);L(value,RDP_VALUE,0);}else {w=consume(h,RDP_BLOCK_FACE);w=restore_longs(w,h,0x2600);}
}

/* C20592: original split-square diagonal and exact clip input order. */
void render_split_square(RenderParentState w,const RenderParentHooks *h) {
 int32_t first,second;int alternate;
 L(value,RDP_VALUE,rd_u32(w.stream));P(stream,RDP_STREAM,w.stream+4);P(descriptor,RDP_DESCRIPTOR,0xc48390);w=load_words(w,h,w.descriptor+6,0x000e);
 CW(w.secondary,w.base);if((int16_t)w.base>(int16_t)w.secondary)goto outside;NEG(base,RDP_BASE);CW(w.secondary,w.base);if((int16_t)w.base>(int16_t)w.secondary)goto outside;
 CW(w.secondary,w.control);if((int16_t)w.control>(int16_t)w.secondary)goto outside;NEG(control,RDP_CONTROL);CW(w.secondary,w.control);if((int16_t)w.control>(int16_t)w.secondary)goto outside;
 P(registers,RDP_REGISTERS,0xc4bf90);longword(h,w.registers,3);P(registers,RDP_REGISTERS,w.registers+4);
 W(size,RDP_SIZE,rd_u16(0xc45a72));W(offset,RDP_OFFSET,rd_u16(0xc45a76));NEG(size,RDP_SIZE);NEG(offset,RDP_OFFSET);W(control,RDP_CONTROL,rd_u16(0xc45b2a));W(secondary,RDP_SECONDARY,rd_u16(0xc45b2e));W(stride,RDP_STRIDE,rd_u16(0xc45ab8));ASL(control,RDP_CONTROL,w.stride);ASL(secondary,RDP_SECONDARY,w.stride);
 first=(int16_t)w.size-(int16_t)w.control;SW(size,RDP_SIZE,w.control);second=(int16_t)w.offset-(int16_t)w.secondary;SW(offset,RDP_OFFSET,w.secondary);
 alternate=(first<0)!=(second<0);
 if(first<0)NEG(size,RDP_SIZE);if(second<0)NEG(offset,RDP_OFFSET);CW(w.size,w.offset);if(!((int16_t)w.offset>(int16_t)w.size))SWAP(value,RDP_VALUE);W(offset,RDP_OFFSET,w.value);
 longword(h,w.registers,rd_u32(w.descriptor));P(registers,RDP_REGISTERS,w.registers+4);word(h,w.registers,rd_u16(w.descriptor+4));P(registers,RDP_REGISTERS,w.registers+2);
 longword(h,w.registers,rd_u32(w.descriptor+(alternate?12u:6u)));P(registers,RDP_REGISTERS,w.registers+4);word(h,w.registers,rd_u16(w.descriptor+(alternate?16u:10u)));P(registers,RDP_REGISTERS,w.registers+2);
 longword(h,w.registers,rd_u32(w.descriptor+(alternate?18u:24u)));P(registers,RDP_REGISTERS,w.registers+4);word(h,w.registers,rd_u16(w.descriptor+(alternate?22u:28u)));P(registers,RDP_REGISTERS,w.registers+2);
 word(h,0xc45954,(uint16_t)w.offset);w=save_longs(w,h,0x0064);w=consume(h,RDP_SPLIT_FACE);w=restore_longs(w,h,0x2600);return;
outside:L(value,RDP_VALUE,0xffffffffu);
}

/* C201A6: original record-shadow admission, scaling, winding, face stream
 * and every clip child. Source stream terminators alone stop traversal. */
void render_record_shadow(RenderParentState w,const RenderParentHooks *h) {
 int32_t excess;
 L(value,RDP_VALUE,rd_u32(0xc45a66));P(screen,RDP_SCREEN,0xc46184+(gaddr)(int32_t)rd_s16(0xc459b6));B(secondary,RDP_SECONDARY,rd_u8(w.screen+4));B(offset,RDP_OFFSET,w.secondary);ANDB(offset,RDP_OFFSET,0xc0);
 if((uint8_t)w.offset) {CB(0xc0,w.offset);if((uint8_t)w.offset!=0xc0)goto one;}
 ANDB(secondary,RDP_SECONDARY,0x40);
 if((uint8_t)w.secondary) {
  W(offset,RDP_OFFSET,rd_u16(w.screen+0x4e));NEG(offset,RDP_OFFSET);EL(offset,RDP_OFFSET);ALL(offset,RDP_OFFSET,8);CL(w.offset,w.value);if((int32_t)w.value>=(int32_t)w.offset)goto one;
  SL(value,RDP_VALUE,w.offset);W(secondary,RDP_SECONDARY,rd_u16(0xc45ab8));ARL(value,RDP_VALUE,w.secondary);longword(h,w.frame-0x90,w.value);
 }else longword(h,w.frame-0x90,rd_u32(w.frame-0x1c));
 CL(0xfff00000u,w.value);if((int32_t)w.value<-0x100000) {L(value,RDP_VALUE,0xffffffffu);return;}
 TB(rd_u8(0xc45785));if(!rd_u8(0xc45785)) {
  CB(3,rd_u8(0xc4586b));if(rd_s8(0xc4586b)<3) {
   W(offset,RDP_OFFSET,rd_u16(0xc459b4));CW(rd_u16(0xc458dc),w.offset);
   if((uint16_t)w.offset==rd_u16(0xc458dc)) {TB(rd_u8(0xc4586b));if(!rd_u8(0xc4586b))goto one;L(control,RDP_CONTROL,0xffffe000u);CL(w.control,w.value);if((int32_t)w.value<(int32_t)w.control)goto one;}
   else {
    CB(0x30,rd_u8(w.screen+0x62));if(rd_u8(w.screen+0x62)!=0x30) {
     CL(0xffff0000u,w.value);if((int32_t)w.value<-0x10000) {
      CB(0x14,rd_u8(w.screen+0x62));if(rd_u8(w.screen+0x62)!=0x14) {
       CB(1,rd_u8(0xc4586b));if(rd_s8(0xc4586b)<=1)goto one;L(control,RDP_CONTROL,0xfff60000u);CL(w.control,w.value);if((int32_t)w.value<(int32_t)w.control)goto one;
      }
     }
    }
   }
  }
 }
 w=save_longs(w,h,0x0044);W(value,RDP_VALUE,rd_u16(w.stream));P(stream,RDP_STREAM,w.stream+2);EL(value,RDP_VALUE);ALL(value,RDP_VALUE,8);CL(rd_u32(w.screen+24),w.value);if((int32_t)w.value<rd_s32(w.screen+24))goto stream_end;
 P(screen,RDP_SCREEN,w.screen+0xa4);word(h,w.frame-2,rd_u16(0xc45ab8));L(control,RDP_CONTROL,w.value);NL(value,RDP_VALUE);SWAP(value,RDP_VALUE);EL(value,RDP_VALUE);ARL(value,RDP_VALUE,5);excess=(int16_t)w.value-rd_s16(w.frame-2);SW(value,RDP_VALUE,rd_u16(w.frame-2));
 if(excess<=0) {W(value,RDP_VALUE,0);longword(h,w.frame-0x94,rd_u32(w.frame-0x20));longword(h,w.frame-0x8c,rd_u32(w.frame-0x18));}
 else {
  uint16_t previous=rd_u16(w.frame-2);wr_u16(w.frame-2,(uint16_t)(previous+w.value));observe(h,RDP_MEMORY_ADD_WORD,RDP_VALUE,previous,(uint16_t)w.value);
  L(base,RDP_BASE,rd_u32(w.frame-0x20));L(secondary,RDP_SECONDARY,rd_u32(w.frame-0x18));ARL(base,RDP_BASE,w.value);ARL(secondary,RDP_SECONDARY,w.value);W(value,RDP_VALUE,rd_u16(w.frame-2));ARL(control,RDP_CONTROL,w.value);store_longs(h,w.frame-0x94,0x000e);
 }
 w=load_longs(w,h,w.frame-0x94,0x000e);L(source,RDP_SOURCE,rd_u32(0xc45b30));L(stride,RDP_STRIDE,rd_u32(0xc45b38));ARL(source,RDP_SOURCE,w.value);ARL(stride,RDP_STRIDE,w.value);AL(base,RDP_BASE,w.source);AL(secondary,RDP_SECONDARY,w.stride);L(offset,RDP_OFFSET,8);SW(offset,RDP_OFFSET,rd_u16(w.frame-6));ARL(base,RDP_BASE,w.offset);ARL(control,RDP_CONTROL,w.offset);ARL(secondary,RDP_SECONDARY,w.offset);store_words(h,w.frame-0x14,0x000e);word(h,0xc45954,0);word(h,0xc4bf90,0);
 for(;;) {
  TW(rd_u16(w.stream+2));
  if(!rd_u16(w.stream+2)) {
   w=load_words(w,h,w.stream+4,0x000e);W(value,RDP_VALUE,rd_u16(w.screen+(gaddr)(int32_t)(int16_t)w.base));W(stride,RDP_STRIDE,rd_u16(w.screen+(gaddr)(int32_t)(int16_t)w.control));SW(stride,RDP_STRIDE,w.value);W(size,RDP_SIZE,rd_u16(w.screen+(gaddr)(int32_t)(int16_t)w.secondary));SW(size,RDP_SIZE,w.value);
   W(source,RDP_SOURCE,rd_u16(w.screen+4+(gaddr)(int32_t)(int16_t)w.base));W(base,RDP_BASE,rd_u16(w.screen+4+(gaddr)(int32_t)(int16_t)w.control));SW(base,RDP_BASE,w.source);W(secondary,RDP_SECONDARY,rd_u16(w.screen+4+(gaddr)(int32_t)(int16_t)w.secondary));SW(secondary,RDP_SECONDARY,w.source);MUL(size,RDP_SIZE,w.base);MUL(secondary,RDP_SECONDARY,w.stride);SL(size,RDP_SIZE,w.secondary);ARL(size,RDP_SIZE,8);
   if((int32_t)w.size<0) {
    W(offset,RDP_OFFSET,rd_u16(w.stream));P(stream,RDP_STREAM,w.stream+2);AW(offset,RDP_OFFSET,w.offset);AW(offset,RDP_OFFSET,2);P(stream,RDP_STREAM,w.stream+(gaddr)(int32_t)(int16_t)w.offset);goto next_face;
   }
  }
  W(offset,RDP_OFFSET,rd_u16(w.stream));P(stream,RDP_STREAM,w.stream+4);P(registers,RDP_REGISTERS,0xc4bf92);word(h,w.registers,(uint16_t)w.offset);P(registers,RDP_REGISTERS,w.registers+2);word(h,w.frame-14,(uint16_t)w.offset);
  do {
   W(offset,RDP_OFFSET,rd_u16(w.stream));P(stream,RDP_STREAM,w.stream+2);W(stride,RDP_STRIDE,rd_u16(w.frame-8));W(control,RDP_CONTROL,rd_u16(w.screen+(gaddr)(int32_t)(int16_t)w.offset));ASR(control,RDP_CONTROL,w.stride);W(source,RDP_SOURCE,rd_u16(w.screen+4+(gaddr)(int32_t)(int16_t)w.offset));ASR(source,RDP_SOURCE,w.stride);AW(control,RDP_CONTROL,rd_u16(w.frame-20));W(secondary,RDP_SECONDARY,rd_u16(w.frame-18));AW(source,RDP_SOURCE,rd_u16(w.frame-16));P(modulo,RDP_MODULO,0xc45bd8);
   w=matrix_row(w,h,w.modulo);P(modulo,RDP_MODULO,w.modulo+6);word(h,w.registers,(uint16_t)w.offset);P(registers,RDP_REGISTERS,w.registers+2);
   w=matrix_row(w,h,w.modulo);P(modulo,RDP_MODULO,w.modulo+6);word(h,w.registers,(uint16_t)w.offset);P(registers,RDP_REGISTERS,w.registers+2);
   MUP(control,RDP_CONTROL,modulo,RDP_MODULO);MUP(secondary,RDP_SECONDARY,modulo,RDP_MODULO);MUP(source,RDP_SOURCE,modulo,RDP_MODULO);AL(source,RDP_SOURCE,w.secondary);AL(source,RDP_SOURCE,w.control);ARL(source,RDP_SOURCE,8);word(h,w.registers,(uint16_t)w.source);P(registers,RDP_REGISTERS,w.registers+2);
  }while(decrement_word(h,w.frame-14)>0);
  w=save_longs(w,h,0x0028);w=consume(h,RDP_SHADOW_FACE);w=restore_longs(w,h,0x1400);
next_face:
  W(value,RDP_VALUE,rd_u16(w.stream));P(stream,RDP_STREAM,w.stream+2);if((int16_t)w.value<0)goto stream_end;EL(value,RDP_VALUE);P(modulo,RDP_MODULO,w.screen-0x94);CL(rd_u32(w.modulo),w.value);if((int32_t)w.value<rd_s32(w.modulo))goto stream_end;
 }
stream_end:w=restore_longs(w,h,0x2200);
one:L(value,RDP_VALUE,1);
}

static RenderParentState vertex_orientation(RenderParentState w,const RenderParentHooks *h) {
 w=matrix_row(w,h,w.stream);P(stream,RDP_STREAM,w.stream+6);P(modulo,RDP_MODULO,(gaddr)(int32_t)(int16_t)w.offset);
 w=matrix_row(w,h,w.stream);P(stream,RDP_STREAM,w.stream+6);
 MUP(control,RDP_CONTROL,stream,RDP_STREAM);MUP(secondary,RDP_SECONDARY,stream,RDP_STREAM);MUP(source,RDP_SOURCE,stream,RDP_STREAM);AL(source,RDP_SOURCE,w.secondary);AL(source,RDP_SOURCE,w.control);ARL(source,RDP_SOURCE,8);return w;
}
static RenderParentState vertex_view(RenderParentState w,const RenderParentHooks *h,int pointer) {
 gaddr row=pointer?w.stream:w.registers;
 w=matrix_row(w,h,row);row+=6;if(pointer)P(stream,RDP_STREAM,row);else P(registers,RDP_REGISTERS,row);
 word(h,w.descriptor,(uint16_t)w.offset);P(descriptor,RDP_DESCRIPTOR,w.descriptor+2);
 w=matrix_row(w,h,row);row+=6;if(pointer)P(stream,RDP_STREAM,row);else P(registers,RDP_REGISTERS,row);
 word(h,w.descriptor,(uint16_t)w.offset);P(descriptor,RDP_DESCRIPTOR,w.descriptor+2);
 MUL(control,RDP_CONTROL,rd_u16(row));row+=2;if(pointer)P(stream,RDP_STREAM,row);else P(registers,RDP_REGISTERS,row);
 MUL(secondary,RDP_SECONDARY,rd_u16(row));row+=2;if(pointer)P(stream,RDP_STREAM,row);else P(registers,RDP_REGISTERS,row);
 MUL(source,RDP_SOURCE,rd_u16(row));row+=2;if(pointer)P(stream,RDP_STREAM,row);else P(registers,RDP_REGISTERS,row);
 AL(source,RDP_SOURCE,w.secondary);AL(source,RDP_SOURCE,w.control);ARL(source,RDP_SOURCE,8);word(h,w.descriptor,(uint16_t)w.source);P(descriptor,RDP_DESCRIPTOR,w.descriptor+2);return w;
}

/* C1F99A: all oriented, ordinary and compressed vertex paths. */
void render_vertices(RenderParentState w,const RenderParentHooks *h) {
 uint32_t swap;
 P(descriptor,RDP_DESCRIPTOR,0xc48390);observe(h,RDP_PUSH_LONG,RDP_VALUE,w.table,0);w=restored(h);word(h,w.frame-10,(uint16_t)w.value);P(table,RDP_TABLE,rd_u32(0xc45a32));B(size,RDP_SIZE,rd_u8(w.table+7));observe(h,RDP_BIT_TEST,RDP_SIZE,w.size,0);
 if(w.size&1u) {
  w=save_longs(w,h,0x0024);P(table,RDP_TABLE,w.table+10+(gaddr)(int32_t)(int16_t)w.offset);P(stream,RDP_STREAM,0xc45bc6);P(screen,RDP_SCREEN,w.stream);P(descriptor,RDP_DESCRIPTOR,w.descriptor+(gaddr)(int32_t)(int16_t)w.offset);w=load_longs(w,h,w.frame-32,7);
  swap=w.base;w.base=w.control;w.control=swap;observe(h,RDP_EXCHANGE_DATA,RDP_BASE,2,0);w=load_longs(w,h,0xc45b30,0x0038);AL(value,RDP_VALUE,w.secondary);AL(control,RDP_CONTROL,w.source);AL(base,RDP_BASE,w.stride);L(offset,RDP_OFFSET,8);SW(offset,RDP_OFFSET,rd_u16(w.frame-6));ARL(value,RDP_VALUE,w.offset);ARL(base,RDP_BASE,w.offset);ARL(control,RDP_CONTROL,w.offset);P(registers,RDP_REGISTERS,(gaddr)(int32_t)(int16_t)w.control);
  do {
   w=load_words(w,h,w.table,0x001c);P(table,RDP_TABLE,w.table+6);W(offset,RDP_OFFSET,rd_u16(w.frame-8));ASR(control,RDP_CONTROL,w.offset);ASR(secondary,RDP_SECONDARY,w.offset);ASR(source,RDP_SOURCE,w.offset);P(stream,RDP_STREAM,w.screen);w=vertex_orientation(w,h);
   if(bit(h,w.frame-0x7f,0))w=consume(h,RDP_VERTEX_TRANSFORM);
   else {W(control,RDP_CONTROL,w.modulo);W(secondary,RDP_SECONDARY,w.offset);AW(control,RDP_CONTROL,w.value);AW(secondary,RDP_SECONDARY,w.registers);AW(source,RDP_SOURCE,w.base);P(stream,RDP_STREAM,w.screen+18);w=vertex_view(w,h,1);}
  }while(decrement_word(h,w.frame-10)>0);
  w=restore_longs(w,h,0x2400);
 } else {
  P(table,RDP_TABLE,w.table+10);P(table,RDP_TABLE,w.table+(gaddr)(int32_t)(int16_t)w.offset);P(registers,RDP_REGISTERS,0xc45bd8);P(screen,RDP_SCREEN,w.registers);w=load_longs(w,h,w.frame-32,7);swap=w.base;w.base=w.control;w.control=swap;observe(h,RDP_EXCHANGE_DATA,RDP_BASE,2,0);AL(value,RDP_VALUE,rd_u32(0xc45b30));AL(base,RDP_BASE,rd_u32(0xc45b38));L(secondary,RDP_SECONDARY,8);SW(secondary,RDP_SECONDARY,rd_u16(w.frame-6));ARL(value,RDP_VALUE,w.secondary);ARL(base,RDP_BASE,w.secondary);ARL(control,RDP_CONTROL,w.secondary);word(h,w.frame-18,(uint16_t)w.control);ANDW(size,RDP_SIZE,2);
  if(!(uint16_t)w.size) {
   P(descriptor,RDP_DESCRIPTOR,w.descriptor+(gaddr)(int32_t)(int16_t)w.offset);
   do {w=load_words(w,h,w.table,0x001c);P(table,RDP_TABLE,w.table+6);W(offset,RDP_OFFSET,rd_u16(w.frame-8));ASR(control,RDP_CONTROL,w.offset);ASR(secondary,RDP_SECONDARY,w.offset);ASR(source,RDP_SOURCE,w.offset);AW(control,RDP_CONTROL,w.value);AW(secondary,RDP_SECONDARY,rd_u16(w.frame-18));AW(source,RDP_SOURCE,w.base);P(registers,RDP_REGISTERS,w.screen);w=vertex_view(w,h,0);}while(decrement_word(h,w.frame-10)>0);
  } else {
   W(control,RDP_CONTROL,w.offset);ASR(control,RDP_CONTROL,1);AW(offset,RDP_OFFSET,w.control);P(descriptor,RDP_DESCRIPTOR,w.descriptor+(gaddr)(int32_t)(int16_t)w.offset);w=load_words(w,h,w.frame-0x78,0x0048);
   do {
    W(control,RDP_CONTROL,rd_u16(w.table));P(table,RDP_TABLE,w.table+2);W(source,RDP_SOURCE,rd_u16(w.table));P(table,RDP_TABLE,w.table+2);W(offset,RDP_OFFSET,rd_u16(w.frame-8));ASR(control,RDP_CONTROL,w.offset);ASR(source,RDP_SOURCE,w.offset);AW(control,RDP_CONTROL,w.value);AW(source,RDP_SOURCE,w.base);P(registers,RDP_REGISTERS,w.screen);
    W(stride,RDP_STRIDE,w.control);W(offset,RDP_OFFSET,w.source);MUP(stride,RDP_STRIDE,registers,RDP_REGISTERS);P(registers,RDP_REGISTERS,w.registers+2);MUP(offset,RDP_OFFSET,registers,RDP_REGISTERS);AL(offset,RDP_OFFSET,w.stride);ARL(offset,RDP_OFFSET,8);AW(offset,RDP_OFFSET,w.secondary);word(h,w.descriptor,(uint16_t)w.offset);P(descriptor,RDP_DESCRIPTOR,w.descriptor+2);
    W(stride,RDP_STRIDE,w.control);W(offset,RDP_OFFSET,w.source);MUP(stride,RDP_STRIDE,registers,RDP_REGISTERS);P(registers,RDP_REGISTERS,w.registers+2);MUP(offset,RDP_OFFSET,registers,RDP_REGISTERS);AL(offset,RDP_OFFSET,w.stride);ARL(offset,RDP_OFFSET,8);AW(offset,RDP_OFFSET,w.size);word(h,w.descriptor,(uint16_t)w.offset);P(descriptor,RDP_DESCRIPTOR,w.descriptor+2);
    MUP(control,RDP_CONTROL,registers,RDP_REGISTERS);MUL(source,RDP_SOURCE,rd_u16(w.registers+2));AL(source,RDP_SOURCE,w.control);ARL(source,RDP_SOURCE,8);AW(source,RDP_SOURCE,rd_u16(w.frame-0x74));word(h,w.descriptor,(uint16_t)w.source);P(descriptor,RDP_DESCRIPTOR,w.descriptor+2);
   }while(decrement_word(h,w.frame-10)>0);
  }
 }
 observe(h,RDP_POP_POINTER,RDP_TABLE,0,0);w=restored(h);
}

#define DS(f,id,n) do { int16_t n_=(int16_t)(n);if(n_) {int64_t q_=(int64_t)(int32_t)w.f/n_;if(q_>=-32768 && q_<=32767)w.f=(uint32_t)(uint16_t)((int64_t)(int32_t)w.f%n_)<<16|(uint16_t)q_;}observe(h,RDP_DIVS,id,(uint16_t)n_,0); } while(0)
#define MU(f,id,n) do {uint16_t n_=(uint16_t)(n);w.f=(uint16_t)w.f*(uint32_t)n_;observe(h,RDP_MULU,id,n_,0);}while(0)

/* C2D16C: every shape component, original view tests, screen limits and
 * line/polygon/circle children. Tables remain original memory data. */
void render_shape(RenderParentState w,const RenderParentHooks *h) {
 int32_t kind,adjust;int inside;
 observe(h,RDP_LINK_FRAME,RDP_VALUE,0x3e,0);w=restored(h);byte(h,w.frame-0x2e,(uint8_t)w.source);word(h,w.frame-0x1e,0);word(h,w.frame-0x3c,(uint16_t)w.size);word(h,w.frame-0x18,(uint16_t)w.offset);store_words(h,w.frame-8,7);word(h,w.frame-0x30,(uint16_t)w.secondary);word(h,w.frame-10,11);word(h,0xc4b390,3);
 kind=(int8_t)w.source;SB(source,RDP_SOURCE,1);
 if(kind<=1) {longword(h,w.frame-0x34,0xc2d001);longword(h,w.frame-0x38,0xc2cf6e);}
 else {SB(source,RDP_SOURCE,1);if(kind<=2) {longword(h,w.frame-0x34,0xc2d00d);longword(h,w.frame-0x38,0xc2cf6e);}else {SB(source,RDP_SOURCE,1);if(kind<=3) {longword(h,w.frame-0x34,0xc2d019);longword(h,w.frame-0x38,0xc2cfe3);word(h,w.frame-10,8);}else {SB(source,RDP_SOURCE,1);if(kind<=4) {longword(h,w.frame-0x34,0xc2d025);longword(h,w.frame-0x38,0xc2cfbc);}else {longword(h,w.frame-0x34,0xc2d031);longword(h,w.frame-0x38,0xc2cf95);}}}}
 do {
  P(screen,RDP_SCREEN,rd_u32(w.frame-0x38));W(source,RDP_SOURCE,rd_u16(w.frame-10));W(stride,RDP_STRIDE,w.source);AW(source,RDP_SOURCE,w.source);AW(source,RDP_SOURCE,w.stride);P(screen,RDP_SCREEN,w.screen+(gaddr)(int32_t)(int16_t)w.source);
  B(value,RDP_VALUE,rd_u8(w.screen));B(base,RDP_BASE,rd_u8(w.screen+1));B(control,RDP_CONTROL,rd_u8(w.screen+2));EW(value,RDP_VALUE);EW(base,RDP_BASE);EW(control,RDP_CONTROL);W(source,RDP_SOURCE,rd_u16(w.frame-0x30));MU(value,RDP_VALUE,w.source);MU(base,RDP_BASE,w.source);MU(control,RDP_CONTROL,w.source);
  AW(stride,RDP_STRIDE,w.stride);AW(stride,RDP_STRIDE,w.source);AW(stride,RDP_STRIDE,w.source);P(descriptor,RDP_DESCRIPTOR,0xc2d03e);W(secondary,RDP_SECONDARY,rd_u16(w.descriptor+(gaddr)(int32_t)(int16_t)w.stride));W(source,RDP_SOURCE,rd_u16(w.frame-0x18));ASR(value,RDP_VALUE,w.source);ASR(base,RDP_BASE,w.source);ASR(control,RDP_CONTROL,w.source);AW(value,RDP_VALUE,rd_u16(w.frame-8));AW(base,RDP_BASE,rd_u16(w.frame-6));AW(control,RDP_CONTROL,rd_u16(w.frame-4));
  P(screen,RDP_SCREEN,(gaddr)(int32_t)(int16_t)w.value);P(modulo,RDP_MODULO,(gaddr)(int32_t)(int16_t)w.base);word(h,w.frame-2,(uint16_t)w.control);W(source,RDP_SOURCE,w.secondary);P(stream,RDP_STREAM,rd_u32(w.frame-0x34));W(secondary,RDP_SECONDARY,rd_u16(w.frame-10));B(secondary,RDP_SECONDARY,rd_u8(w.stream+(gaddr)(int32_t)(int16_t)w.secondary));TB(rd_u8(w.frame-0x2e));if(!rd_u8(w.frame-0x2e))L(secondary,RDP_SECONDARY,13);word(h,0xc45954,(uint16_t)w.secondary);
  P(stream,RDP_STREAM,0xc2cebe);P(stream,RDP_STREAM,rd_u32(w.stream+(gaddr)(int32_t)(int16_t)w.source));P(table,RDP_TABLE,0xc4b392);inside=1;
  for(;;) {
   CB(3,rd_u8(w.frame-0x2e));if(rd_u8(w.frame-0x2e)==3) {L(secondary,RDP_SECONDARY,0);L(source,RDP_SOURCE,0);L(stride,RDP_STRIDE,0);}else {w=load_words(w,h,w.stream,0x0038);P(stream,RDP_STREAM,w.stream+6);}
   AW(secondary,RDP_SECONDARY,w.screen);AW(source,RDP_SOURCE,w.modulo);AW(stride,RDP_STRIDE,rd_u16(w.frame-2));W(offset,RDP_OFFSET,rd_u16(w.frame-0x18));ASR(secondary,RDP_SECONDARY,w.offset);ASR(source,RDP_SOURCE,w.offset);ASR(stride,RDP_STRIDE,w.offset);P(registers,RDP_REGISTERS,0xc45bd8);
   w=load_words(w,h,w.registers,7);P(registers,RDP_REGISTERS,w.registers+6);MUL(value,RDP_VALUE,w.secondary);MUL(base,RDP_BASE,w.source);MUL(control,RDP_CONTROL,w.stride);AL(value,RDP_VALUE,w.base);AL(value,RDP_VALUE,w.control);ARL(value,RDP_VALUE,8);W(size,RDP_SIZE,w.value);
   w=load_words(w,h,w.registers,7);P(registers,RDP_REGISTERS,w.registers+6);MUL(value,RDP_VALUE,w.secondary);MUL(base,RDP_BASE,w.source);MUL(control,RDP_CONTROL,w.stride);AL(control,RDP_CONTROL,w.value);AL(control,RDP_CONTROL,w.base);ARL(control,RDP_CONTROL,8);
   MUP(secondary,RDP_SECONDARY,registers,RDP_REGISTERS);MUP(source,RDP_SOURCE,registers,RDP_REGISTERS);MUP(stride,RDP_STRIDE,registers,RDP_REGISTERS);AL(stride,RDP_STRIDE,w.secondary);AL(stride,RDP_STRIDE,w.source);ARL(stride,RDP_STRIDE,8);
   if((int32_t)w.stride<=0) {inside=0;break;}CW(w.stride,w.size);if((int16_t)w.size>(int16_t)w.stride) {inside=0;break;}W(source,RDP_SOURCE,w.size);NEG(source,RDP_SOURCE);CW(w.stride,w.source);if((int16_t)w.source>(int16_t)w.stride) {inside=0;break;}CW(w.stride,w.control);if((int16_t)w.control>(int16_t)w.stride) {inside=0;break;}W(source,RDP_SOURCE,w.control);NEG(source,RDP_SOURCE);CW(w.stride,w.source);if((int16_t)w.source>(int16_t)w.stride) {inside=0;break;}
   MUL(size,RDP_SIZE,160);DS(size,RDP_SIZE,w.stride);adjust=(int16_t)w.size+160;AW(size,RDP_SIZE,160);if(adjust<0)W(size,RDP_SIZE,0);else {CW(320,w.size);if((int16_t)w.size>=320)W(size,RDP_SIZE,319);}
   W(offset,RDP_OFFSET,w.control);MUL(offset,RDP_OFFSET,90);DS(offset,RDP_OFFSET,w.stride);adjust=(int16_t)w.offset+90;AW(offset,RDP_OFFSET,90);if(adjust<0)W(offset,RDP_OFFSET,0);else {CW(180,w.offset);if((int16_t)w.offset>=180)W(offset,RDP_OFFSET,179);}
   SW(size,RDP_SIZE,319);NEG(size,RDP_SIZE);SW(offset,RDP_OFFSET,179);NEG(offset,RDP_OFFSET);word(h,w.table,(uint16_t)w.size);P(table,RDP_TABLE,w.table+2);word(h,w.table,(uint16_t)w.offset);P(table,RDP_TABLE,w.table+2);
   CB(3,rd_u8(w.frame-0x2e));if(rd_u8(w.frame-0x2e)==3)break;CL(0xc4b39e,w.table);if(w.table>=0xc4b39e)break;
  }
  if(inside) {
   B(value,RDP_VALUE,rd_u8(w.frame-0x2e));if(!(uint8_t)w.value) {w=load_words(w,h,0xc4b392,15);w=consume(h,RDP_SHAPE_LINE);}else {kind=(int8_t)w.value;SB(value,RDP_VALUE,3);if(kind==3) {w=load_words(w,h,0xc4b392,3);W(size,RDP_SIZE,rd_u16(w.frame-0x3c));w=consume(h,RDP_SHAPE_CIRCLE);}else w=consume(h,RDP_SHAPE_POLYGON);}
   word(h,w.frame-0x1e,1);
  }
 }while(decrement_word(h,w.frame-10)>=0);
 W(value,RDP_VALUE,rd_u16(w.frame-0x1e));observe(h,RDP_UNLINK_FRAME,RDP_VALUE,0,0);w=restored(h);
}

/* C2168A: oriented side triangle, including both original clip sites. */
void render_side_triangle(RenderParentState w,const RenderParentHooks *h) {
 int first_path;
 P(descriptor,RDP_DESCRIPTOR,0xc48390);P(registers,RDP_REGISTERS,0xc4bf92);word(h,w.registers,3);P(registers,RDP_REGISTERS,w.registers+2);
 word(h,0xc45954,rd_u16(w.stream));P(stream,RDP_STREAM,w.stream+2);W(value,RDP_VALUE,rd_u16(w.stream));P(stream,RDP_STREAM,w.stream+2);w=load_words(w,h,w.descriptor+(gaddr)(int32_t)(int16_t)w.value,0x00fc);
 SW(stride,RDP_STRIDE,w.control);SW(size,RDP_SIZE,w.secondary);SW(offset,RDP_OFFSET,w.source);SWAP(size,RDP_SIZE);SWAP(offset,RDP_OFFSET);P(screen,RDP_SCREEN,rd_u32(0xc45a32));
 W(value,RDP_VALUE,rd_u16(w.stream));P(stream,RDP_STREAM,w.stream+2);W(base,RDP_BASE,rd_u16(w.screen+10+(gaddr)(int32_t)(int16_t)w.value));W(control,RDP_CONTROL,rd_u16(w.screen+14+(gaddr)(int32_t)(int16_t)w.value));W(secondary,RDP_SECONDARY,w.base);W(source,RDP_SOURCE,w.control);
 W(value,RDP_VALUE,rd_u16(w.stream));P(stream,RDP_STREAM,w.stream+2);W(size,RDP_SIZE,w.value);observe(h,RDP_BIT_CLEAR,RDP_VALUE,w.value,15);w.value&=~0x8000u;
 SW(base,RDP_BASE,rd_u16(w.screen+10+(gaddr)(int32_t)(int16_t)w.value));SW(control,RDP_CONTROL,rd_u16(w.screen+14+(gaddr)(int32_t)(int16_t)w.value));B(offset,RDP_OFFSET,rd_u8(w.screen+6));ANDW(offset,RDP_OFFSET,15);
 ASR(secondary,RDP_SECONDARY,w.offset);ASR(source,RDP_SOURCE,w.offset);AW(secondary,RDP_SECONDARY,rd_u16(0xc45b2a));AW(source,RDP_SOURCE,rd_u16(0xc45b2e));W(offset,RDP_OFFSET,rd_u16(0xc45ab8));ASL(secondary,RDP_SECONDARY,w.offset);ASL(source,RDP_SOURCE,w.offset);
 AW(secondary,RDP_SECONDARY,rd_u16(0xc45a72));AW(source,RDP_SOURCE,rd_u16(0xc45a76));MUL(secondary,RDP_SECONDARY,w.base);MUL(source,RDP_SOURCE,w.control);AL(source,RDP_SOURCE,w.secondary);SWAP(size,RDP_SIZE);SWAP(offset,RDP_OFFSET);
 W(value,RDP_VALUE,rd_u16(w.stream));P(stream,RDP_STREAM,w.stream+2);w=load_words(w,h,w.descriptor+(gaddr)(int32_t)(int16_t)w.value,0x000e);SW(base,RDP_BASE,w.stride);SW(control,RDP_CONTROL,w.size);SW(secondary,RDP_SECONDARY,w.offset);
 word(h,w.registers,(uint16_t)w.base);P(registers,RDP_REGISTERS,w.registers+2);word(h,w.registers,(uint16_t)w.control);P(registers,RDP_REGISTERS,w.registers+2);word(h,w.registers,(uint16_t)w.secondary);P(registers,RDP_REGISTERS,w.registers+2);
 P(descriptor,RDP_DESCRIPTOR,w.descriptor+(gaddr)(int32_t)rd_s16(w.stream));P(stream,RDP_STREAM,w.stream+2);w=save_longs(w,h,0x0064);TL(w.source);TL(w.size);first_path=((int32_t)w.source<0)==((int32_t)w.size<0);
 w=load_words(w,h,w.descriptor,0x00fc);SW(stride,RDP_STRIDE,w.control);SW(size,RDP_SIZE,w.secondary);SW(offset,RDP_OFFSET,w.source);
 if(first_path) {
  w=load_words(w,h,w.descriptor+18,7);word(h,w.registers,(uint16_t)w.value);P(registers,RDP_REGISTERS,w.registers+2);word(h,w.registers,(uint16_t)w.base);P(registers,RDP_REGISTERS,w.registers+2);word(h,w.registers,(uint16_t)w.control);P(registers,RDP_REGISTERS,w.registers+2);
 } else {P(table,RDP_TABLE,(gaddr)(int32_t)(int16_t)w.stride);P(screen,RDP_SCREEN,(gaddr)(int32_t)(int16_t)w.size);P(modulo,RDP_MODULO,(gaddr)(int32_t)(int16_t)w.offset);w=load_words(w,h,w.descriptor+18,7);}
 AW(value,RDP_VALUE,w.stride);AW(base,RDP_BASE,w.size);AW(control,RDP_CONTROL,w.offset);
 if(!first_path) {word(h,w.registers,(uint16_t)w.value);P(registers,RDP_REGISTERS,w.registers+2);word(h,w.registers,(uint16_t)w.base);P(registers,RDP_REGISTERS,w.registers+2);word(h,w.registers,(uint16_t)w.control);P(registers,RDP_REGISTERS,w.registers+2);}
 P(stream,RDP_STREAM,(gaddr)(int32_t)(int16_t)w.control);w=load_words(w,h,w.descriptor+6,0x00fc);SW(stride,RDP_STRIDE,w.control);SW(size,RDP_SIZE,w.secondary);SW(offset,RDP_OFFSET,w.source);W(control,RDP_CONTROL,w.stream);AW(value,RDP_VALUE,w.stride);AW(base,RDP_BASE,w.size);AW(control,RDP_CONTROL,w.offset);
 if(!first_path) {SW(value,RDP_VALUE,w.table);SW(base,RDP_BASE,w.screen);SW(control,RDP_CONTROL,w.modulo);}
 store_words(h,w.registers,7);ANDW(control,RDP_CONTROL,rd_u16(w.registers-8));ANDW(control,RDP_CONTROL,rd_u16(w.registers-2));
 if((int16_t)w.control<0) {w=restore_longs(w,h,0x2600);L(value,RDP_VALUE,0);return;}
 w=consume(h,first_path?RDP_SIDE_FIRST_FACE:RDP_SIDE_SECOND_FACE);w=restore_longs(w,h,0x2600);
}

static void square_status(RenderParentState w,const RenderParentHooks *h) {word(h,w.frame-0x7e,(uint16_t)(rd_u16(w.frame-0x7e)|w.value));}

/* C203D0: both possible face placements and their second-face children. */
void render_square_faces(RenderParentState w,const RenderParentHooks *h) {
 int32_t first,second;int alternate;
 word(h,w.frame-0x7e,0);L(value,RDP_VALUE,rd_u32(w.stream));P(stream,RDP_STREAM,w.stream+4);word(h,w.frame-0x5e,rd_u16(w.stream));P(stream,RDP_STREAM,w.stream+2);P(descriptor,RDP_DESCRIPTOR,0xc48390);w=load_words(w,h,w.descriptor+6,0x000e);
 CW(w.secondary,w.base);if((int16_t)w.base>(int16_t)w.secondary)goto outside;NEG(base,RDP_BASE);CW(w.secondary,w.base);if((int16_t)w.base>(int16_t)w.secondary)goto outside;CW(w.secondary,w.control);if((int16_t)w.control>(int16_t)w.secondary)goto outside;NEG(control,RDP_CONTROL);CW(w.secondary,w.control);if((int16_t)w.control>(int16_t)w.secondary)goto outside;
 w=save_longs(w,h,0x0064);P(registers,RDP_REGISTERS,0xc4bf90);longword(h,w.registers,4);P(registers,RDP_REGISTERS,w.registers+4);
 W(size,RDP_SIZE,rd_u16(0xc45a72));W(offset,RDP_OFFSET,rd_u16(0xc45a76));NEG(size,RDP_SIZE);NEG(offset,RDP_OFFSET);W(control,RDP_CONTROL,rd_u16(0xc45b2a));W(secondary,RDP_SECONDARY,rd_u16(0xc45b2e));W(stride,RDP_STRIDE,rd_u16(0xc45ab8));ASL(control,RDP_CONTROL,w.stride);ASL(secondary,RDP_SECONDARY,w.stride);
 first=(int16_t)w.size-(int16_t)w.control;SW(size,RDP_SIZE,w.control);second=(int16_t)w.offset-(int16_t)w.secondary;SW(offset,RDP_OFFSET,w.secondary);alternate=(first<0)!=(second<0);
 if(first<0)NEG(size,RDP_SIZE);if(second<0)NEG(offset,RDP_OFFSET);CW(w.size,w.offset);if(!((int16_t)w.offset>(int16_t)w.size))SWAP(value,RDP_VALUE);word(h,0xc45954,(uint16_t)w.value);
 w=load_words(w,h,w.descriptor,0x10ff);P(descriptor,RDP_DESCRIPTOR,w.descriptor+18);
 if(!alternate) {
  store_words(h,w.registers,0x003f);SW(value,RDP_VALUE,w.secondary);SW(base,RDP_BASE,w.source);SW(control,RDP_CONTROL,w.stride);SW(size,RDP_SIZE,w.secondary);SW(offset,RDP_OFFSET,w.source);P(screen,RDP_SCREEN,w.screen-(gaddr)(int32_t)(int16_t)w.stride);
  w=load_words(w,h,w.descriptor,0x0038);AW(secondary,RDP_SECONDARY,w.size);AW(source,RDP_SOURCE,w.offset);AW(stride,RDP_STRIDE,w.screen);store_words(h,w.registers+12,0x0038);AW(secondary,RDP_SECONDARY,w.value);AW(source,RDP_SOURCE,w.base);AW(stride,RDP_STRIDE,w.control);store_words(h,w.registers+18,0x0038);
  w=consume(h,RDP_SQUARE_FIRST_FACE);square_status(w,h);word(h,0xc45954,rd_u16(w.frame-0x5e));TB(rd_u8(0xc45786));if(!rd_u8(0xc45786))goto done;
  P(registers,RDP_REGISTERS,0xc4bf94);P(descriptor,RDP_DESCRIPTOR,0xc48390);longword(h,w.registers+12,rd_u32(w.registers+18));word(h,w.registers+16,rd_u16(w.registers+22));w=load_words(w,h,w.descriptor,0x10ff);
  SW(size,RDP_SIZE,w.secondary);SW(offset,RDP_OFFSET,w.source);P(screen,RDP_SCREEN,w.screen-(gaddr)(int32_t)(int16_t)w.stride);W(secondary,RDP_SECONDARY,w.value);W(source,RDP_SOURCE,w.base);W(stride,RDP_STRIDE,w.control);
  AW(value,RDP_VALUE,w.size);AW(base,RDP_BASE,w.offset);AW(control,RDP_CONTROL,w.screen);store_words(h,w.registers+6,7);SW(value,RDP_VALUE,rd_u16(w.registers+12));SW(base,RDP_BASE,rd_u16(w.registers+14));SW(control,RDP_CONTROL,rd_u16(w.registers+16));SW(secondary,RDP_SECONDARY,w.value);SW(source,RDP_SOURCE,w.base);SW(stride,RDP_STRIDE,w.control);store_words(h,w.registers+18,0x0038);
 } else {
  store_words(h,w.registers,0x10c0);SW(value,RDP_VALUE,w.secondary);SW(base,RDP_BASE,w.source);SW(control,RDP_CONTROL,w.stride);w=load_words(w,h,w.descriptor,0x0038);store_words(h,w.registers+6,0x0038);
  AW(secondary,RDP_SECONDARY,w.value);AW(source,RDP_SOURCE,w.base);AW(stride,RDP_STRIDE,w.control);AW(size,RDP_SIZE,w.value);AW(offset,RDP_OFFSET,w.base);P(screen,RDP_SCREEN,w.screen+(gaddr)(int32_t)(int16_t)w.control);store_words(h,w.registers+12,0x10f8);
  w=consume(h,RDP_SQUARE_ALTERNATE_FACE);square_status(w,h);word(h,0xc45954,rd_u16(w.frame-0x5e));TB(rd_u8(0xc45786));if(!rd_u8(0xc45786))goto done;
  P(registers,RDP_REGISTERS,0xc4bf94);P(descriptor,RDP_DESCRIPTOR,0xc48390);w=load_words(w,h,w.descriptor,7);store_words(h,w.registers,7);w=load_words(w,h,w.registers+12,0x10f8);store_words(h,w.registers+6,0x10c0);store_words(h,w.registers+18,0x0038);
  SW(secondary,RDP_SECONDARY,w.value);SW(source,RDP_SOURCE,w.base);SW(stride,RDP_STRIDE,w.control);AW(size,RDP_SIZE,w.secondary);AW(offset,RDP_OFFSET,w.source);P(screen,RDP_SCREEN,w.screen+(gaddr)(int32_t)(int16_t)w.stride);store_words(h,w.registers+12,0x10c0);
 }
 w=consume(h,RDP_SQUARE_SECOND_FACE);square_status(w,h);
done:w=restore_longs(w,h,0x2600);W(value,RDP_VALUE,rd_u16(w.frame-0x7e));return;
outside:L(value,RDP_VALUE,0xffffffffu);
}
