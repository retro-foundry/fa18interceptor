#include "face_list_parents.h"
#include <stdlib.h>
static void observe(const FaceListHooks *h,enum FaceListPhase p,enum FaceListField f,uint32_t v,uint32_t o) {
    if(h && h->observe) h->observe(h->context,p,f,v,o);
}
static FaceListState consume(const FaceListHooks *h,enum FaceListChild child) {
    if(h && h->consume) return h->consume(h->context,child); abort();
}
static FaceListState restored(const FaceListHooks *h) {
    if(h && h->restored) return h->restored(h->context); abort();
}
static uint32_t low_word(uint32_t old,uint16_t v) { return (old&0xffff0000u)|v; }
static uint32_t low_byte(uint32_t old,uint8_t v) { return (old&0xffffff00u)|v; }
#define W(f,id,v) do { w.f=low_word(w.f,(uint16_t)(v)); observe(h,FLS_WORD,id,w.f,0); } while(0)
#define B(f,id,v) do { w.f=low_byte(w.f,(uint8_t)(v)); observe(h,FLS_BYTE,id,w.f,0); } while(0)
#define L(f,id,v) do { w.f=(uint32_t)(v); observe(h,FLS_LONG,id,w.f,0); } while(0)
#define P(f,id,v) do { w.f=(gaddr)(v); observe(h,FLS_POINTER,id,w.f,0); } while(0)
#define AW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f+n_)); observe(h,FLS_ADD_WORD,id,n_,0); } while(0)
#define SW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f-n_)); observe(h,FLS_SUB_WORD,id,n_,0); } while(0)
#define AL(f,id,v) do { uint32_t n_=(uint32_t)(v); w.f+=n_; observe(h,FLS_ADD_LONG,id,n_,0); } while(0)
#define SL(f,id,v) do { uint32_t n_=(uint32_t)(v); w.f-=n_; observe(h,FLS_SUB_LONG,id,n_,0); } while(0)
#define ANDW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f&n_)); observe(h,FLS_AND_WORD,id,n_,0); } while(0)
#define ANDB(f,id,v) do { uint8_t n_=(uint8_t)(v); w.f=low_byte(w.f,(uint8_t)(w.f&n_)); observe(h,FLS_AND_BYTE,id,n_,0); } while(0)
#define ORW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f|n_)); observe(h,FLS_OR_WORD,id,n_,0); } while(0)
#define EL(f,id) do { w.f=(uint32_t)(int32_t)(int16_t)w.f; observe(h,FLS_EXT_LONG,id,0,0); } while(0)
#define NEG(f,id) do { w.f=low_word(w.f,(uint16_t)(0u-w.f)); observe(h,FLS_NEG_WORD,id,0,0); } while(0)
#define SWAP(f,id) do { w.f=(w.f<<16)|(w.f>>16); observe(h,FLS_SWAP,id,0,0); } while(0)
#define ASL(f,id,n) do { w.f=low_word(w.f,(uint16_t)((((unsigned)(n)&63u)<16?w.f<<((unsigned)(n)&63u):0))); observe(h,FLS_ASL_WORD,id,n,0); } while(0)
#define LSL(f,id,n) do { w.f=low_word(w.f,(uint16_t)((((unsigned)(n)&63u)<16?w.f<<((unsigned)(n)&63u):0))); observe(h,FLS_LSL_WORD,id,n,0); } while(0)
#define ASR(f,id,n) do { unsigned n_=(unsigned)(n)&63u; w.f=low_word(w.f,(uint16_t)((int16_t)w.f>>(n_<16?n_:15))); observe(h,FLS_ASR_WORD,id,n_,0); } while(0)
#define CW(v,n) observe(h,FLS_COMPARE_WORD,FLS_VALUE,(uint16_t)(n),(uint16_t)(v))
#define CB(v,n) observe(h,FLS_COMPARE_BYTE,FLS_VALUE,(uint8_t)(n),(uint8_t)(v))
#define TW(v) observe(h,FLS_TEST_WORD,FLS_VALUE,(uint16_t)(v),0)
#define TB(v) observe(h,FLS_TEST_BYTE,FLS_VALUE,(uint8_t)(v),0)
static void word(const FaceListHooks *h,gaddr a,uint16_t v) { wr_u16(a,v); observe(h,FLS_STORE_WORD,FLS_VALUE,v,0); }
static void byte(const FaceListHooks *h,gaddr a,uint8_t v) { wr_u8(a,v); observe(h,FLS_STORE_BYTE,FLS_VALUE,v,0); }
static void longword(const FaceListHooks *h,gaddr a,uint32_t v) { wr_u32(a,v); observe(h,FLS_STORE_LONG,FLS_VALUE,v,0); }
static void decrement_byte(const FaceListHooks *h,gaddr a) { uint8_t old=rd_u8(a); wr_u8(a,(uint8_t)(old-1)); observe(h,FLS_MEMORY_SUB_BYTE,FLS_VALUE,old,1); }
static int bit(const FaceListHooks *h,gaddr a,unsigned b) { uint8_t v=rd_u8(a); observe(h,FLS_BIT_TEST,FLS_VALUE,v,b); return (v>>b)&1; }

/* MOVEM transfers retain CCR; individual MOVE transfers above update it. */
#define RAW(f,id,v) do { w.f=(uint32_t)(v); observe(h,FLS_RAW_LONG,id,w.f,0); } while(0)
#define MUL(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=(uint32_t)((int32_t)(int16_t)w.f*(int16_t)n_); observe(h,FLS_MULS,id,n_,0); } while(0)
#define MUP(f,id,ptr,pi) do { MUL(f,id,rd_u16(w.ptr)); P(ptr,pi,w.ptr+2); } while(0)
#define ARL(f,id,n) do { unsigned n_=(unsigned)(n)&63u; w.f=(uint32_t)((int32_t)w.f>>(n_<32?n_:31)); observe(h,FLS_ASR_LONG,id,n_,0); } while(0)
#define DU(f,id,n) do { uint16_t n_=(uint16_t)(n); if(n_ && w.f/n_<=65535u) w.f=(w.f%n_)<<16|(w.f/n_); observe(h,FLS_DIVU,id,n_,0); } while(0)

#define AB(f,id,v) do { uint8_t n_=(uint8_t)(v); w.f=low_byte(w.f,(uint8_t)(w.f+n_)); observe(h,FLS_ADD_BYTE,id,n_,0); } while(0)
#define SB(f,id,v) do { uint8_t n_=(uint8_t)(v); w.f=low_byte(w.f,(uint8_t)(w.f-n_)); observe(h,FLS_SUB_BYTE,id,n_,0); } while(0)
#define EW(f,id) do { w.f=low_word(w.f,(uint16_t)(int16_t)(int8_t)w.f); observe(h,FLS_EXT_WORD,id,0,0); } while(0)
#define NL(f,id) do { w.f=0u-w.f; observe(h,FLS_NEG_LONG,id,0,0); } while(0)
#define CL(v,n) observe(h,FLS_COMPARE_LONG,FLS_VALUE,(uint32_t)(n),(uint32_t)(v))
#define TL(v) observe(h,FLS_TEST_LONG,FLS_VALUE,(uint32_t)(v),0)
static int memory_decrement(const FaceListHooks *h,gaddr at) {
    uint8_t v=rd_u8(at); wr_u8(at,(uint8_t)(v-1)); observe(h,FLS_MEMORY_SUB_BYTE,FLS_VALUE,v,1); return (int8_t)v-1>=0;
}
static void memory_increment(const FaceListHooks *h,gaddr at) {
    uint8_t v=rd_u8(at);wr_u8(at,(uint8_t)(v+1));observe(h,FLS_MEMORY_ADD_BYTE,FLS_VALUE,v,1);
}
static FaceListState load_words(FaceListState w,const FaceListHooks *h,gaddr at,uint16_t mask) {
    (void)w;observe(h,FLS_LOAD_WORDS_AT,FLS_VALUE,at,mask);return restored(h);
}
static FaceListState load_longs(FaceListState w,const FaceListHooks *h,gaddr at,uint16_t mask) {
    (void)w;observe(h,FLS_LOAD_LONGS_AT,FLS_VALUE,at,mask);return restored(h);
}
static void store_words(const FaceListHooks *h,gaddr at,uint16_t mask) { observe(h,FLS_STORE_WORDS_AT,FLS_VALUE,at,mask); }
static void store_longs(const FaceListHooks *h,gaddr at,uint16_t mask) { observe(h,FLS_STORE_LONGS_AT,FLS_VALUE,at,mask); }
static int decrement_word(const FaceListHooks *h,gaddr at) {
 uint16_t old=rd_u16(at);wr_u16(at,(uint16_t)(old-1));observe(h,FLS_MEMORY_SUB_WORD,FLS_VALUE,old,1);return (int16_t)old-1;
}
#define ALL(f,id,n) do { unsigned n_=(unsigned)(n)&63u; w.f=n_<32?w.f<<n_:0;observe(h,FLS_ASL_LONG,id,n_,0); } while(0)
static FaceListState save_longs(FaceListState w,const FaceListHooks *h,uint16_t mask) { observe(h,FLS_SAVE_LONGS,FLS_VALUE,mask,0);return restored(h); }
static FaceListState restore_longs(FaceListState w,const FaceListHooks *h,uint16_t mask) { observe(h,FLS_RESTORE_LONGS,FLS_VALUE,mask,0);return restored(h); }

static void result_word(FaceListState w,const FaceListHooks *h) {word(h,w.frame-0x7e,(uint16_t)(rd_u16(w.frame-0x7e)|w.value));}
static void increment_word(const FaceListHooks *h,gaddr at) {uint16_t old=rd_u16(at);wr_u16(at,(uint16_t)(old+1));observe(h,FLS_MEMORY_ADD_WORD,FLS_VALUE,old,1);}
static FaceListState push_long(FaceListState w,const FaceListHooks *h,uint32_t value) {observe(h,FLS_PUSH_LONG,FLS_VALUE,value,0);return restored(h);}
static FaceListState pop_pointer(FaceListState w,const FaceListHooks *h,enum FaceListField field) {observe(h,FLS_POP_POINTER,field,0,0);return restored(h);}
static void copy_vertex(FaceListState *state,const FaceListHooks *h,gaddr vertex,int advance) {
 FaceListState w=*state;longword(h,w.registers,rd_u32(vertex));P(registers,FLS_REGISTERS,w.registers+4);word(h,w.registers,rd_u16(vertex+4));if(advance)P(registers,FLS_REGISTERS,w.registers+2);*state=w;
}

static void style_words(FaceListState *state,const FaceListHooks *h,uint16_t planes,uint16_t colour,uint16_t complement) {
 FaceListState w=*state;W(value,FLS_VALUE,planes);W(base,FLS_BASE,colour);W(control,FLS_CONTROL,complement);W(secondary,FLS_SECONDARY,0);store_words(h,0xc456e6,15);*state=w;
}

/* C1FF0A: all three original corners and the actual predicate CCR. */
void face_list_test_stream_face(FaceListState w,const FaceListHooks *h) {
 P(registers,FLS_REGISTERS,0xc4bf94);P(descriptor,FLS_DESCRIPTOR,0xc48390);w=load_words(w,h,w.stream,7);P(stream,FLS_STREAM,w.stream+6);
 copy_vertex(&w,h,w.descriptor+(gaddr)(int32_t)(int16_t)w.value,1);copy_vertex(&w,h,w.descriptor+(gaddr)(int32_t)(int16_t)w.base,1);copy_vertex(&w,h,w.descriptor+(gaddr)(int32_t)(int16_t)w.control,1);
 P(descriptor,FLS_DESCRIPTOR,(gaddr)(int32_t)rd_s16(w.stream));P(stream,FLS_STREAM,w.stream+2);W(offset,FLS_OFFSET,w.descriptor);w=consume(h,FLS_STREAM_FACE_TEST);if(!w.zero)P(stream,FLS_STREAM,w.stream+18);
}

/* C200AE: shared tested-face tail, including changed child A2/A3. */
static void tested_face_tail(FaceListState w,const FaceListHooks *h) {
 P(descriptor,FLS_DESCRIPTOR,(gaddr)(int32_t)rd_s16(w.stream));P(stream,FLS_STREAM,w.stream+2);W(offset,FLS_OFFSET,w.descriptor);
 if((int16_t)w.offset>=0) {
  increment_word(h,w.frame-0x32);w=consume(h,FLS_TESTED_FACE_TEST);
  if(!w.zero){W(base,FLS_BASE,w.descriptor);ANDW(base,FLS_BASE,0x4000);if(!(uint16_t)w.base){L(value,FLS_VALUE,0xffffffffu);return;}W(offset,FLS_OFFSET,rd_u16(w.stream));P(stream,FLS_STREAM,w.stream+2);}
  else {increment_word(h,w.frame-0x34);W(base,FLS_BASE,w.descriptor);W(offset,FLS_OFFSET,w.base);ANDW(base,FLS_BASE,0x4000);if((uint16_t)w.base)P(stream,FLS_STREAM,w.stream+2);}
 }
 word(h,0xc45954,(uint16_t)w.offset);w=save_longs(w,h,0x0064);w=consume(h,FLS_TESTED_FACE_CLIP);w=restore_longs(w,h,0x2600);
}

/* C2005C's A4-indexed corners, preserving MOVE and AND ordering. */
static FaceListState indexed_corner(FaceListState w,const FaceListHooks *h,int first,uint16_t index) {
 P(screen,FLS_SCREEN,w.descriptor+(gaddr)(int32_t)(int16_t)index);longword(h,w.registers,rd_u32(w.screen));P(screen,FLS_SCREEN,w.screen+4);P(registers,FLS_REGISTERS,w.registers+4);
 if(first){W(size,FLS_SIZE,rd_u16(w.screen));word(h,w.registers,(uint16_t)w.size);}else word(h,w.registers,rd_u16(w.screen));
 P(registers,FLS_REGISTERS,w.registers+2);if(!first)ANDW(size,FLS_SIZE,rd_u16(w.screen));return w;
}
void face_list_tested_face(FaceListState w,const FaceListHooks *h) {
 P(registers,FLS_REGISTERS,0xc4bf90);word(h,w.registers,0);P(registers,FLS_REGISTERS,w.registers+2);P(registers,FLS_REGISTERS,w.registers+2);P(descriptor,FLS_DESCRIPTOR,0xc48390);L(offset,FLS_OFFSET,3);
 W(base,FLS_BASE,rd_u16(w.stream));P(stream,FLS_STREAM,w.stream+2);w=indexed_corner(w,h,1,(uint16_t)w.base);W(base,FLS_BASE,rd_u16(w.stream));P(stream,FLS_STREAM,w.stream+2);w=indexed_corner(w,h,0,(uint16_t)w.base);
 for(;;){W(base,FLS_BASE,rd_u16(w.stream));P(stream,FLS_STREAM,w.stream+2);if((int16_t)w.base<0)break;w=indexed_corner(w,h,0,(uint16_t)w.base);AW(offset,FLS_OFFSET,1);}
 ANDW(base,FLS_BASE,0x7fff);w=indexed_corner(w,h,0,(uint16_t)w.base);if((int16_t)w.size<0){L(value,FLS_VALUE,0xffffffffu);return;}
 word(h,0xc4bf92,(uint16_t)w.offset);tested_face_tail(w,h);
}
void face_list_tested_parallelogram(FaceListState w,const FaceListHooks *h) {
 P(descriptor,FLS_DESCRIPTOR,0xc48390);P(registers,FLS_REGISTERS,0xc4bf90);longword(h,w.registers,4);P(registers,FLS_REGISTERS,w.registers+4);w=load_words(w,h,w.stream,0x000e);P(stream,FLS_STREAM,w.stream+6);
 copy_vertex(&w,h,w.descriptor+(gaddr)(int32_t)(int16_t)w.base,1);copy_vertex(&w,h,w.descriptor+(gaddr)(int32_t)(int16_t)w.control,1);copy_vertex(&w,h,w.descriptor+(gaddr)(int32_t)(int16_t)w.secondary,1);
 w=load_words(w,h,w.registers-18,0x00fc);W(value,FLS_VALUE,w.source);ANDW(value,FLS_VALUE,w.offset);SW(stride,FLS_STRIDE,w.control);SW(size,FLS_SIZE,w.secondary);SW(offset,FLS_OFFSET,w.source);w=load_words(w,h,w.registers-6,0x001c);ANDW(value,FLS_VALUE,w.source);SW(control,FLS_CONTROL,w.stride);SW(secondary,FLS_SECONDARY,w.size);SW(source,FLS_SOURCE,w.offset);ANDW(value,FLS_VALUE,w.source);
 if((int16_t)w.value<0){L(value,FLS_VALUE,0xffffffffu);return;}store_words(h,w.registers,0x001c);tested_face_tail(w,h);
}

/* C20100: descriptor lists and every predicate/clip result. */
void face_list_indexed_face_list(FaceListState w,const FaceListHooks *h) {
 w=save_longs(w,h,0x0044);word(h,w.frame-0x7e,0);word(h,0xc4bf90,0);P(screen,FLS_SCREEN,rd_u32(w.stream));P(stream,FLS_STREAM,w.stream+4);
 for(;;) {
  W(base,FLS_BASE,rd_u16(w.stream));P(stream,FLS_STREAM,w.stream+2);if((int16_t)w.base<0)break;w=push_long(w,h,w.screen);P(screen,FLS_SCREEN,w.screen+(gaddr)(int32_t)(int16_t)w.base);P(registers,FLS_REGISTERS,0xc4bf94);P(descriptor,FLS_DESCRIPTOR,0xc48390);L(offset,FLS_OFFSET,3);
  W(base,FLS_BASE,rd_u16(w.screen));P(screen,FLS_SCREEN,w.screen+2);longword(h,w.registers,rd_u32(w.descriptor+(gaddr)(int32_t)(int16_t)w.base));P(registers,FLS_REGISTERS,w.registers+4);W(size,FLS_SIZE,rd_u16(w.descriptor+4+(gaddr)(int32_t)(int16_t)w.base));word(h,w.registers,(uint16_t)w.size);P(registers,FLS_REGISTERS,w.registers+2);
  W(base,FLS_BASE,rd_u16(w.screen));P(screen,FLS_SCREEN,w.screen+2);copy_vertex(&w,h,w.descriptor+(gaddr)(int32_t)(int16_t)w.base,1);ANDW(size,FLS_SIZE,rd_u16(w.registers-2));
  for(;;){W(base,FLS_BASE,rd_u16(w.screen));P(screen,FLS_SCREEN,w.screen+2);if((int16_t)w.base<0)break;copy_vertex(&w,h,w.descriptor+(gaddr)(int32_t)(int16_t)w.base,1);ANDW(size,FLS_SIZE,rd_u16(w.registers-2));AW(offset,FLS_OFFSET,1);}
  ANDW(base,FLS_BASE,0x7fff);copy_vertex(&w,h,w.descriptor+(gaddr)(int32_t)(int16_t)w.base,1);ANDW(size,FLS_SIZE,rd_u16(w.registers-2));if((int16_t)w.size<0)goto next_face;
  word(h,0xc4bf92,(uint16_t)w.offset);P(descriptor,FLS_DESCRIPTOR,(gaddr)(int32_t)rd_s16(w.screen));P(screen,FLS_SCREEN,w.screen+2);W(offset,FLS_OFFSET,w.descriptor);
  if((int16_t)w.offset>=0){w=consume(h,FLS_LIST_FACE_TEST);if(!w.zero){W(base,FLS_BASE,w.descriptor);ANDW(base,FLS_BASE,0x4000);if(!(uint16_t)w.base)goto next_face;W(offset,FLS_OFFSET,rd_u16(w.screen));P(screen,FLS_SCREEN,w.screen+2);}else W(offset,FLS_OFFSET,w.descriptor);}
  word(h,0xc45954,(uint16_t)w.offset);w=push_long(w,h,w.stream);w=consume(h,FLS_LIST_FACE_CLIP);result_word(w,h);w=pop_pointer(w,h,FLS_STREAM);
next_face:w=pop_pointer(w,h,FLS_SCREEN);
 }
 w=restore_longs(w,h,0x2200);W(value,FLS_VALUE,rd_u16(w.frame-0x7e));
}

/* C21060: source quad list, including skipped quads and every child. */
void face_list_quad_list(FaceListState w,const FaceListHooks *h) {
 w=save_longs(w,h,0x0044);word(h,0xc45954,12);style_words(&w,h,8,8,0);word(h,w.frame-0x7e,0);longword(h,0xc4bf90,4);
 for(;;){W(base,FLS_BASE,rd_u16(w.stream));P(stream,FLS_STREAM,w.stream+2);if((int16_t)w.base<0)break;w=load_words(w,h,w.stream,0x001c);P(stream,FLS_STREAM,w.stream+6);P(registers,FLS_REGISTERS,0xc4bf94);P(descriptor,FLS_DESCRIPTOR,0xc48390);w=indexed_corner(w,h,1,(uint16_t)w.base);
  w=indexed_corner(w,h,0,(uint16_t)w.control);w=indexed_corner(w,h,0,(uint16_t)w.secondary);w=indexed_corner(w,h,0,(uint16_t)w.source);
  if((int16_t)w.size<0)continue;w=push_long(w,h,w.stream);w=consume(h,FLS_QUAD_LIST);result_word(w,h);w=pop_pointer(w,h,FLS_STREAM);
 }
 w=restore_longs(w,h,0x2200);W(value,FLS_VALUE,rd_u16(w.frame-0x7e));
}

/* C21C4C: original midpoint and quarter-point operations. */
void face_list_split_edge(FaceListState w,const FaceListHooks *h) {
 w=load_words(w,h,w.descriptor,0x003f);SW(secondary,FLS_SECONDARY,w.value);SW(source,FLS_SOURCE,w.base);SW(stride,FLS_STRIDE,w.control);ASR(secondary,FLS_SECONDARY,1);ASR(source,FLS_SOURCE,1);ASR(stride,FLS_STRIDE,1);AW(value,FLS_VALUE,w.secondary);AW(base,FLS_BASE,w.source);AW(control,FLS_CONTROL,w.stride);store_words(h,w.descriptor+0x1e,7);
 SW(value,FLS_VALUE,w.secondary);SW(base,FLS_BASE,w.source);SW(control,FLS_CONTROL,w.stride);ASR(secondary,FLS_SECONDARY,1);ASR(source,FLS_SOURCE,1);ASR(stride,FLS_STRIDE,1);AW(value,FLS_VALUE,w.secondary);AW(base,FLS_BASE,w.source);AW(control,FLS_CONTROL,w.stride);store_words(h,w.descriptor+0x24,7);L(value,FLS_VALUE,0);
}
void face_list_split_record_edges(FaceListState w,const FaceListHooks *h) {
 P(descriptor,FLS_DESCRIPTOR,0xc46184);W(value,FLS_VALUE,rd_u16(0xc459b6));AW(value,FLS_VALUE,0xa4);P(descriptor,FLS_DESCRIPTOR,w.descriptor+(gaddr)(int32_t)(int16_t)w.value);w=consume(h,FLS_SPLIT_RECORD);P(descriptor,FLS_DESCRIPTOR,0xc48390);P(descriptor,FLS_DESCRIPTOR,w.descriptor+(gaddr)(int32_t)rd_s16(w.stream));P(stream,FLS_STREAM,w.stream+2);face_list_split_edge(w,h);
}

/* C219AE: all edge-derived writes, byte flags and original stream skip. */
void face_list_derive_edge_vertices(FaceListState w,const FaceListHooks *h) {
 P(descriptor,FLS_DESCRIPTOR,0xc48390);W(value,FLS_VALUE,rd_u16(w.stream));P(stream,FLS_STREAM,w.stream+2);W(base,FLS_BASE,rd_u16(w.stream));P(stream,FLS_STREAM,w.stream+2);w=load_words(w,h,w.descriptor+(gaddr)(int32_t)(int16_t)w.value,0x0038);w=load_words(w,h,w.descriptor+(gaddr)(int32_t)(int16_t)w.base,7);P(descriptor,FLS_DESCRIPTOR,w.descriptor+(gaddr)(int32_t)rd_s16(w.stream));P(stream,FLS_STREAM,w.stream+2);
 SW(value,FLS_VALUE,w.secondary);SW(base,FLS_BASE,w.source);SW(control,FLS_CONTROL,w.stride);w=load_words(w,h,w.descriptor,0x0038);AW(secondary,FLS_SECONDARY,w.value);AW(source,FLS_SOURCE,w.base);AW(stride,FLS_STRIDE,w.control);store_words(h,w.descriptor+0x12,0x0038);ASR(value,FLS_VALUE,1);ASR(base,FLS_BASE,1);ASR(control,FLS_CONTROL,1);w=load_words(w,h,w.descriptor+6,0x10f8);
 AW(secondary,FLS_SECONDARY,w.value);AW(source,FLS_SOURCE,w.base);AW(stride,FLS_STRIDE,w.control);AW(size,FLS_SIZE,w.value);AW(offset,FLS_OFFSET,w.base);P(screen,FLS_SCREEN,w.screen+(gaddr)(int32_t)(int16_t)w.control);store_words(h,w.descriptor+0x18,0x10f8);P(descriptor,FLS_DESCRIPTOR,0xc46184);P(descriptor,FLS_DESCRIPTOR,w.descriptor+(gaddr)(int32_t)rd_s16(0xc459b6));B(base,FLS_BASE,rd_u8(w.descriptor+0x7c));observe(h,FLS_BIT_CLEAR,FLS_BASE,w.base,7);w.base&=~0x80u;
 w.base=low_byte(w.base,(uint8_t)((int8_t)w.base>>4));observe(h,FLS_ASR_BYTE,FLS_BASE,4,0);EW(base,FLS_BASE);AW(base,FLS_BASE,w.base);W(value,FLS_VALUE,w.base);ASL(base,FLS_BASE,3);SW(base,FLS_BASE,w.value);P(stream,FLS_STREAM,w.stream+(gaddr)(int32_t)(int16_t)w.base);L(value,FLS_VALUE,0);
}

/* C2084A: normalize both vectors, then consume the original signed test. */
void face_list_edge_alignment(FaceListState w,const FaceListHooks *h) {
 int64_t sum;
 W(stride,FLS_STRIDE,rd_u16(w.descriptor));P(descriptor,FLS_DESCRIPTOR,w.descriptor+2);W(offset,FLS_OFFSET,rd_u16(w.descriptor));P(descriptor,FLS_DESCRIPTOR,w.descriptor+2);SW(stride,FLS_STRIDE,rd_u16(w.descriptor));P(descriptor,FLS_DESCRIPTOR,w.descriptor+2);SW(offset,FLS_OFFSET,rd_u16(w.descriptor));P(descriptor,FLS_DESCRIPTOR,w.descriptor+2);NEG(stride,FLS_STRIDE);NEG(offset,FLS_OFFSET);W(size,FLS_SIZE,0);W(value,FLS_VALUE,256);w=consume(h,FLS_ALIGNMENT_EDGE);
 observe(h,FLS_SAVE_WORDS,FLS_VALUE,0x0700,0);w=restored(h);W(stride,FLS_STRIDE,rd_u16(w.frame-0x26));SW(stride,FLS_STRIDE,rd_u16(0xc45b2a));W(size,FLS_SIZE,0);W(offset,FLS_OFFSET,rd_u16(w.frame-0x22));SW(offset,FLS_OFFSET,rd_u16(0xc45b2e));W(value,FLS_VALUE,256);w=consume(h,FLS_ALIGNMENT_EYE);observe(h,FLS_RESTORE_WORDS,FLS_VALUE,7,0);w=restored(h);
 MUL(value,FLS_VALUE,w.stride);MUL(base,FLS_BASE,w.size);MUL(control,FLS_CONTROL,w.offset);AL(control,FLS_CONTROL,w.value);sum=(int64_t)(int32_t)w.control+(int32_t)w.base;AL(control,FLS_CONTROL,w.base);if(sum<0)NL(control,FLS_CONTROL);ARL(control,FLS_CONTROL,4);P(descriptor,FLS_DESCRIPTOR,0xc208d4);CL((uint32_t)-128,rd_u32(0xc45a78));if(rd_s32(0xc45a78)<=-128)P(descriptor,FLS_DESCRIPTOR,0xc208ec);
 W(value,FLS_VALUE,rd_u16(w.frame-0x28));ASR(value,FLS_VALUE,4);CW(10,w.value);if((int16_t)w.value>10)W(value,FLS_VALUE,11);AW(value,FLS_VALUE,w.value);W(base,FLS_BASE,rd_u16(w.descriptor+(gaddr)(int32_t)(int16_t)w.value));CW(w.base,w.control);L(value,FLS_VALUE,(int16_t)w.control<(int16_t)w.base?0xffffffffu:0);
}
void face_list_edge_alignment_test(FaceListState w,const FaceListHooks *h) {
 P(descriptor,FLS_DESCRIPTOR,rd_u32(0xc45a32));P(descriptor,FLS_DESCRIPTOR,w.descriptor+10);P(descriptor,FLS_DESCRIPTOR,w.descriptor+(gaddr)(int32_t)rd_s16(w.stream));P(stream,FLS_STREAM,w.stream+2);CL((uint32_t)-320,rd_u32(0xc45a78));if(rd_s32(0xc45a78)<=-320){L(value,FLS_VALUE,0);return;}TB(rd_u8(0xc4586c));if(rd_u8(0xc4586c)){L(value,FLS_VALUE,0);return;}face_list_edge_alignment(w,h);
}

/* C20C5A: shared grid tail; source frame counts and every quad child. */
static void face_grid_body(FaceListState w,const FaceListHooks *h) {
 P(descriptor,FLS_DESCRIPTOR,0xc48390);P(descriptor,FLS_DESCRIPTOR,w.descriptor+(gaddr)(int32_t)rd_s16(w.stream));P(stream,FLS_STREAM,w.stream+2);word(h,w.frame-0x38,rd_u16(w.stream));P(stream,FLS_STREAM,w.stream+2);w=load_words(w,h,w.descriptor,0x007e);P(descriptor,FLS_DESCRIPTOR,w.descriptor+12);
 SW(base,FLS_BASE,rd_u16(w.descriptor+6));SW(control,FLS_CONTROL,rd_u16(w.descriptor+8));SW(secondary,FLS_SECONDARY,rd_u16(w.descriptor+10));SW(source,FLS_SOURCE,rd_u16(w.descriptor+6));SW(stride,FLS_STRIDE,rd_u16(w.descriptor+8));SW(size,FLS_SIZE,rd_u16(w.descriptor+10));store_words(h,w.frame-0x46,0x007e);
 w=load_words(w,h,w.descriptor,0x000e);P(descriptor,FLS_DESCRIPTOR,w.descriptor+6);SW(base,FLS_BASE,rd_u16(w.descriptor));SW(control,FLS_CONTROL,rd_u16(w.descriptor+2));SW(secondary,FLS_SECONDARY,rd_u16(w.descriptor+4));store_words(h,w.frame-0x52,0x000e);word(h,w.frame-0x7e,0);P(registers,FLS_REGISTERS,0xc4bf90);longword(h,w.registers,4);P(registers,FLS_REGISTERS,w.registers+4);
 for(;;) {
  word(h,w.frame-0x3a,rd_u16(w.stream));P(stream,FLS_STREAM,w.stream+2);W(source,FLS_SOURCE,0);W(stride,FLS_STRIDE,0);W(size,FLS_SIZE,0);
  for(;;) {
   store_words(h,w.frame-0x58,0x0070);P(registers,FLS_REGISTERS,0xc4bf94);w=load_words(w,h,w.descriptor,0x000e);ASR(source,FLS_SOURCE,1);ASR(stride,FLS_STRIDE,1);ASR(size,FLS_SIZE,1);AW(base,FLS_BASE,w.source);AW(control,FLS_CONTROL,w.stride);AW(secondary,FLS_SECONDARY,w.size);W(source,FLS_SOURCE,w.base);W(stride,FLS_STRIDE,w.control);W(size,FLS_SIZE,w.secondary);AW(source,FLS_SOURCE,rd_u16(w.frame-0x46));AW(stride,FLS_STRIDE,rd_u16(w.frame-0x44));AW(size,FLS_SIZE,rd_u16(w.frame-0x42));store_words(h,w.registers,0x007e);P(registers,FLS_REGISTERS,w.registers+12);
   w=load_words(w,h,w.descriptor,0x0070);AW(source,FLS_SOURCE,rd_u16(w.frame-0x52));AW(stride,FLS_STRIDE,rd_u16(w.frame-0x50));AW(size,FLS_SIZE,rd_u16(w.frame-0x4e));w=load_words(w,h,w.frame-0x58,0x000e);ASR(base,FLS_BASE,1);ASR(control,FLS_CONTROL,1);ASR(secondary,FLS_SECONDARY,1);AW(source,FLS_SOURCE,w.base);AW(stride,FLS_STRIDE,w.control);AW(size,FLS_SIZE,w.secondary);W(base,FLS_BASE,w.source);W(control,FLS_CONTROL,w.stride);W(secondary,FLS_SECONDARY,w.size);AW(base,FLS_BASE,rd_u16(w.frame-0x46));AW(control,FLS_CONTROL,rd_u16(w.frame-0x44));AW(secondary,FLS_SECONDARY,rd_u16(w.frame-0x42));store_words(h,w.registers,0x007e);
   w=save_longs(w,h,0x0030);w=consume(h,FLS_FACE_GRID);result_word(w,h);w=restore_longs(w,h,0x0c00);if(decrement_word(h,w.frame-0x3a)<=0)break;w=load_words(w,h,w.frame-0x58,0x0070);AW(source,FLS_SOURCE,rd_u16(w.frame-0x40));AW(stride,FLS_STRIDE,rd_u16(w.frame-0x3e));AW(size,FLS_SIZE,rd_u16(w.frame-0x3c));
  }
  if(decrement_word(h,w.frame-0x38)<=0)break;P(descriptor,FLS_DESCRIPTOR,w.descriptor+6);
 }
 w=restore_longs(w,h,0x2200);W(value,FLS_VALUE,rd_u16(w.frame-0x7e));
}
void face_list_face_grid(FaceListState w,const FaceListHooks *h) {w=save_longs(w,h,0x0044);word(h,0xc45954,13);style_words(&w,h,2,0,2);face_grid_body(w,h);}
void face_list_face_grid_plain(FaceListState w,const FaceListHooks *h) {w=save_longs(w,h,0x0044);longword(h,0xc456e6,0x000fffffu);word(h,0xc45954,rd_u16(w.stream));P(stream,FLS_STREAM,w.stream+2);face_grid_body(w,h);}

/* C20A70: both lattice passes, with the original missing second-pass OR
 * retained. Only the first pass saves A3/A4 around its clip child. */
static void face_lattice_body(FaceListState w,const FaceListHooks *h) {
 unsigned pass;
 P(descriptor,FLS_DESCRIPTOR,0xc48390);P(descriptor,FLS_DESCRIPTOR,w.descriptor+(gaddr)(int32_t)rd_s16(w.stream));P(stream,FLS_STREAM,w.stream+2);word(h,w.frame-0x38,rd_u16(w.stream));word(h,w.frame-0x3a,rd_u16(w.stream));P(stream,FLS_STREAM,w.stream+2);w=save_longs(w,h,0x0064);w=load_words(w,h,w.descriptor,0x007e);P(descriptor,FLS_DESCRIPTOR,w.descriptor+12);P(screen,FLS_SCREEN,w.descriptor+6);
 SW(base,FLS_BASE,rd_u16(w.descriptor));SW(control,FLS_CONTROL,rd_u16(w.descriptor+2));SW(secondary,FLS_SECONDARY,rd_u16(w.descriptor+4));SW(source,FLS_SOURCE,rd_u16(w.descriptor));SW(stride,FLS_STRIDE,rd_u16(w.descriptor+2));SW(size,FLS_SIZE,rd_u16(w.descriptor+4));store_words(h,w.frame-0x46,0x007e);ASR(source,FLS_SOURCE,1);ASR(stride,FLS_STRIDE,1);ASR(size,FLS_SIZE,1);
 w=load_words(w,h,w.descriptor,0x000e);SW(base,FLS_BASE,w.source);SW(control,FLS_CONTROL,w.stride);SW(secondary,FLS_SECONDARY,w.size);AW(base,FLS_BASE,rd_u16(w.frame-0x46));AW(control,FLS_CONTROL,rd_u16(w.frame-0x44));AW(secondary,FLS_SECONDARY,rd_u16(w.frame-0x42));store_words(h,w.frame-0x4c,0x000e);
 w=load_words(w,h,w.screen,0x000e);SW(base,FLS_BASE,w.source);SW(control,FLS_CONTROL,w.stride);SW(secondary,FLS_SECONDARY,w.size);AW(base,FLS_BASE,rd_u16(w.frame-0x46));AW(control,FLS_CONTROL,rd_u16(w.frame-0x44));AW(secondary,FLS_SECONDARY,rd_u16(w.frame-0x42));store_words(h,w.frame-0x52,0x000e);word(h,w.frame-0x7e,0);P(registers,FLS_REGISTERS,0xc4bf90);longword(h,w.registers,4);P(registers,FLS_REGISTERS,w.registers+4);
 for(pass=0;pass<2;++pass) {
  W(source,FLS_SOURCE,0);W(stride,FLS_STRIDE,0);W(size,FLS_SIZE,0);
  for(;;) {
   store_words(h,w.frame-0x58,0x0070);P(registers,FLS_REGISTERS,0xc4bf94);w=load_words(w,h,pass?w.frame-0x4c:w.descriptor,0x000e);ASR(source,FLS_SOURCE,1);ASR(stride,FLS_STRIDE,1);ASR(size,FLS_SIZE,1);
   if(!pass){AW(base,FLS_BASE,w.source);AW(control,FLS_CONTROL,w.stride);AW(secondary,FLS_SECONDARY,w.size);}else{SW(base,FLS_BASE,w.source);SW(control,FLS_CONTROL,w.stride);SW(secondary,FLS_SECONDARY,w.size);}W(source,FLS_SOURCE,w.base);W(stride,FLS_STRIDE,w.control);W(size,FLS_SIZE,w.secondary);
   if(!pass){AW(source,FLS_SOURCE,rd_u16(w.frame-0x46));AW(stride,FLS_STRIDE,rd_u16(w.frame-0x44));AW(size,FLS_SIZE,rd_u16(w.frame-0x42));}else{SW(source,FLS_SOURCE,rd_u16(w.frame-0x46));SW(stride,FLS_STRIDE,rd_u16(w.frame-0x44));SW(size,FLS_SIZE,rd_u16(w.frame-0x42));}store_words(h,w.registers,0x007e);P(registers,FLS_REGISTERS,w.registers+12);
   w=load_words(w,h,pass?w.frame-0x52:w.screen,0x0070);w=load_words(w,h,w.frame-0x58,0x000e);ASR(base,FLS_BASE,1);ASR(control,FLS_CONTROL,1);ASR(secondary,FLS_SECONDARY,1);
   if(!pass){AW(source,FLS_SOURCE,w.base);AW(stride,FLS_STRIDE,w.control);AW(size,FLS_SIZE,w.secondary);}else{SW(source,FLS_SOURCE,w.base);SW(stride,FLS_STRIDE,w.control);SW(size,FLS_SIZE,w.secondary);}W(base,FLS_BASE,w.source);W(control,FLS_CONTROL,w.stride);W(secondary,FLS_SECONDARY,w.size);
   if(!pass){AW(base,FLS_BASE,rd_u16(w.frame-0x46));AW(control,FLS_CONTROL,rd_u16(w.frame-0x44));AW(secondary,FLS_SECONDARY,rd_u16(w.frame-0x42));}else{SW(base,FLS_BASE,rd_u16(w.frame-0x46));SW(control,FLS_CONTROL,rd_u16(w.frame-0x44));SW(secondary,FLS_SECONDARY,rd_u16(w.frame-0x42));}store_words(h,w.registers,0x007e);
   if(!pass)w=save_longs(w,h,0x0018);w=consume(h,pass?FLS_LATTICE_SECOND:FLS_LATTICE_FIRST);if(!pass){result_word(w,h);w=restore_longs(w,h,0x1800);}if(decrement_word(h,w.frame-(pass?0x3au:0x38u))<=0)break;w=load_words(w,h,w.frame-0x58,0x0070);AW(source,FLS_SOURCE,rd_u16(w.frame-0x40));AW(stride,FLS_STRIDE,rd_u16(w.frame-0x3e));AW(size,FLS_SIZE,rd_u16(w.frame-0x3c));
  }
 }
 w=restore_longs(w,h,0x2600);W(value,FLS_VALUE,rd_u16(w.frame-0x7e));
}
void face_list_face_lattice(FaceListState w,const FaceListHooks *h) {word(h,0xc45954,13);style_words(&w,h,2,0,2);face_lattice_body(w,h);}
void face_list_face_lattice_plain(FaceListState w,const FaceListHooks *h) {longword(h,0xc456e6,0x000fffffu);word(h,0xc45954,rd_u16(w.stream));P(stream,FLS_STREAM,w.stream+2);face_lattice_body(w,h);}
