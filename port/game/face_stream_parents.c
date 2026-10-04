#include "face_stream_parents.h"
#include <stdlib.h>
static void observe(const FaceStreamHooks *h,enum FaceStreamPhase p,enum FaceStreamField f,uint32_t v,uint32_t o) {
    if(h && h->observe) h->observe(h->context,p,f,v,o);
}
static FaceStreamState consume(const FaceStreamHooks *h,enum FaceStreamChild child) {
    if(h && h->consume) return h->consume(h->context,child); abort();
}
static FaceStreamState restored(const FaceStreamHooks *h) {
    if(h && h->restored) return h->restored(h->context); abort();
}
static uint32_t low_word(uint32_t old,uint16_t v) { return (old&0xffff0000u)|v; }
static uint32_t low_byte(uint32_t old,uint8_t v) { return (old&0xffffff00u)|v; }
#define W(f,id,v) do { w.f=low_word(w.f,(uint16_t)(v)); observe(h,FSP_WORD,id,w.f,0); } while(0)
#define B(f,id,v) do { w.f=low_byte(w.f,(uint8_t)(v)); observe(h,FSP_BYTE,id,w.f,0); } while(0)
#define L(f,id,v) do { w.f=(uint32_t)(v); observe(h,FSP_LONG,id,w.f,0); } while(0)
#define P(f,id,v) do { w.f=(gaddr)(v); observe(h,FSP_POINTER,id,w.f,0); } while(0)
#define AW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f+n_)); observe(h,FSP_ADD_WORD,id,n_,0); } while(0)
#define SW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f-n_)); observe(h,FSP_SUB_WORD,id,n_,0); } while(0)
#define AL(f,id,v) do { uint32_t n_=(uint32_t)(v); w.f+=n_; observe(h,FSP_ADD_LONG,id,n_,0); } while(0)
#define SL(f,id,v) do { uint32_t n_=(uint32_t)(v); w.f-=n_; observe(h,FSP_SUB_LONG,id,n_,0); } while(0)
#define ANDW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f&n_)); observe(h,FSP_AND_WORD,id,n_,0); } while(0)
#define ANDB(f,id,v) do { uint8_t n_=(uint8_t)(v); w.f=low_byte(w.f,(uint8_t)(w.f&n_)); observe(h,FSP_AND_BYTE,id,n_,0); } while(0)
#define ORW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f|n_)); observe(h,FSP_OR_WORD,id,n_,0); } while(0)
#define EL(f,id) do { w.f=(uint32_t)(int32_t)(int16_t)w.f; observe(h,FSP_EXT_LONG,id,0,0); } while(0)
#define NEG(f,id) do { w.f=low_word(w.f,(uint16_t)(0u-w.f)); observe(h,FSP_NEG_WORD,id,0,0); } while(0)
#define SWAP(f,id) do { w.f=(w.f<<16)|(w.f>>16); observe(h,FSP_SWAP,id,0,0); } while(0)
#define ASL(f,id,n) do { w.f=low_word(w.f,(uint16_t)((((unsigned)(n)&63u)<16?w.f<<((unsigned)(n)&63u):0))); observe(h,FSP_ASL_WORD,id,n,0); } while(0)
#define LSL(f,id,n) do { w.f=low_word(w.f,(uint16_t)((((unsigned)(n)&63u)<16?w.f<<((unsigned)(n)&63u):0))); observe(h,FSP_LSL_WORD,id,n,0); } while(0)
#define ASR(f,id,n) do { unsigned n_=(unsigned)(n)&63u; w.f=low_word(w.f,(uint16_t)((int16_t)w.f>>(n_<16?n_:15))); observe(h,FSP_ASR_WORD,id,n_,0); } while(0)
#define CW(v,n) observe(h,FSP_COMPARE_WORD,FSP_VALUE,(uint16_t)(n),(uint16_t)(v))
#define CB(v,n) observe(h,FSP_COMPARE_BYTE,FSP_VALUE,(uint8_t)(n),(uint8_t)(v))
#define TW(v) observe(h,FSP_TEST_WORD,FSP_VALUE,(uint16_t)(v),0)
#define TB(v) observe(h,FSP_TEST_BYTE,FSP_VALUE,(uint8_t)(v),0)
static void word(const FaceStreamHooks *h,gaddr a,uint16_t v) { wr_u16(a,v); observe(h,FSP_STORE_WORD,FSP_VALUE,v,0); }
static void byte(const FaceStreamHooks *h,gaddr a,uint8_t v) { wr_u8(a,v); observe(h,FSP_STORE_BYTE,FSP_VALUE,v,0); }
static void longword(const FaceStreamHooks *h,gaddr a,uint32_t v) { wr_u32(a,v); observe(h,FSP_STORE_LONG,FSP_VALUE,v,0); }
static void decrement_byte(const FaceStreamHooks *h,gaddr a) { uint8_t old=rd_u8(a); wr_u8(a,(uint8_t)(old-1)); observe(h,FSP_MEMORY_SUB_BYTE,FSP_VALUE,old,1); }
static int bit(const FaceStreamHooks *h,gaddr a,unsigned b) { uint8_t v=rd_u8(a); observe(h,FSP_BIT_TEST,FSP_VALUE,v,b); return (v>>b)&1; }

/* MOVEM transfers retain CCR; individual MOVE transfers above update it. */
#define RAW(f,id,v) do { w.f=(uint32_t)(v); observe(h,FSP_RAW_LONG,id,w.f,0); } while(0)
#define MUL(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=(uint32_t)((int32_t)(int16_t)w.f*(int16_t)n_); observe(h,FSP_MULS,id,n_,0); } while(0)
#define MUP(f,id,ptr,pi) do { MUL(f,id,rd_u16(w.ptr)); P(ptr,pi,w.ptr+2); } while(0)
#define ARL(f,id,n) do { unsigned n_=(unsigned)(n)&63u; w.f=(uint32_t)((int32_t)w.f>>(n_<32?n_:31)); observe(h,FSP_ASR_LONG,id,n_,0); } while(0)
#define DU(f,id,n) do { uint16_t n_=(uint16_t)(n); if(n_ && w.f/n_<=65535u) w.f=(w.f%n_)<<16|(w.f/n_); observe(h,FSP_DIVU,id,n_,0); } while(0)

#define AB(f,id,v) do { uint8_t n_=(uint8_t)(v); w.f=low_byte(w.f,(uint8_t)(w.f+n_)); observe(h,FSP_ADD_BYTE,id,n_,0); } while(0)
#define SB(f,id,v) do { uint8_t n_=(uint8_t)(v); w.f=low_byte(w.f,(uint8_t)(w.f-n_)); observe(h,FSP_SUB_BYTE,id,n_,0); } while(0)
#define EW(f,id) do { w.f=low_word(w.f,(uint16_t)(int16_t)(int8_t)w.f); observe(h,FSP_EXT_WORD,id,0,0); } while(0)
#define NL(f,id) do { w.f=0u-w.f; observe(h,FSP_NEG_LONG,id,0,0); } while(0)
#define CL(v,n) observe(h,FSP_COMPARE_LONG,FSP_VALUE,(uint32_t)(n),(uint32_t)(v))
#define TL(v) observe(h,FSP_TEST_LONG,FSP_VALUE,(uint32_t)(v),0)
static int memory_decrement(const FaceStreamHooks *h,gaddr at) {
    uint8_t v=rd_u8(at); wr_u8(at,(uint8_t)(v-1)); observe(h,FSP_MEMORY_SUB_BYTE,FSP_VALUE,v,1); return (int8_t)v-1>=0;
}
static void memory_increment(const FaceStreamHooks *h,gaddr at) {
    uint8_t v=rd_u8(at);wr_u8(at,(uint8_t)(v+1));observe(h,FSP_MEMORY_ADD_BYTE,FSP_VALUE,v,1);
}
static FaceStreamState load_words(FaceStreamState w,const FaceStreamHooks *h,gaddr at,uint16_t mask) {
    (void)w;observe(h,FSP_LOAD_WORDS_AT,FSP_VALUE,at,mask);return restored(h);
}
static FaceStreamState load_longs(FaceStreamState w,const FaceStreamHooks *h,gaddr at,uint16_t mask) {
    (void)w;observe(h,FSP_LOAD_LONGS_AT,FSP_VALUE,at,mask);return restored(h);
}
static void store_words(const FaceStreamHooks *h,gaddr at,uint16_t mask) { observe(h,FSP_STORE_WORDS_AT,FSP_VALUE,at,mask); }
static void store_longs(const FaceStreamHooks *h,gaddr at,uint16_t mask) { observe(h,FSP_STORE_LONGS_AT,FSP_VALUE,at,mask); }
static int decrement_word(const FaceStreamHooks *h,gaddr at) {
 uint16_t old=rd_u16(at);wr_u16(at,(uint16_t)(old-1));observe(h,FSP_MEMORY_SUB_WORD,FSP_VALUE,old,1);return (int16_t)old-1;
}
#define ALL(f,id,n) do { unsigned n_=(unsigned)(n)&63u; w.f=n_<32?w.f<<n_:0;observe(h,FSP_ASL_LONG,id,n_,0); } while(0)
static FaceStreamState save_longs(FaceStreamState w,const FaceStreamHooks *h,uint16_t mask) { observe(h,FSP_SAVE_LONGS,FSP_VALUE,mask,0);return restored(h); }
static FaceStreamState restore_longs(FaceStreamState w,const FaceStreamHooks *h,uint16_t mask) { observe(h,FSP_RESTORE_LONGS,FSP_VALUE,mask,0);return restored(h); }

static void result_word(FaceStreamState w,const FaceStreamHooks *h) {word(h,w.frame-0x7e,(uint16_t)(rd_u16(w.frame-0x7e)|w.value));}
static void increment_word(const FaceStreamHooks *h,gaddr at) {uint16_t old=rd_u16(at);wr_u16(at,(uint16_t)(old+1));observe(h,FSP_MEMORY_ADD_WORD,FSP_VALUE,old,1);}
static FaceStreamState push_long(FaceStreamState w,const FaceStreamHooks *h,uint32_t value) {observe(h,FSP_PUSH_LONG,FSP_VALUE,value,0);return restored(h);}
static FaceStreamState pop_pointer(FaceStreamState w,const FaceStreamHooks *h,enum FaceStreamField field) {observe(h,FSP_POP_POINTER,field,0,0);return restored(h);}
static void copy_vertex(FaceStreamState *state,const FaceStreamHooks *h,gaddr vertex,int advance) {
 FaceStreamState w=*state;longword(h,w.registers,rd_u32(vertex));P(registers,FSP_REGISTERS,w.registers+4);word(h,w.registers,rd_u16(vertex+4));if(advance)P(registers,FSP_REGISTERS,w.registers+2);*state=w;
}

/* C212B0: the colour and every source pair, including rejected pairs. */
void face_stream_segment_pairs(FaceStreamState w,const FaceStreamHooks *h) {
 w=save_longs(w,h,0x0044);longword(h,0xc456e6,0xffffffffu);word(h,0xc45954,rd_u16(w.stream));P(stream,FSP_STREAM,w.stream+2);word(h,w.frame-0x7e,0);word(h,w.frame-0x6e,0);
 for(;;) {
  TW(rd_u16(w.frame-0x6e));if(rd_u16(w.frame-0x6e))break;
  W(base,FSP_BASE,rd_u16(w.stream));P(stream,FSP_STREAM,w.stream+2);W(control,FSP_CONTROL,rd_u16(w.stream));P(stream,FSP_STREAM,w.stream+2);
  if((int16_t)w.control<0){increment_word(h,w.frame-0x6e);ANDW(control,FSP_CONTROL,0x7fff);}
  P(descriptor,FSP_DESCRIPTOR,0xc48390);P(registers,FSP_REGISTERS,0xc4c592);P(screen,FSP_SCREEN,w.descriptor+(gaddr)(int32_t)(int16_t)w.base);
  longword(h,w.registers,rd_u32(w.screen));P(screen,FSP_SCREEN,w.screen+4);P(registers,FSP_REGISTERS,w.registers+4);W(size,FSP_SIZE,rd_u16(w.screen));word(h,w.registers,(uint16_t)w.size);P(registers,FSP_REGISTERS,w.registers+2);
  P(screen,FSP_SCREEN,w.descriptor+(gaddr)(int32_t)(int16_t)w.control);longword(h,w.registers,rd_u32(w.screen));P(screen,FSP_SCREEN,w.screen+4);P(registers,FSP_REGISTERS,w.registers+4);ANDW(size,FSP_SIZE,rd_u16(w.screen));if((int16_t)w.size<0)continue;
  word(h,w.registers,rd_u16(w.screen));w=push_long(w,h,w.stream);w=consume(h,FSP_SEGMENT_PAIRS);result_word(w,h);w=pop_pointer(w,h,FSP_STREAM);
 }
 w=restore_longs(w,h,0x2200);W(value,FSP_VALUE,rd_u16(w.frame-0x7e));
}

/* C2129C: near-plane prefix and the shared C212B0 body. */
void face_stream_segment_pairs_near(FaceStreamState w,const FaceStreamHooks *h) {
 CL((uint32_t)-192,rd_u32(0xc45a78));if(rd_s32(0xc45a78)<-192){face_stream_segment_pairs(w,h);return;}
 for(;;){uint16_t v=rd_u16(w.stream);TW(v);P(stream,FSP_STREAM,w.stream+2);if((int16_t)v<0)break;}
 L(value,FSP_VALUE,0);
}

/* C211DC: all counted segment pairs and returned result accumulation. */
void face_stream_segment_run(FaceStreamState w,const FaceStreamHooks *h) {
 W(value,FSP_VALUE,rd_u16(w.stream));P(stream,FSP_STREAM,w.stream+2);W(base,FSP_BASE,w.value);ANDW(base,FSP_BASE,63);word(h,0xc45954,(uint16_t)w.base);w.value=low_word(w.value,(uint16_t)w.value>>8);observe(h,FSP_LSR_WORD,FSP_VALUE,8,0);word(h,w.frame-0x30,(uint16_t)w.value);
 P(descriptor,FSP_DESCRIPTOR,0xc48390);P(descriptor,FSP_DESCRIPTOR,w.descriptor+(gaddr)(int32_t)rd_s16(w.stream));P(stream,FSP_STREAM,w.stream+2);w=save_longs(w,h,0x0064);word(h,w.frame-0x7e,0);
 do {w=load_words(w,h,w.descriptor,0x003f);P(descriptor,FSP_DESCRIPTOR,w.descriptor+12);store_words(h,0xc4c592,0x003f);w=push_long(w,h,w.descriptor);w=consume(h,FSP_SEGMENT_RUN);result_word(w,h);w=pop_pointer(w,h,FSP_DESCRIPTOR);}while(decrement_word(h,w.frame-0x30)>0);
 w=restore_longs(w,h,0x2600);W(value,FSP_VALUE,rd_u16(w.frame-0x7e));
}

/* C2131C: every offset pair, actual final-pair RAM and saved base word. */
void face_stream_offset_segments(FaceStreamState w,const FaceStreamHooks *h) {
 w=save_longs(w,h,0x0044);word(h,0xc45954,rd_u16(w.stream));P(stream,FSP_STREAM,w.stream+2);W(value,FSP_VALUE,rd_u16(w.stream));P(stream,FSP_STREAM,w.stream+2);word(h,w.frame-0x7e,0);word(h,w.frame-0x6e,0);
 for(;;) {
  TW(rd_u16(w.frame-0x6e));if(rd_u16(w.frame-0x6e))break;W(base,FSP_BASE,rd_u16(w.stream));P(stream,FSP_STREAM,w.stream+2);TW(rd_u16(w.stream));if(rd_s16(w.stream)<0)increment_word(h,w.frame-0x6e);
  P(descriptor,FSP_DESCRIPTOR,0xc48390);w=load_words(w,h,w.descriptor+(gaddr)(int32_t)(int16_t)w.value,0x00fc);SW(stride,FSP_STRIDE,w.control);SW(size,FSP_SIZE,w.secondary);SW(offset,FSP_OFFSET,w.source);P(registers,FSP_REGISTERS,0xc4c592);
  w=load_words(w,h,w.descriptor+(gaddr)(int32_t)(int16_t)w.base,0x001c);SW(control,FSP_CONTROL,w.stride);SW(secondary,FSP_SECONDARY,w.size);SW(source,FSP_SOURCE,w.offset);store_words(h,w.registers,0x001c);
  W(base,FSP_BASE,rd_u16(w.stream));P(stream,FSP_STREAM,w.stream+2);ANDW(base,FSP_BASE,0x7fff);w=load_words(w,h,w.descriptor+(gaddr)(int32_t)(int16_t)w.base,0x001c);SW(control,FSP_CONTROL,w.stride);SW(secondary,FSP_SECONDARY,w.size);SW(source,FSP_SOURCE,w.offset);store_words(h,w.registers+6,0x001c);
  w=push_long(w,h,w.stream);observe(h,FSP_PUSH_WORD,FSP_VALUE,w.value,0);w=restored(h);w=consume(h,FSP_OFFSET_SEGMENTS);result_word(w,h);observe(h,FSP_POP_WORD,FSP_VALUE,0,0);w=restored(h);w=pop_pointer(w,h,FSP_STREAM);
 }
 w=restore_longs(w,h,0x2200);W(value,FSP_VALUE,rd_u16(w.frame-0x7e));
}

static void style_words(FaceStreamState *state,const FaceStreamHooks *h,uint16_t planes,uint16_t colour,uint16_t complement) {
 FaceStreamState w=*state;W(value,FSP_VALUE,planes);W(base,FSP_BASE,colour);W(control,FSP_CONTROL,complement);W(secondary,FSP_SECONDARY,0);store_words(h,0xc456e6,15);*state=w;
}
static void parallelogram_body(FaceStreamState w,const FaceStreamHooks *h) {
 word(h,0xc45954,rd_u16(w.stream));P(stream,FSP_STREAM,w.stream+2);P(descriptor,FSP_DESCRIPTOR,0xc48390);P(descriptor,FSP_DESCRIPTOR,w.descriptor+(gaddr)(int32_t)rd_s16(w.stream));P(stream,FSP_STREAM,w.stream+2);w=save_longs(w,h,0x0064);P(registers,FSP_REGISTERS,0xc4bf90);longword(h,w.registers,4);P(registers,FSP_REGISTERS,w.registers+4);
 w=load_words(w,h,w.descriptor,0x00fc);P(descriptor,FSP_DESCRIPTOR,w.descriptor+12);store_words(h,w.registers,0x00fc);W(value,FSP_VALUE,w.source);ANDW(value,FSP_VALUE,w.offset);P(registers,FSP_REGISTERS,w.registers+12);SW(stride,FSP_STRIDE,w.control);SW(size,FSP_SIZE,w.secondary);SW(offset,FSP_OFFSET,w.source);
 w=load_words(w,h,w.descriptor,0x001c);ANDW(value,FSP_VALUE,w.source);store_words(h,w.registers,0x001c);P(registers,FSP_REGISTERS,w.registers+6);SW(control,FSP_CONTROL,w.stride);SW(secondary,FSP_SECONDARY,w.size);SW(source,FSP_SOURCE,w.offset);ANDW(value,FSP_VALUE,w.source);
 if((int16_t)w.value<0){w=restore_longs(w,h,0x2600);L(value,FSP_VALUE,0);return;}
 store_words(h,w.registers,0x001c);w=consume(h,FSP_PARALLELOGRAM_FACE);w=restore_longs(w,h,0x2600);
}
/* C20E4E/C20E40: distinct style prefixes and the complete shared tail. */
void face_stream_parallelogram_face(FaceStreamState w,const FaceStreamHooks *h) {style_words(&w,h,8,8,0);parallelogram_body(w,h);}
void face_stream_parallelogram_face_two(FaceStreamState w,const FaceStreamHooks *h) {style_words(&w,h,2,0,2);parallelogram_body(w,h);}

/* C21490: source near test, all four corners and rejection flag words. */
void face_stream_near_parallelogram_face(FaceStreamState w,const FaceStreamHooks *h) {
 word(h,0xc45954,rd_u16(w.stream));P(stream,FSP_STREAM,w.stream+2);P(descriptor,FSP_DESCRIPTOR,0xc48390);P(descriptor,FSP_DESCRIPTOR,w.descriptor+(gaddr)(int32_t)rd_s16(w.stream));P(stream,FSP_STREAM,w.stream+2);CL((uint32_t)-128,rd_u32(0xc45a78));if(rd_s32(0xc45a78)<-128)goto rejected;
 P(registers,FSP_REGISTERS,0xc4bf90);word(h,w.registers,0);P(registers,FSP_REGISTERS,w.registers+2);word(h,w.registers,4);P(registers,FSP_REGISTERS,w.registers+2);w=load_words(w,h,w.descriptor,0x00fc);P(descriptor,FSP_DESCRIPTOR,w.descriptor+12);store_words(h,w.registers,0x00fc);P(registers,FSP_REGISTERS,w.registers+12);SW(stride,FSP_STRIDE,w.control);SW(size,FSP_SIZE,w.secondary);SW(offset,FSP_OFFSET,w.source);w=load_words(w,h,w.descriptor,0x001c);
 word(h,w.registers,(uint16_t)w.control);P(registers,FSP_REGISTERS,w.registers+2);word(h,w.registers,(uint16_t)w.secondary);P(registers,FSP_REGISTERS,w.registers+2);word(h,w.registers,(uint16_t)w.source);P(registers,FSP_REGISTERS,w.registers+2);
 SW(control,FSP_CONTROL,w.stride);SW(secondary,FSP_SECONDARY,w.size);SW(source,FSP_SOURCE,w.offset);store_words(h,w.registers,0x001c);W(offset,FSP_OFFSET,w.source);ANDW(offset,FSP_OFFSET,rd_u16(w.registers+10));ANDW(offset,FSP_OFFSET,rd_u16(w.registers+16));ANDW(offset,FSP_OFFSET,rd_u16(w.registers+22));if((int16_t)w.offset<0)goto rejected;
 w=save_longs(w,h,0x0064);w=consume(h,FSP_NEAR_PARALLELOGRAM_FACE);w=restore_longs(w,h,0x2600);return;
rejected:L(value,FSP_VALUE,0);
}

/* C2139E/C21412: original offset and mixed four-point faces. */
void face_stream_offset_face(FaceStreamState w,const FaceStreamHooks *h) {
 word(h,0xc45954,rd_u16(w.stream));P(stream,FSP_STREAM,w.stream+2);w=load_words(w,h,w.stream,0x0087);P(stream,FSP_STREAM,w.stream+8);P(descriptor,FSP_DESCRIPTOR,0xc48390);P(registers,FSP_REGISTERS,0xc4bf90);word(h,w.registers,0);P(registers,FSP_REGISTERS,w.registers+2);word(h,w.registers,4);P(registers,FSP_REGISTERS,w.registers+2);
 copy_vertex(&w,h,w.descriptor+(gaddr)(int32_t)(int16_t)w.base,1);W(size,FSP_SIZE,rd_u16(w.registers-2));copy_vertex(&w,h,w.descriptor+(gaddr)(int32_t)(int16_t)w.control,1);ANDW(size,FSP_SIZE,rd_u16(w.registers-2));copy_vertex(&w,h,w.descriptor+(gaddr)(int32_t)(int16_t)w.offset,1);ANDW(size,FSP_SIZE,rd_u16(w.registers-2));
 w=load_words(w,h,w.descriptor+(gaddr)(int32_t)(int16_t)w.value,0x003f);SW(secondary,FSP_SECONDARY,w.value);SW(source,FSP_SOURCE,w.base);SW(stride,FSP_STRIDE,w.control);w=load_words(w,h,w.descriptor+(gaddr)(int32_t)(int16_t)w.offset,7);SW(value,FSP_VALUE,w.secondary);SW(base,FSP_BASE,w.source);SW(control,FSP_CONTROL,w.stride);store_words(h,w.registers,7);ANDW(size,FSP_SIZE,w.control);if((int16_t)w.size<0){L(value,FSP_VALUE,0);return;}
 w=save_longs(w,h,0x0064);w=consume(h,FSP_OFFSET_FACE);w=restore_longs(w,h,0x2600);
}
void face_stream_mixed_face(FaceStreamState w,const FaceStreamHooks *h) {
 word(h,0xc45954,rd_u16(w.stream));P(stream,FSP_STREAM,w.stream+2);W(offset,FSP_OFFSET,rd_u16(w.stream));P(stream,FSP_STREAM,w.stream+2);P(descriptor,FSP_DESCRIPTOR,0xc48390);P(registers,FSP_REGISTERS,0xc4bf90);word(h,w.registers,0);P(registers,FSP_REGISTERS,w.registers+2);word(h,w.registers,4);P(registers,FSP_REGISTERS,w.registers+2);copy_vertex(&w,h,w.descriptor+(gaddr)(int32_t)(int16_t)w.offset,1);W(size,FSP_SIZE,rd_u16(w.registers-2));
 W(value,FSP_VALUE,rd_u16(w.stream));P(stream,FSP_STREAM,w.stream+2);w=load_words(w,h,w.descriptor+(gaddr)(int32_t)(int16_t)w.value,0x003f);SW(secondary,FSP_SECONDARY,w.value);SW(source,FSP_SOURCE,w.base);SW(stride,FSP_STRIDE,w.control);w=load_words(w,h,w.descriptor+(gaddr)(int32_t)(int16_t)w.offset,7);SW(value,FSP_VALUE,w.secondary);SW(base,FSP_BASE,w.source);SW(control,FSP_CONTROL,w.stride);
 word(h,w.registers,(uint16_t)w.value);P(registers,FSP_REGISTERS,w.registers+2);word(h,w.registers,(uint16_t)w.base);P(registers,FSP_REGISTERS,w.registers+2);word(h,w.registers,(uint16_t)w.control);P(registers,FSP_REGISTERS,w.registers+2);ANDW(size,FSP_SIZE,w.control);
 W(offset,FSP_OFFSET,rd_u16(w.stream));P(stream,FSP_STREAM,w.stream+2);w=load_words(w,h,w.descriptor+(gaddr)(int32_t)(int16_t)w.offset,7);SW(value,FSP_VALUE,w.secondary);SW(base,FSP_BASE,w.source);SW(control,FSP_CONTROL,w.stride);
 word(h,w.registers,(uint16_t)w.value);P(registers,FSP_REGISTERS,w.registers+2);word(h,w.registers,(uint16_t)w.base);P(registers,FSP_REGISTERS,w.registers+2);word(h,w.registers,(uint16_t)w.control);P(registers,FSP_REGISTERS,w.registers+2);ANDW(size,FSP_SIZE,w.control);copy_vertex(&w,h,w.descriptor+(gaddr)(int32_t)(int16_t)w.offset,0);ANDW(size,FSP_SIZE,rd_u16(w.registers));if((int16_t)w.size<0){L(value,FSP_VALUE,0);return;}
 w=save_longs(w,h,0x0064);w=consume(h,FSP_MIXED_FACE);w=restore_longs(w,h,0x2600);
}

/* C20F18: the shared derived-point tail, preserving MOVEM and SWAP. */
static void extend_parallelogram_body(FaceStreamState w,const FaceStreamHooks *h) {
 w=load_words(w,h,w.descriptor,0x10ff);P(descriptor,FSP_DESCRIPTOR,w.descriptor+18);SW(value,FSP_VALUE,w.secondary);SW(base,FSP_BASE,w.source);SW(control,FSP_CONTROL,w.stride);AW(size,FSP_SIZE,w.value);AW(offset,FSP_OFFSET,w.base);P(screen,FSP_SCREEN,w.screen+(gaddr)(int32_t)(int16_t)w.control);store_words(h,w.descriptor+6,0x10c0);
 w=load_words(w,h,w.descriptor,0x10c0);P(descriptor,FSP_DESCRIPTOR,w.descriptor+6);AW(size,FSP_SIZE,w.value);AW(offset,FSP_OFFSET,w.base);P(screen,FSP_SCREEN,w.screen+(gaddr)(int32_t)(int16_t)w.control);store_words(h,w.descriptor+6,0x10c0);
 SWAP(value,FSP_VALUE);SWAP(base,FSP_BASE);SWAP(control,FSP_CONTROL);W(value,FSP_VALUE,w.secondary);W(base,FSP_BASE,w.source);W(control,FSP_CONTROL,w.stride);w=load_words(w,h,w.descriptor-12,0x10f8);SW(secondary,FSP_SECONDARY,w.value);SW(source,FSP_SOURCE,w.base);SW(stride,FSP_STRIDE,w.control);AW(size,FSP_SIZE,w.secondary);AW(offset,FSP_OFFSET,w.source);P(screen,FSP_SCREEN,w.screen+(gaddr)(int32_t)(int16_t)w.stride);store_words(h,w.descriptor+12,0x10c0);
 SWAP(value,FSP_VALUE);SWAP(base,FSP_BASE);SWAP(control,FSP_CONTROL);AW(size,FSP_SIZE,w.value);AW(offset,FSP_OFFSET,w.base);P(screen,FSP_SCREEN,w.screen+(gaddr)(int32_t)(int16_t)w.control);store_words(h,w.descriptor+18,0x10c0);L(value,FSP_VALUE,0);
}
void face_stream_extend_parallelograms(FaceStreamState w,const FaceStreamHooks *h) {
 P(descriptor,FSP_DESCRIPTOR,0xc48390);P(descriptor,FSP_DESCRIPTOR,w.descriptor+(gaddr)(int32_t)rd_s16(w.stream));P(stream,FSP_STREAM,w.stream+2);extend_parallelogram_body(w,h);
}
void face_stream_extend_parallelograms_scaled(FaceStreamState w,const FaceStreamHooks *h) {
 P(descriptor,FSP_DESCRIPTOR,0xc48390);P(descriptor,FSP_DESCRIPTOR,w.descriptor+(gaddr)(int32_t)rd_s16(w.stream));P(stream,FSP_STREAM,w.stream+2);w=load_words(w,h,w.descriptor+6,0x10ff);SW(value,FSP_VALUE,w.secondary);SW(base,FSP_BASE,w.source);SW(control,FSP_CONTROL,w.stride);SW(secondary,FSP_SECONDARY,w.size);SW(source,FSP_SOURCE,w.offset);SW(stride,FSP_STRIDE,w.screen);SWAP(value,FSP_VALUE);W(value,FSP_VALUE,rd_u16(w.stream));P(stream,FSP_STREAM,w.stream+2);
 if((int16_t)w.value<0){NEG(value,FSP_VALUE);ASR(secondary,FSP_SECONDARY,w.value);ASR(source,FSP_SOURCE,w.value);ASR(stride,FSP_STRIDE,w.value);}else {ASL(secondary,FSP_SECONDARY,w.value);ASL(source,FSP_SOURCE,w.value);ASL(stride,FSP_STRIDE,w.value);}
 SWAP(value,FSP_VALUE);SW(size,FSP_SIZE,w.secondary);SW(offset,FSP_OFFSET,w.source);P(screen,FSP_SCREEN,w.screen-(gaddr)(int32_t)(int16_t)w.stride);store_words(h,w.descriptor+0x30,0x10c0);SW(size,FSP_SIZE,w.value);SW(offset,FSP_OFFSET,w.base);P(screen,FSP_SCREEN,w.screen-(gaddr)(int32_t)(int16_t)w.control);store_words(h,w.descriptor+0x36,0x10c0);extend_parallelogram_body(w,h);
}

/* C21A20's three source copies share identical point updates. */
static FaceStreamState block_copy(FaceStreamState w,const FaceStreamHooks *h,gaddr destination) {
 w=load_words(w,h,w.table+6,0x10f8);AW(secondary,FSP_SECONDARY,w.value);AW(source,FSP_SOURCE,w.base);AW(stride,FSP_STRIDE,w.control);AW(size,FSP_SIZE,w.value);AW(offset,FSP_OFFSET,w.base);P(screen,FSP_SCREEN,w.screen+(gaddr)(int32_t)(int16_t)w.control);store_words(h,w.table+destination,0x10f8);
 w=load_words(w,h,w.table+18,0x10f8);AW(secondary,FSP_SECONDARY,w.value);AW(source,FSP_SOURCE,w.base);AW(stride,FSP_STRIDE,w.control);AW(size,FSP_SIZE,w.value);AW(offset,FSP_OFFSET,w.base);P(screen,FSP_SCREEN,w.screen+(gaddr)(int32_t)(int16_t)w.control);store_words(h,w.table+destination+12,0x10f8);
 w=load_words(w,h,w.table+30,0x0038);AW(secondary,FSP_SECONDARY,w.value);AW(source,FSP_SOURCE,w.base);AW(stride,FSP_STRIDE,w.control);store_words(h,w.table+destination+24,0x0038);return w;
}
void face_stream_offset_block_copies(FaceStreamState w,const FaceStreamHooks *h) {
 P(descriptor,FSP_DESCRIPTOR,0xc48390);w=push_long(w,h,w.table);P(table,FSP_TABLE,w.descriptor);P(table,FSP_TABLE,w.table+(gaddr)(int32_t)rd_s16(w.stream));P(stream,FSP_STREAM,w.stream+2);W(offset,FSP_OFFSET,rd_u16(w.stream));w=load_words(w,h,w.descriptor+(gaddr)(int32_t)(int16_t)w.offset,7);w=load_words(w,h,w.table,0x0038);SW(value,FSP_VALUE,w.secondary);SW(base,FSP_BASE,w.source);SW(control,FSP_CONTROL,w.stride);w=block_copy(w,h,0xb4);
 W(offset,FSP_OFFSET,rd_u16(w.stream));AW(offset,FSP_OFFSET,6);w=load_words(w,h,w.table,0x0038);w=load_words(w,h,w.descriptor+(gaddr)(int32_t)(int16_t)w.offset,7);SW(value,FSP_VALUE,w.secondary);SW(base,FSP_BASE,w.source);SW(control,FSP_CONTROL,w.stride);w=block_copy(w,h,0xd2);
 W(offset,FSP_OFFSET,rd_u16(w.stream));P(stream,FSP_STREAM,w.stream+2);AW(offset,FSP_OFFSET,12);w=load_words(w,h,w.table,0x0038);w=load_words(w,h,w.descriptor+(gaddr)(int32_t)(int16_t)w.offset,7);SW(value,FSP_VALUE,w.secondary);SW(base,FSP_BASE,w.source);SW(control,FSP_CONTROL,w.stride);w=block_copy(w,h,0xf0);
 W(value,FSP_VALUE,rd_u16(w.stream));P(stream,FSP_STREAM,w.stream+2);P(stream,FSP_STREAM,w.stream+(gaddr)(int32_t)(int16_t)w.value);w=pop_pointer(w,h,FSP_TABLE);L(value,FSP_VALUE,0);
}

/* C217EA: scaled original block extension; intermediate writes are read
 * again at the original source offsets before the final copies. */
void face_stream_extend_block_scaled(FaceStreamState w,const FaceStreamHooks *h) {
 P(descriptor,FSP_DESCRIPTOR,0xc48390);P(descriptor,FSP_DESCRIPTOR,w.descriptor+(gaddr)(int32_t)rd_s16(w.stream));P(stream,FSP_STREAM,w.stream+2);w=load_words(w,h,w.descriptor+6,0x10ff);SW(secondary,FSP_SECONDARY,w.size);SW(source,FSP_SOURCE,w.offset);SW(stride,FSP_STRIDE,w.screen);SWAP(value,FSP_VALUE);W(value,FSP_VALUE,rd_u16(w.stream));P(stream,FSP_STREAM,w.stream+2);
 if((int16_t)w.value<0){NEG(value,FSP_VALUE);ASR(secondary,FSP_SECONDARY,w.value);ASR(source,FSP_SOURCE,w.value);ASR(stride,FSP_STRIDE,w.value);}else {ASL(secondary,FSP_SECONDARY,w.value);ASL(source,FSP_SOURCE,w.value);ASL(stride,FSP_STRIDE,w.value);}
 SWAP(value,FSP_VALUE);SW(size,FSP_SIZE,w.secondary);SW(offset,FSP_OFFSET,w.source);P(screen,FSP_SCREEN,w.screen-(gaddr)(int32_t)(int16_t)w.stride);store_words(h,w.descriptor+0x3c,0x10c0);AW(size,FSP_SIZE,w.secondary);AW(offset,FSP_OFFSET,w.source);P(screen,FSP_SCREEN,w.screen+(gaddr)(int32_t)(int16_t)w.stride);
 w=load_words(w,h,w.descriptor+12,0x0038);SW(secondary,FSP_SECONDARY,w.value);SW(source,FSP_SOURCE,w.base);SW(stride,FSP_STRIDE,w.control);AW(size,FSP_SIZE,w.secondary);AW(offset,FSP_OFFSET,w.source);P(screen,FSP_SCREEN,w.screen+(gaddr)(int32_t)(int16_t)w.stride);store_words(h,w.descriptor+0x36,0x10c0);
 w=load_words(w,h,w.descriptor+0x1e,0x10ff);SW(value,FSP_VALUE,w.secondary);SW(base,FSP_BASE,w.source);SW(control,FSP_CONTROL,w.stride);AW(size,FSP_SIZE,w.value);AW(offset,FSP_OFFSET,w.base);P(screen,FSP_SCREEN,w.screen+(gaddr)(int32_t)(int16_t)w.control);store_words(h,w.descriptor+0x42,0x10c0);
 w=load_words(w,h,w.descriptor+0x30,7);SW(value,FSP_VALUE,w.secondary);SW(base,FSP_BASE,w.source);SW(control,FSP_CONTROL,w.stride);AW(size,FSP_SIZE,w.value);AW(offset,FSP_OFFSET,w.base);P(screen,FSP_SCREEN,w.screen+(gaddr)(int32_t)(int16_t)w.control);store_words(h,w.descriptor+0x48,0x10c0);
 w=load_words(w,h,w.descriptor+0x18,7);SW(value,FSP_VALUE,w.secondary);SW(base,FSP_BASE,w.source);SW(control,FSP_CONTROL,w.stride);w=load_words(w,h,w.descriptor+0x1e,0x0038);AW(secondary,FSP_SECONDARY,w.value);AW(source,FSP_SOURCE,w.base);AW(stride,FSP_STRIDE,w.control);w=load_words(w,h,w.descriptor+0x30,0x10c0);AW(size,FSP_SIZE,w.value);AW(offset,FSP_OFFSET,w.base);P(screen,FSP_SCREEN,w.screen+(gaddr)(int32_t)(int16_t)w.control);store_words(h,w.descriptor+0x4e,0x10f8);
 w=load_words(w,h,w.descriptor+0x2a,0x0038);AW(secondary,FSP_SECONDARY,w.value);AW(source,FSP_SOURCE,w.base);AW(stride,FSP_STRIDE,w.control);store_words(h,w.descriptor+0x5a,0x0038);
 w=load_words(w,h,w.descriptor+0x42,0x10f8);AW(secondary,FSP_SECONDARY,w.value);AW(source,FSP_SOURCE,w.base);AW(stride,FSP_STRIDE,w.control);AW(size,FSP_SIZE,w.value);AW(offset,FSP_OFFSET,w.base);P(screen,FSP_SCREEN,w.screen+(gaddr)(int32_t)(int16_t)w.control);store_words(h,w.descriptor+0x60,0x10f8);L(value,FSP_VALUE,0);
}

/* C20D68: every row and segment; frame vectors and source counts survive
 * the actual returned child state through original MOVEM transfers. */
void face_stream_segment_grid(FaceStreamState w,const FaceStreamHooks *h) {
 w=save_longs(w,h,0x0044);style_words(&w,h,15,0xffff,0);word(h,0xc45954,rd_u16(w.stream));P(stream,FSP_STREAM,w.stream+2);P(descriptor,FSP_DESCRIPTOR,0xc48390);P(descriptor,FSP_DESCRIPTOR,w.descriptor+(gaddr)(int32_t)rd_s16(w.stream));P(stream,FSP_STREAM,w.stream+2);word(h,w.frame-0x38,rd_u16(w.stream));P(stream,FSP_STREAM,w.stream+2);w=load_words(w,h,w.descriptor,0x007e);P(descriptor,FSP_DESCRIPTOR,w.descriptor+12);
 SW(base,FSP_BASE,rd_u16(w.descriptor));SW(control,FSP_CONTROL,rd_u16(w.descriptor+2));SW(secondary,FSP_SECONDARY,rd_u16(w.descriptor+4));store_words(h,w.frame-0x40,0x000e);SW(source,FSP_SOURCE,rd_u16(w.descriptor));SW(stride,FSP_STRIDE,rd_u16(w.descriptor+2));SW(size,FSP_SIZE,rd_u16(w.descriptor+4));store_words(h,w.frame-0x52,0x0070);word(h,w.frame-0x7e,0);
 for(;;) {
  word(h,w.frame-0x3a,rd_u16(w.stream));P(stream,FSP_STREAM,w.stream+2);W(source,FSP_SOURCE,0);W(stride,FSP_STRIDE,0);W(size,FSP_SIZE,0);
  for(;;) {
   store_words(h,w.frame-0x58,0x0070);w=load_words(w,h,w.descriptor,0x000e);ASR(source,FSP_SOURCE,1);ASR(stride,FSP_STRIDE,1);ASR(size,FSP_SIZE,1);AW(base,FSP_BASE,w.source);AW(control,FSP_CONTROL,w.stride);AW(secondary,FSP_SECONDARY,w.size);store_words(h,0xc4c592,0x000e);
   w=load_words(w,h,w.descriptor,0x000e);AW(base,FSP_BASE,rd_u16(w.frame-0x52));AW(control,FSP_CONTROL,rd_u16(w.frame-0x50));AW(secondary,FSP_SECONDARY,rd_u16(w.frame-0x4e));AW(base,FSP_BASE,w.source);AW(control,FSP_CONTROL,w.stride);AW(secondary,FSP_SECONDARY,w.size);store_words(h,0xc4c598,0x000e);
   w=save_longs(w,h,0x0030);w=consume(h,FSP_SEGMENT_GRID);result_word(w,h);w=restore_longs(w,h,0x0c00);if(decrement_word(h,w.frame-0x3a)<=0)break;
   w=load_words(w,h,w.frame-0x58,0x0070);AW(source,FSP_SOURCE,rd_u16(w.frame-0x40));AW(stride,FSP_STRIDE,rd_u16(w.frame-0x3e));AW(size,FSP_SIZE,rd_u16(w.frame-0x3c));
  }
  if(decrement_word(h,w.frame-0x38)<=0)break;P(descriptor,FSP_DESCRIPTOR,w.descriptor+6);
 }
 w=restore_longs(w,h,0x2200);W(value,FSP_VALUE,rd_u16(w.frame-0x7e));
}

/* C20904: the two source lattice passes differ in point construction,
 * saved A3 and actual returned pointers; preserve those differences. */
void face_stream_segment_lattice(FaceStreamState w,const FaceStreamHooks *h) {
 unsigned pass;
 style_words(&w,h,15,0xffff,0);word(h,0xc45954,rd_u16(w.stream));P(stream,FSP_STREAM,w.stream+2);word(h,w.frame-0x7e,0);P(descriptor,FSP_DESCRIPTOR,0xc48390);P(descriptor,FSP_DESCRIPTOR,w.descriptor+(gaddr)(int32_t)rd_s16(w.stream));P(stream,FSP_STREAM,w.stream+2);word(h,w.frame-0x38,rd_u16(w.stream));word(h,w.frame-0x3a,rd_u16(w.stream));P(stream,FSP_STREAM,w.stream+2);w=save_longs(w,h,0x0064);
 w=load_words(w,h,w.descriptor,0x007e);P(descriptor,FSP_DESCRIPTOR,w.descriptor+12);SW(base,FSP_BASE,rd_u16(w.descriptor));SW(control,FSP_CONTROL,rd_u16(w.descriptor+2));SW(secondary,FSP_SECONDARY,rd_u16(w.descriptor+4));SW(source,FSP_SOURCE,rd_u16(w.descriptor));SW(stride,FSP_STRIDE,rd_u16(w.descriptor+2));SW(size,FSP_SIZE,rd_u16(w.descriptor+4));store_words(h,w.frame-0x46,0x007e);
 w=load_words(w,h,w.frame-0x40,0x0070);ASR(source,FSP_SOURCE,1);ASR(stride,FSP_STRIDE,1);ASR(size,FSP_SIZE,1);w=load_words(w,h,w.descriptor,0x000e);SW(base,FSP_BASE,w.source);SW(control,FSP_CONTROL,w.stride);SW(secondary,FSP_SECONDARY,w.size);AW(base,FSP_BASE,rd_u16(w.frame-0x46));AW(control,FSP_CONTROL,rd_u16(w.frame-0x44));AW(secondary,FSP_SECONDARY,rd_u16(w.frame-0x42));store_words(h,w.frame-0x4c,0x000e);
 for(pass=0;pass<2;++pass) {
  L(source,FSP_SOURCE,0);L(stride,FSP_STRIDE,0);L(size,FSP_SIZE,0);
  for(;;) {
   store_words(h,w.frame-0x58,0x0070);ASR(source,FSP_SOURCE,1);ASR(stride,FSP_STRIDE,1);ASR(size,FSP_SIZE,1);
   if(!pass){w=load_words(w,h,w.descriptor,0x000e);AW(base,FSP_BASE,w.source);AW(control,FSP_CONTROL,w.stride);AW(secondary,FSP_SECONDARY,w.size);}else {w=load_words(w,h,w.frame-0x4c,0x000e);SW(base,FSP_BASE,w.source);SW(control,FSP_CONTROL,w.stride);SW(secondary,FSP_SECONDARY,w.size);}
   W(source,FSP_SOURCE,w.base);W(stride,FSP_STRIDE,w.control);W(size,FSP_SIZE,w.secondary);
   if(!pass){AW(source,FSP_SOURCE,rd_u16(w.frame-0x46));AW(stride,FSP_STRIDE,rd_u16(w.frame-0x44));AW(size,FSP_SIZE,rd_u16(w.frame-0x42));}else {SW(source,FSP_SOURCE,rd_u16(w.frame-0x46));SW(stride,FSP_STRIDE,rd_u16(w.frame-0x44));SW(size,FSP_SIZE,rd_u16(w.frame-0x42));}
   store_words(h,0xc4c592,0x007e);if(!pass)w=push_long(w,h,w.descriptor);w=consume(h,pass?FSP_LATTICE_SECOND:FSP_LATTICE_FIRST);result_word(w,h);if(!pass)w=pop_pointer(w,h,FSP_DESCRIPTOR);
   if(decrement_word(h,w.frame-(pass?0x3au:0x38u))<=0)break;w=load_words(w,h,w.frame-0x58,0x0070);AW(source,FSP_SOURCE,rd_u16(w.frame-0x40));AW(stride,FSP_STRIDE,rd_u16(w.frame-0x3e));AW(size,FSP_SIZE,rd_u16(w.frame-0x3c));
  }
 }
 w=restore_longs(w,h,0x2600);W(value,FSP_VALUE,rd_u16(w.frame-0x7e));
}

/* C2159E: complete side face and overflow-sensitive dot-product branch. */
void face_stream_side_face(FaceStreamState w,const FaceStreamHooks *h) {
 int64_t sum;int first_path;
 P(descriptor,FSP_DESCRIPTOR,0xc48390);P(registers,FSP_REGISTERS,0xc4bf92);word(h,w.registers,3);P(registers,FSP_REGISTERS,w.registers+2);word(h,0xc45954,rd_u16(w.stream));P(stream,FSP_STREAM,w.stream+2);P(screen,FSP_SCREEN,rd_u32(0xc45a32));W(value,FSP_VALUE,rd_u16(w.stream));P(stream,FSP_STREAM,w.stream+2);W(base,FSP_BASE,rd_u16(w.screen+10+(gaddr)(int32_t)(int16_t)w.value));W(control,FSP_CONTROL,rd_u16(w.screen+14+(gaddr)(int32_t)(int16_t)w.value));W(secondary,FSP_SECONDARY,w.base);W(source,FSP_SOURCE,w.control);
 W(value,FSP_VALUE,rd_u16(w.stream));P(stream,FSP_STREAM,w.stream+2);W(size,FSP_SIZE,w.value);observe(h,FSP_BIT_CLEAR,FSP_VALUE,w.value,15);w.value&=~0x8000u;SW(base,FSP_BASE,rd_u16(w.screen+10+(gaddr)(int32_t)(int16_t)w.value));SW(control,FSP_CONTROL,rd_u16(w.screen+14+(gaddr)(int32_t)(int16_t)w.value));B(offset,FSP_OFFSET,rd_u8(w.screen+6));ANDW(offset,FSP_OFFSET,15);ASR(secondary,FSP_SECONDARY,w.offset);ASR(source,FSP_SOURCE,w.offset);AW(secondary,FSP_SECONDARY,rd_u16(0xc45b2a));AW(source,FSP_SOURCE,rd_u16(0xc45b2e));W(offset,FSP_OFFSET,rd_u16(0xc45ab8));ASL(secondary,FSP_SECONDARY,w.offset);ASL(source,FSP_SOURCE,w.offset);AW(secondary,FSP_SECONDARY,rd_u16(0xc45a72));AW(source,FSP_SOURCE,rd_u16(0xc45a76));MUL(secondary,FSP_SECONDARY,w.base);MUL(source,FSP_SOURCE,w.control);
 W(value,FSP_VALUE,rd_u16(w.stream));P(stream,FSP_STREAM,w.stream+2);copy_vertex(&w,h,w.descriptor+(gaddr)(int32_t)(int16_t)w.value,1);P(descriptor,FSP_DESCRIPTOR,w.descriptor+(gaddr)(int32_t)rd_s16(w.stream));P(stream,FSP_STREAM,w.stream+2);w=save_longs(w,h,0x0064);sum=(int64_t)(int32_t)w.source+(int32_t)w.secondary;AL(source,FSP_SOURCE,w.secondary);TW(w.size);first_path=(sum<0)==((int16_t)w.size<0);
 if(first_path) {copy_vertex(&w,h,w.descriptor,1);copy_vertex(&w,h,w.descriptor+12,0);W(offset,FSP_OFFSET,rd_u16(w.registers-12));ANDW(offset,FSP_OFFSET,rd_u16(w.registers-6));ANDW(offset,FSP_OFFSET,rd_u16(w.registers));if((int16_t)w.offset<0)goto rejected;}
 else {w=load_words(w,h,w.descriptor,0x00fc);P(descriptor,FSP_DESCRIPTOR,w.descriptor+12);word(h,w.registers,(uint16_t)w.stride);P(registers,FSP_REGISTERS,w.registers+2);word(h,w.registers,(uint16_t)w.size);P(registers,FSP_REGISTERS,w.registers+2);word(h,w.registers,(uint16_t)w.offset);P(registers,FSP_REGISTERS,w.registers+2);SW(stride,FSP_STRIDE,w.control);SW(size,FSP_SIZE,w.secondary);SW(offset,FSP_OFFSET,w.source);w=load_words(w,h,w.descriptor,0x001c);SW(control,FSP_CONTROL,w.stride);SW(secondary,FSP_SECONDARY,w.size);SW(source,FSP_SOURCE,w.offset);store_words(h,w.registers,0x001c);ANDW(source,FSP_SOURCE,rd_u16(w.registers-8));ANDW(source,FSP_SOURCE,rd_u16(w.registers-2));if((int16_t)w.source<0)goto rejected;}
 w=consume(h,first_path?FSP_SIDE_FIRST:FSP_SIDE_SECOND);w=restore_longs(w,h,0x2600);return;
rejected:w=restore_longs(w,h,0x2600);L(value,FSP_VALUE,0);
}

/* C210E6: original strip constructor and every quad clip child. */
void face_stream_quad_strip(FaceStreamState w,const FaceStreamHooks *h) {
 style_words(&w,h,2,0,2);P(descriptor,FSP_DESCRIPTOR,0xc48390);P(descriptor,FSP_DESCRIPTOR,w.descriptor+(gaddr)(int32_t)rd_s16(w.stream));P(stream,FSP_STREAM,w.stream+2);word(h,w.frame-0x38,rd_u16(w.stream));P(stream,FSP_STREAM,w.stream+2);w=save_longs(w,h,0x0064);w=load_words(w,h,w.descriptor,0x007e);P(descriptor,FSP_DESCRIPTOR,w.descriptor+12);
 SW(base,FSP_BASE,rd_u16(w.descriptor+6));SW(control,FSP_CONTROL,rd_u16(w.descriptor+8));SW(secondary,FSP_SECONDARY,rd_u16(w.descriptor+10));SW(source,FSP_SOURCE,rd_u16(w.descriptor+6));SW(stride,FSP_STRIDE,rd_u16(w.descriptor+8));SW(size,FSP_SIZE,rd_u16(w.descriptor+10));W(offset,FSP_OFFSET,w.source);AW(offset,FSP_OFFSET,w.offset);AW(source,FSP_SOURCE,w.offset);W(offset,FSP_OFFSET,w.stride);AW(offset,FSP_OFFSET,w.offset);AW(stride,FSP_STRIDE,w.offset);W(offset,FSP_OFFSET,w.size);AW(offset,FSP_OFFSET,w.offset);AW(size,FSP_SIZE,w.offset);ASR(source,FSP_SOURCE,1);ASR(stride,FSP_STRIDE,1);ASR(size,FSP_SIZE,1);AW(source,FSP_SOURCE,w.base);AW(stride,FSP_STRIDE,w.control);AW(size,FSP_SIZE,w.secondary);store_words(h,w.frame-0x46,0x0070);
 w=load_words(w,h,w.descriptor,0x000e);P(descriptor,FSP_DESCRIPTOR,w.descriptor+6);SW(base,FSP_BASE,rd_u16(w.descriptor));SW(control,FSP_CONTROL,rd_u16(w.descriptor+2));SW(secondary,FSP_SECONDARY,rd_u16(w.descriptor+4));store_words(h,w.frame-0x52,0x000e);word(h,w.frame-0x7e,0);P(registers,FSP_REGISTERS,0xc4bf90);longword(h,w.registers,4);P(registers,FSP_REGISTERS,w.registers+4);
 for(;;) {
  P(registers,FSP_REGISTERS,0xc4bf94);w=load_words(w,h,w.descriptor,0x000e);W(source,FSP_SOURCE,w.base);W(stride,FSP_STRIDE,w.control);W(size,FSP_SIZE,w.secondary);AW(source,FSP_SOURCE,rd_u16(w.frame-0x46));AW(stride,FSP_STRIDE,rd_u16(w.frame-0x44));AW(size,FSP_SIZE,rd_u16(w.frame-0x42));store_words(h,w.registers,0x007e);P(registers,FSP_REGISTERS,w.registers+12);
  w=load_words(w,h,w.descriptor,0x0070);AW(source,FSP_SOURCE,rd_u16(w.frame-0x52));AW(stride,FSP_STRIDE,rd_u16(w.frame-0x50));AW(size,FSP_SIZE,rd_u16(w.frame-0x4e));W(base,FSP_BASE,w.source);W(control,FSP_CONTROL,w.stride);W(secondary,FSP_SECONDARY,w.size);AW(base,FSP_BASE,rd_u16(w.frame-0x46));AW(control,FSP_CONTROL,rd_u16(w.frame-0x44));AW(secondary,FSP_SECONDARY,rd_u16(w.frame-0x42));store_words(h,w.registers,0x007e);w=push_long(w,h,w.descriptor);w=consume(h,FSP_QUAD_STRIP);result_word(w,h);w=pop_pointer(w,h,FSP_DESCRIPTOR);if(decrement_word(h,w.frame-0x38)<=0)break;P(descriptor,FSP_DESCRIPTOR,w.descriptor+6);
 }
 w=restore_longs(w,h,0x2600);W(value,FSP_VALUE,rd_u16(w.frame-0x7e));
}
