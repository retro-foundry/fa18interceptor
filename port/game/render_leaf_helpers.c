#include "render_leaf_helpers.h"
#include <stdlib.h>
static void observe(const RenderLeafHooks *h,enum RenderLeafPhase p,enum RenderLeafField f,uint32_t v,uint32_t o) {
    if(h && h->observe) h->observe(h->context,p,f,v,o);
}
static RenderLeafState consume(const RenderLeafHooks *h,enum RenderLeafChild child) {
    if(h && h->consume) return h->consume(h->context,child); abort();
}
static RenderLeafState restored(const RenderLeafHooks *h) {
    if(h && h->restored) return h->restored(h->context); abort();
}
static uint32_t low_word(uint32_t old,uint16_t v) { return (old&0xffff0000u)|v; }
static uint32_t low_byte(uint32_t old,uint8_t v) { return (old&0xffffff00u)|v; }
#define W(f,id,v) do { w.f=low_word(w.f,(uint16_t)(v)); observe(h,RL_WORD,id,w.f,0); } while(0)
#define B(f,id,v) do { w.f=low_byte(w.f,(uint8_t)(v)); observe(h,RL_BYTE,id,w.f,0); } while(0)
#define L(f,id,v) do { w.f=(uint32_t)(v); observe(h,RL_LONG,id,w.f,0); } while(0)
#define P(f,id,v) do { w.f=(gaddr)(v); observe(h,RL_POINTER,id,w.f,0); } while(0)
#define AW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f+n_)); observe(h,RL_ADD_WORD,id,n_,0); } while(0)
#define SW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f-n_)); observe(h,RL_SUB_WORD,id,n_,0); } while(0)
#define AL(f,id,v) do { uint32_t n_=(uint32_t)(v); w.f+=n_; observe(h,RL_ADD_LONG,id,n_,0); } while(0)
#define SL(f,id,v) do { uint32_t n_=(uint32_t)(v); w.f-=n_; observe(h,RL_SUB_LONG,id,n_,0); } while(0)
#define ANDW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f&n_)); observe(h,RL_AND_WORD,id,n_,0); } while(0)
#define ANDB(f,id,v) do { uint8_t n_=(uint8_t)(v); w.f=low_byte(w.f,(uint8_t)(w.f&n_)); observe(h,RL_AND_BYTE,id,n_,0); } while(0)
#define ORW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f|n_)); observe(h,RL_OR_WORD,id,n_,0); } while(0)
#define EL(f,id) do { w.f=(uint32_t)(int32_t)(int16_t)w.f; observe(h,RL_EXT_LONG,id,0,0); } while(0)
#define NEG(f,id) do { w.f=low_word(w.f,(uint16_t)(0u-w.f)); observe(h,RL_NEG_WORD,id,0,0); } while(0)
#define SWAP(f,id) do { w.f=(w.f<<16)|(w.f>>16); observe(h,RL_SWAP,id,0,0); } while(0)
#define ASL(f,id,n) do { w.f=low_word(w.f,(uint16_t)((((unsigned)(n)&63u)<16?w.f<<((unsigned)(n)&63u):0))); observe(h,RL_ASL_WORD,id,n,0); } while(0)
#define LSL(f,id,n) do { w.f=low_word(w.f,(uint16_t)((((unsigned)(n)&63u)<16?w.f<<((unsigned)(n)&63u):0))); observe(h,RL_LSL_WORD,id,n,0); } while(0)
#define ASR(f,id,n) do { unsigned n_=(unsigned)(n)&63u; w.f=low_word(w.f,(uint16_t)((int16_t)w.f>>(n_<16?n_:15))); observe(h,RL_ASR_WORD,id,n_,0); } while(0)
#define CW(v,n) observe(h,RL_COMPARE_WORD,RL_VALUE,(uint16_t)(n),(uint16_t)(v))
#define CB(v,n) observe(h,RL_COMPARE_BYTE,RL_VALUE,(uint8_t)(n),(uint8_t)(v))
#define TW(v) observe(h,RL_TEST_WORD,RL_VALUE,(uint16_t)(v),0)
#define TB(v) observe(h,RL_TEST_BYTE,RL_VALUE,(uint8_t)(v),0)
static void word(const RenderLeafHooks *h,gaddr a,uint16_t v) { wr_u16(a,v); observe(h,RL_STORE_WORD,RL_VALUE,v,0); }
static void byte(const RenderLeafHooks *h,gaddr a,uint8_t v) { wr_u8(a,v); observe(h,RL_STORE_BYTE,RL_VALUE,v,0); }
static void longword(const RenderLeafHooks *h,gaddr a,uint32_t v) { wr_u32(a,v); observe(h,RL_STORE_LONG,RL_VALUE,v,0); }
static void decrement_byte(const RenderLeafHooks *h,gaddr a) { uint8_t old=rd_u8(a); wr_u8(a,(uint8_t)(old-1)); observe(h,RL_MEMORY_SUB_BYTE,RL_VALUE,old,1); }
static int bit(const RenderLeafHooks *h,gaddr a,unsigned b) { uint8_t v=rd_u8(a); observe(h,RL_BIT_TEST,RL_VALUE,v,b); return (v>>b)&1; }

/* MOVEM transfers retain CCR; individual MOVE transfers above update it. */
#define RAW(f,id,v) do { w.f=(uint32_t)(v); observe(h,RL_RAW_LONG,id,w.f,0); } while(0)
#define MUL(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=(uint32_t)((int32_t)(int16_t)w.f*(int16_t)n_); observe(h,RL_MULS,id,n_,0); } while(0)
#define MUP(f,id,ptr,pi) do { MUL(f,id,rd_u16(w.ptr)); P(ptr,pi,w.ptr+2); } while(0)
#define ARL(f,id,n) do { unsigned n_=(unsigned)(n)&63u; w.f=(uint32_t)((int32_t)w.f>>(n_<32?n_:31)); observe(h,RL_ASR_LONG,id,n_,0); } while(0)
#define DU(f,id,n) do { uint16_t n_=(uint16_t)(n); if(n_ && w.f/n_<=65535u) w.f=(w.f%n_)<<16|(w.f/n_); observe(h,RL_DIVU,id,n_,0); } while(0)

#define AB(f,id,v) do { uint8_t n_=(uint8_t)(v); w.f=low_byte(w.f,(uint8_t)(w.f+n_)); observe(h,RL_ADD_BYTE,id,n_,0); } while(0)
#define SB(f,id,v) do { uint8_t n_=(uint8_t)(v); w.f=low_byte(w.f,(uint8_t)(w.f-n_)); observe(h,RL_SUB_BYTE,id,n_,0); } while(0)
#define EW(f,id) do { w.f=low_word(w.f,(uint16_t)(int16_t)(int8_t)w.f); observe(h,RL_EXT_WORD,id,0,0); } while(0)
#define NL(f,id) do { w.f=0u-w.f; observe(h,RL_NEG_LONG,id,0,0); } while(0)
#define CL(v,n) observe(h,RL_COMPARE_LONG,RL_VALUE,(uint32_t)(n),(uint32_t)(v))
#define TL(v) observe(h,RL_TEST_LONG,RL_VALUE,(uint32_t)(v),0)
static int memory_decrement(const RenderLeafHooks *h,gaddr at) {
    uint8_t v=rd_u8(at); wr_u8(at,(uint8_t)(v-1)); observe(h,RL_MEMORY_SUB_BYTE,RL_VALUE,v,1); return (int8_t)v-1>=0;
}
static void memory_increment(const RenderLeafHooks *h,gaddr at) {
    uint8_t v=rd_u8(at);wr_u8(at,(uint8_t)(v+1));observe(h,RL_MEMORY_ADD_BYTE,RL_VALUE,v,1);
}
static RenderLeafState load_words(RenderLeafState w,const RenderLeafHooks *h,gaddr at,uint16_t mask) {
    (void)w;observe(h,RL_LOAD_WORDS_AT,RL_VALUE,at,mask);return restored(h);
}
static RenderLeafState load_longs(RenderLeafState w,const RenderLeafHooks *h,gaddr at,uint16_t mask) {
    (void)w;observe(h,RL_LOAD_LONGS_AT,RL_VALUE,at,mask);return restored(h);
}
static void store_words(const RenderLeafHooks *h,gaddr at,uint16_t mask) { observe(h,RL_STORE_WORDS_AT,RL_VALUE,at,mask); }
static void store_longs(const RenderLeafHooks *h,gaddr at,uint16_t mask) { observe(h,RL_STORE_LONGS_AT,RL_VALUE,at,mask); }
static int decrement_word(const RenderLeafHooks *h,gaddr at) {
 uint16_t old=rd_u16(at);wr_u16(at,(uint16_t)(old-1));observe(h,RL_MEMORY_SUB_WORD,RL_VALUE,old,1);return (int16_t)old-1;
}
#define ALL(f,id,n) do { unsigned n_=(unsigned)(n)&63u; w.f=n_<32?w.f<<n_:0;observe(h,RL_ASL_LONG,id,n_,0); } while(0)
static RenderLeafState save_longs(RenderLeafState w,const RenderLeafHooks *h,uint16_t mask) { observe(h,RL_SAVE_LONGS,RL_VALUE,mask,0);return restored(h); }
static RenderLeafState restore_longs(RenderLeafState w,const RenderLeafHooks *h,uint16_t mask) { observe(h,RL_RESTORE_LONGS,RL_VALUE,mask,0);return restored(h); }

/* Typed transfers keep the source's bus order and operand widths. */
#define S16(v) ((int16_t)(v))
#define S8(v) ((int8_t)(v))
#define SADDR(v) ((gaddr)(int32_t)(int16_t)(v))
#define NB(f,id) do {w.f=low_byte(w.f,(uint8_t)(0u-w.f));observe(h,RL_NEG_BYTE,id,0,0);}while(0)
#define ANDL(f,id,v) do {uint32_t n_=(uint32_t)(v);w.f&=n_;observe(h,RL_AND_LONG,id,n_,0);}while(0)
static RenderLeafState push_long(RenderLeafState w,const RenderLeafHooks *h,uint32_t v) {observe(h,RL_PUSH_LONG,RL_VALUE,v,0);return restored(h);}
static RenderLeafState pop_pointer(RenderLeafState w,const RenderLeafHooks *h,enum RenderLeafField f) {observe(h,RL_POP_POINTER,f,0,0);return restored(h);}
static RenderLeafState push_word(RenderLeafState w,const RenderLeafHooks *h,uint16_t v) {observe(h,RL_PUSH_WORD,RL_VALUE,v,0);return restored(h);}
static RenderLeafState pop_word(RenderLeafState w,const RenderLeafHooks *h,enum RenderLeafField f) {observe(h,RL_POP_WORD,f,0,0);return restored(h);}
static RenderLeafState link_frame(RenderLeafState w,const RenderLeafHooks *h,unsigned bytes) {observe(h,RL_LINK_FRAME,RL_VALUE,bytes,0);return restored(h);}
static void unlink_frame(const RenderLeafHooks *h) {observe(h,RL_UNLINK_FRAME,RL_VALUE,0,0);}
static void increment_word(const RenderLeafHooks *h,gaddr at,unsigned n) {uint16_t v=rd_u16(at);wr_u16(at,(uint16_t)(v+n));observe(h,RL_MEMORY_ADD_WORD,RL_VALUE,v,n);}
static void logic_long(const RenderLeafHooks *h,gaddr at,uint32_t mask,int set) {uint32_t v=rd_u32(at);v=set?v|mask:v&mask;wr_u32(at,v);observe(h,RL_MEMORY_LOGIC_LONG,RL_VALUE,v,0);}
static void logic_byte(const RenderLeafHooks *h,gaddr at,uint8_t mask,int set) {byte(h,at,set?rd_u8(at)|mask:rd_u8(at)&mask);}
static int dbra(uint32_t *v,const RenderLeafHooks *h,enum RenderLeafField f) {*v=low_word(*v,(uint16_t)(*v-1));observe(h,RL_DECREMENT,f,*v,0);return (uint16_t)*v!=0xffff;}
static void blitter_ready(const RenderLeafHooks *h,gaddr custom,gaddr source_pc) {observe(h,RL_WAIT_BLITTER,RL_VALUE,source_pc,custom);}

/* C2F826..C2FA6E: every selected colour writer, plane then row order.
 * The sixteen-slot C2F786/C2F7E6 tables and their four-bit selector are
 * sealed by audit_render_leaf_helpers_scope.py. Disabled planes still write. */
static void pixel_writer(RenderLeafState w,const RenderLeafHooks *h,unsigned colour,unsigned rows) {
 uint32_t keep[]={w.value,w.base,w.control,w.secondary};
 uint32_t set[]={w.source,w.stride,w.size,w.offset};
 gaddr planes[]={w.descriptor,w.stream,w.table,w.registers};
 unsigned plane,row;
 for(plane=0;plane<4;++plane)for(row=0;row<rows;++row){
  gaddr at=planes[plane]+40*row;uint16_t old=rd_u16(at);
  word(h,at,(uint16_t)((colour&(1u<<plane))?old|set[plane]:old&keep[plane]));
 }
}
/* C2F688: common pixel body; retain independent XOR and selected writer paths. */
static void pixel_body(RenderLeafState w,const RenderLeafHooks *h,unsigned rows) {
 unsigned colour;int toggled=0;
 TW(w.base);if(S16(w.base)<=0){L(control,RL_CONTROL,0xffffffffu);return;}
 W(control,RL_CONTROL,rd_u16(0xc45954));ANDW(control,RL_CONTROL,15);colour=(uint16_t)w.control;
 AW(control,RL_CONTROL,w.control);AW(control,RL_CONTROL,w.control);P(screen,RL_SCREEN,rd_u32(w.screen+SADDR(w.control)));
 W(control,RL_CONTROL,w.value);ANDW(value,RL_VALUE,15);AW(value,RL_VALUE,w.value);W(value,RL_VALUE,rd_u16(w.descriptor+SADDR(w.value)));W(offset,RL_OFFSET,w.value);W(value,RL_VALUE,~w.value);
 ASL(base,RL_BASE,3);W(secondary,RL_SECONDARY,w.base);AW(secondary,RL_SECONDARY,w.secondary);AW(secondary,RL_SECONDARY,w.secondary);AW(base,RL_BASE,w.secondary);
 ANDW(control,RL_CONTROL,0xfff0);ASR(control,RL_CONTROL,3);AW(control,RL_CONTROL,w.base);
 w=load_longs(w,h,w.table,0x0f00);
 P(registers,RL_REGISTERS,w.registers+SADDR(w.control));P(table,RL_TABLE,w.table+SADDR(w.control));P(stream,RL_STREAM,w.stream+SADDR(w.control));P(descriptor,RL_DESCRIPTOR,w.descriptor+SADDR(w.control));
 W(base,RL_BASE,w.value);W(control,RL_CONTROL,w.value);W(secondary,RL_SECONDARY,w.value);W(source,RL_SOURCE,w.offset);W(stride,RL_STRIDE,w.offset);W(size,RL_SIZE,w.offset);
 if(!bit(h,0xc456e7,0)){W(value,RL_VALUE,0xffff);W(source,RL_SOURCE,0);}
 if(!bit(h,0xc456e7,1)){W(base,RL_BASE,0xffff);W(stride,RL_STRIDE,0);}
 if(!bit(h,0xc456e7,2)){W(control,RL_CONTROL,0xffff);W(size,RL_SIZE,0);}
 if(!bit(h,0xc456e7,3)){W(secondary,RL_SECONDARY,0xffff);W(offset,RL_OFFSET,0);}
 TW(rd_u16(0xc456e8));if(rd_s16(0xc456e8)<0){pixel_writer(w,h,colour,rows);return;}
 ANDL(value,RL_VALUE,0xffff);
 if(bit(h,0xc456eb,0)){word(h,w.descriptor,rd_u16(w.descriptor)^(uint16_t)w.source);L(value,RL_VALUE,0xffffffffu);toggled=1;}
 if(bit(h,0xc456eb,1)){word(h,w.stream,rd_u16(w.stream)^(uint16_t)w.stride);L(value,RL_VALUE,0xffffffffu);toggled=1;}
 if(bit(h,0xc456eb,2)){word(h,w.table,rd_u16(w.table)^(uint16_t)w.size);L(value,RL_VALUE,0xffffffffu);toggled=1;}
 if(bit(h,0xc456eb,3)){word(h,w.registers,rd_u16(w.registers)^(uint16_t)w.offset);L(value,RL_VALUE,0xffffffffu);toggled=1;}
 TL(w.value);if(!toggled)pixel_writer(w,h,colour,rows);
}
static RenderLeafState pixel_prefix(RenderLeafState w,const RenderLeafHooks *h,gaddr masks,gaddr writers) {
 P(table,RL_TABLE,rd_u32(0xc456b6));P(descriptor,RL_DESCRIPTOR,masks);P(screen,RL_SCREEN,writers);return w;
}
void render_leaf_pixel(RenderLeafState w,const RenderLeafHooks *h) {pixel_body(pixel_prefix(w,h,0xc2f766,0xc2f786),h,1);}
static void restored_pixel(RenderLeafState w,const RenderLeafHooks *h) {
 w=pixel_prefix(w,h,0xc2f766,0xc2f786);w=push_word(w,h,(uint16_t)w.value);w=push_word(w,h,(uint16_t)w.base);w=consume(h,RL_CALL_C2F5EA);w=pop_word(w,h,RL_BASE);w=pop_word(w,h,RL_VALUE);
}
void render_leaf_pixel_restored(RenderLeafState w,const RenderLeafHooks *h) {restored_pixel(w,h);}
void render_leaf_pixel_in_view(RenderLeafState w,const RenderLeafHooks *h) {
 int sum=S16(w.value)+rd_s16(0xc45988);AW(value,RL_VALUE,rd_u16(0xc45988));
 if(sum<0){L(control,RL_CONTROL,0xffffffffu);return;}CW(320,w.value);if(S16(w.value)>=320){L(control,RL_CONTROL,0xffffffffu);return;}
 AW(base,RL_BASE,rd_u16(0xc458d8));restored_pixel(w,h);
}
static void pair(RenderLeafState w,const RenderLeafHooks *h) {
 W(control,RL_CONTROL,w.value);ANDW(control,RL_CONTROL,15);
 if((uint16_t)w.control){pixel_body(pixel_prefix(w,h,0xc2f7c6,0xc2f786),h,1);return;}
 observe(h,RL_SAVE_WORDS,RL_VALUE,0xc000,0);w=restored(h);w=consume(h,RL_CALL_C2F616);
 observe(h,RL_RESTORE_WORDS,RL_VALUE,3,0);w=restored(h);SW(value,RL_VALUE,1);w=consume(h,RL_CALL_C2F61E);
}
void render_leaf_pixel_pair(RenderLeafState w,const RenderLeafHooks *h) {pair(w,h);}
void render_leaf_pixel_block(RenderLeafState w,const RenderLeafHooks *h) {
 CW(rd_u16(0xc45984),w.base);if(S16(w.base)>=rd_s16(0xc45984))pair(w,h);else pixel_body(pixel_prefix(w,h,0xc2f7c6,0xc2f7e6),h,2);
}

/* C310E2: original signed overflow branches and long cursor displacement. */
void render_leaf_bound_span(RenderLeafState w,const RenderLeafHooks *h) {
 int sum;
 W(stride,RL_STRIDE,rd_u16(0xc45986));sum=S16(w.offset)+S16(w.stride);AW(offset,RL_OFFSET,w.stride);
 if(sum<0){
  NEG(offset,RL_OFFSET);CW(w.screen,w.offset);if(S16(w.offset)>=S16(w.screen)){L(stride,RL_STRIDE,0xffffffffu);return;}
  AW(stride,RL_STRIDE,w.offset);AW(stride,RL_STRIDE,w.stride);EL(stride,RL_STRIDE);AL(base,RL_BASE,w.stride);L(stride,RL_STRIDE,0);return;
 }
 AW(stride,RL_STRIDE,w.stride);EL(stride,RL_STRIDE);AL(base,RL_BASE,w.stride);W(stride,RL_STRIDE,w.offset);L(offset,RL_OFFSET,0);
 AW(stride,RL_STRIDE,w.screen);sum=S16(w.stride)-20;SW(stride,RL_STRIDE,20);if(sum<0){L(stride,RL_STRIDE,0);return;}
 CW(w.screen,w.stride);if(S16(w.stride)>=S16(w.screen)){L(stride,RL_STRIDE,0xffffffffu);return;}W(stride,RL_STRIDE,w.stride);
}

/* C32806: three-pixel glyph, inverse/draw/clear rows and exact MOVEM epilogue. */
void render_leaf_glyph3(RenderLeafState w,const RenderLeafHooks *h) {
 unsigned mode,n;uint16_t b;
 w=save_longs(w,h,0xc200);SWAP(control,RL_CONTROL);W(control,RL_CONTROL,w.secondary);SWAP(control,RL_CONTROL);W(size,RL_SIZE,w.offset);P(registers,RL_REGISTERS,w.source);P(descriptor,RL_DESCRIPTOR,w.base);
 w.size=low_word(w.size,(uint16_t)w.size>>6);observe(h,RL_LSR_WORD,RL_SIZE,6,0);SW(size,RL_SIZE,1);W(value,RL_VALUE,w.control);
 b=(uint16_t)w.control;w.control=low_word(w.control,(uint16_t)((b<<4)|(b>>12)));observe(h,RL_ROL_WORD,RL_CONTROL,4,0);
 ANDW(control,RL_CONTROL,15);ANDW(value,RL_VALUE,0xf0);mode=(uint16_t)w.value;
 if(mode){ANDW(value,RL_VALUE,0xc0);mode=(uint16_t)w.value?2:1;}
 do {
  L(base,RL_BASE,0xe0000000u);n=(unsigned)w.control&63u;
  if(mode){L(value,RL_VALUE,0);B(value,RL_VALUE,rd_u8(w.registers));w.value=(w.value>>8)|(w.value<<24);observe(h,RL_ROR_LONG,RL_VALUE,8,0);}
  w.base=n<32?w.base>>n:0;observe(h,RL_LSR_LONG,RL_BASE,n,0);
  if(mode){w.value=n<32?w.value>>n:0;observe(h,RL_LSR_LONG,RL_VALUE,n,0);}
  L(secondary,RL_SECONDARY,rd_u32(w.descriptor));
  if(mode){if(mode==1)L(value,RL_VALUE,~w.value);ANDL(value,RL_VALUE,w.base);}
  L(base,RL_BASE,~w.base);ANDL(base,RL_BASE,w.secondary);if(mode)L(base,RL_BASE,w.base|w.value);
  longword(h,w.descriptor,w.base);P(registers,RL_REGISTERS,w.registers+1);P(descriptor,RL_DESCRIPTOR,w.descriptor+40);
 }while(dbra(&w.size,h,RL_SIZE));
 SWAP(control,RL_CONTROL);W(secondary,RL_SECONDARY,w.control);SWAP(size,RL_SIZE);w=restore_longs(w,h,0x43);
}

/* C304B2: complete mask-clear packet and actual source busy read. */
void render_leaf_clear_mask(RenderLeafState w,const RenderLeafHooks *h) {
 W(value,RL_VALUE,rd_u16(0xc4596e));L(control,RL_CONTROL,rd_u32(0xc45960));L(base,RL_BASE,w.control);P(registers,RL_REGISTERS,0xdff000);
 blitter_ready(h,w.registers,0xc304c6);word(h,w.registers+0x40,0xd0c);word(h,w.registers+0x42,2);longword(h,w.registers+0x50,w.control);longword(h,w.registers+0x4c,w.base);longword(h,w.registers+0x54,w.base);word(h,w.registers+0x58,(uint16_t)w.value);
}

static RenderLeafState exchange(RenderLeafState w,const RenderLeafHooks *h,enum RenderLeafField first,enum RenderLeafField second) {
 observe(h,RL_EXCHANGE_DATA,first,second,0);(void)w;return restored(h);
}
static RenderLeafState divide_signed(RenderLeafState w,const RenderLeafHooks *h,enum RenderLeafField field,int16_t divisor) {
 observe(h,RL_DIVS,field,(uint16_t)divisor,0);(void)w;return restored(h);
}
/* C2FA84..C2FD20: source octants, clipped DIVS rounding and four plane packets. */
static void line_body(RenderLeafState w,const RenderLeafHooks *h) {
 unsigned plane;int negative,round;
 static const gaddr waits[]={0xc2fbba,0xc2fc22,0xc2fc8a,0xc2fcf0};
 word(h,0xc45956,rd_u16(0xc45954));CW(w.base,w.secondary);
 if((uint16_t)w.secondary==(uint16_t)w.base){AW(base,RL_BASE,1);AW(secondary,RL_SECONDARY,1);W(stride,RL_STRIDE,0);goto ascending;}
 if((uint16_t)w.secondary>(uint16_t)w.base){W(stride,RL_STRIDE,w.secondary);SW(stride,RL_STRIDE,w.base);SW(stride,RL_STRIDE,1);AW(base,RL_BASE,1);
ascending:
  CW(w.stream,w.base);if(S16(w.base)>S16(w.stream))return;
  P(table,RL_TABLE,SADDR(w.base));ASL(base,RL_BASE,3);W(offset,RL_OFFSET,w.base);AW(offset,RL_OFFSET,w.offset);AW(offset,RL_OFFSET,w.offset);AW(offset,RL_OFFSET,w.base);
  W(source,RL_SOURCE,w.control);SW(source,RL_SOURCE,w.value);W(size,RL_SIZE,w.value);w.value=low_word(w.value,(uint16_t)w.value>>3);observe(h,RL_LSR_WORD,RL_VALUE,3,0);AW(offset,RL_OFFSET,w.value);
 }else{
  W(stride,RL_STRIDE,w.base);SW(stride,RL_STRIDE,w.secondary);SW(stride,RL_STRIDE,1);W(source,RL_SOURCE,w.value);SW(source,RL_SOURCE,w.control);AW(secondary,RL_SECONDARY,1);
  CW(w.stream,w.secondary);if(S16(w.secondary)>S16(w.stream))return;
  P(table,RL_TABLE,SADDR(w.secondary));ASL(secondary,RL_SECONDARY,3);W(offset,RL_OFFSET,w.secondary);AW(offset,RL_OFFSET,w.offset);AW(offset,RL_OFFSET,w.offset);AW(offset,RL_OFFSET,w.secondary);
  W(size,RL_SIZE,w.control);w.control=low_word(w.control,(uint16_t)w.control>>3);observe(h,RL_LSR_WORD,RL_CONTROL,3,0);AW(offset,RL_OFFSET,w.control);
 }
 EL(offset,RL_OFFSET);L(base,RL_BASE,1);ANDW(size,RL_SIZE,15);
 {uint16_t b=(uint16_t)w.size;w.size=low_word(w.size,(uint16_t)((b>>4)|(b<<12)));observe(h,RL_ROR_WORD,RL_SIZE,4,0);}
 AW(size,RL_SIZE,0xb00);TW(w.source);negative=S16(w.source)<0;
 if(negative)NEG(source,RL_SOURCE);CW(w.stride,w.source);
 if((uint16_t)w.source>=(uint16_t)w.stride){
  AW(base,RL_BASE,negative?0x14:0x10);P(stream,RL_STREAM,w.stream-SADDR(w.table));CW(w.stream,w.stride);
  if(S16(w.stride)>S16(w.stream)){
   W(control,RL_CONTROL,w.stream);W(secondary,RL_SECONDARY,w.source);MUL(secondary,RL_SECONDARY,w.control);AL(secondary,RL_SECONDARY,w.secondary);
   w=divide_signed(w,h,RL_SECONDARY,S16(w.stride));round=w.secondary&1;ASR(secondary,RL_SECONDARY,1);if(round)AW(secondary,RL_SECONDARY,1);P(stream,RL_STREAM,SADDR(w.secondary));
  }else P(stream,RL_STREAM,SADDR(w.source));
 }else{
  if(negative)AW(base,RL_BASE,8);w=exchange(w,h,RL_SOURCE,RL_STRIDE);P(stream,RL_STREAM,w.stream-SADDR(w.table));CW(w.stream,w.source);if(S16(w.source)<=S16(w.stream))P(stream,RL_STREAM,SADDR(w.source));
 }
 AW(stride,RL_STRIDE,w.stride);AW(stride,RL_STRIDE,w.stride);AW(source,RL_SOURCE,w.source);W(control,RL_CONTROL,w.stride);
 {int sum=S16(w.control)-S16(w.source);SW(control,RL_CONTROL,w.source);if(sum<0){observe(h,RL_BIT_SET,RL_BASE,w.base,6);w.base|=64;}}
 W(secondary,RL_SECONDARY,w.stride);AW(source,RL_SOURCE,w.source);SW(stride,RL_STRIDE,w.source);W(source,RL_SOURCE,w.stream);ASL(source,RL_SOURCE,6);AW(source,RL_SOURCE,0x42);
 L(value,RL_VALUE,0xffffffffu);P(descriptor,RL_DESCRIPTOR,SADDR(w.stride));P(registers,RL_REGISTERS,0xdff000);
 blitter_ready(h,w.registers,0xc2fb54);word(h,w.registers+0x64,(uint16_t)w.descriptor);word(h,w.registers+0x66,40);word(h,w.registers+0x60,40);longword(h,w.registers+0x44,w.value);word(h,w.registers+0x72,(uint16_t)w.value);
 P(stream,RL_STREAM,rd_u32(0xc456b6));P(table,RL_TABLE,w.offset);
 for(plane=0;plane<4;++plane){
  if(!bit(h,0xc456e7,plane))continue;
  W(stride,RL_STRIDE,w.size);TW(rd_u16(0xc456e8));
  if(rd_s16(0xc456e8)<0){if(bit(h,0xc45957,plane))AW(stride,RL_STRIDE,0xfa);else AW(stride,RL_STRIDE,0xa);}
  else {if(bit(h,0xc456e9,plane))AW(stride,RL_STRIDE,0xfa);else AW(stride,RL_STRIDE,0xa);}
  L(offset,RL_OFFSET,rd_u32(w.stream+4*(3-plane)));AL(offset,RL_OFFSET,w.table);blitter_ready(h,w.registers,waits[plane]);
  word(h,w.registers+0x40,(uint16_t)w.stride);word(h,w.registers+0x42,(uint16_t)w.base);word(h,w.registers+0x52,(uint16_t)w.control);
  longword(h,w.registers+0x48,w.offset);longword(h,w.registers+0x54,w.offset);word(h,w.registers+0x74,0x8000);word(h,w.registers+0x62,(uint16_t)w.secondary);word(h,w.registers+0x58,(uint16_t)w.source);
 }
}
void render_leaf_line_to_row(RenderLeafState w,const RenderLeafHooks *h) {P(stream,RL_STREAM,199);line_body(w,h);}
void render_leaf_line(RenderLeafState w,const RenderLeafHooks *h) {P(stream,RL_STREAM,SADDR(rd_u16(0xc45984)));line_body(w,h);}

/* Actual source extent segments. Sorted polygon bounds make the NEG
 * arms cold in a stable whole call; retain their general segment behavior. */
RenderLeafState render_leaf_polygon_x_extent(RenderLeafState w,const RenderLeafHooks *h) {
 int sum;W(size,RL_SIZE,w.control);sum=S16(w.size)-S16(w.value);SW(size,RL_SIZE,w.value);if(sum<0)NEG(size,RL_SIZE);return w;
}
RenderLeafState render_leaf_polygon_y_extent(RenderLeafState w,const RenderLeafHooks *h) {
 int sum;W(offset,RL_OFFSET,w.secondary);sum=S16(w.offset)-S16(w.base);SW(offset,RL_OFFSET,w.base);if(sum<0)NEG(offset,RL_OFFSET);return w;
}
/* C301F0: complete polygon preparation, including tiny shapes and fill setup. */
static void polygon_body(RenderLeafState w,const RenderLeafHooks *h) {
 int count,sum;
 P(registers,RL_REGISTERS,0xc4b390);W(size,RL_SIZE,rd_u16(w.registers));P(registers,RL_REGISTERS,w.registers+2);SW(size,RL_SIZE,3);
 w=load_words(w,h,w.registers,0x3f);P(registers,RL_REGISTERS,w.registers+12);
 CW(w.value,w.control);if(S16(w.control)<S16(w.value))w=exchange(w,h,RL_VALUE,RL_CONTROL);
 CW(w.value,w.source);if(S16(w.source)<S16(w.value))W(value,RL_VALUE,w.source);else {CW(w.control,w.source);if(S16(w.source)>S16(w.control))W(control,RL_CONTROL,w.source);}
 CW(w.base,w.secondary);if(S16(w.secondary)<S16(w.base))w=exchange(w,h,RL_BASE,RL_SECONDARY);
 CW(w.base,w.stride);if(S16(w.stride)<S16(w.base))W(base,RL_BASE,w.stride);else {CW(w.secondary,w.stride);if(S16(w.stride)>S16(w.secondary))W(secondary,RL_SECONDARY,w.stride);}
 for(;;){
  count=S16(w.size)-1;SW(size,RL_SIZE,1);if(count<0)break;
  W(source,RL_SOURCE,rd_u16(w.registers));P(registers,RL_REGISTERS,w.registers+2);W(stride,RL_STRIDE,rd_u16(w.registers));P(registers,RL_REGISTERS,w.registers+2);
  CW(w.value,w.source);if(S16(w.source)<=S16(w.value))W(value,RL_VALUE,w.source);else {CW(w.source,w.control);if(S16(w.control)<S16(w.source))W(control,RL_CONTROL,w.source);}
  CW(w.base,w.stride);if(S16(w.stride)<=S16(w.base))W(base,RL_BASE,w.stride);else {CW(w.stride,w.secondary);if(S16(w.secondary)<S16(w.stride))W(secondary,RL_SECONDARY,w.stride);}
 }
 CW(w.screen,w.base);if(S16(w.base)>S16(w.screen))goto tiny_done;
 w=render_leaf_polygon_y_extent(w,h);
 CW(2,w.offset);if(S16(w.offset)>2){w=render_leaf_polygon_x_extent(w,h);CW(1,w.size);if(S16(w.size)<=1)goto tiny_line;goto fill;}
 CW(1,w.offset);if(S16(w.offset)<=1){
  w=render_leaf_polygon_x_extent(w,h);CW(1,w.size);if(S16(w.size)>1)goto tiny_line;
  AW(base,RL_BASE,1);CW(w.screen,w.base);if(S16(w.base)>S16(w.screen))goto tiny_done;
  sum=S16(w.size)-1;SW(size,RL_SIZE,1);if(sum<0)w=consume(h,RL_CALL_C302CE);else {W(value,RL_VALUE,w.control);w=consume(h,RL_CALL_C302D6);}goto tiny_done;
 }
 w=render_leaf_polygon_x_extent(w,h);CW(2,w.size);if(S16(w.size)>2)goto fill;
 AW(base,RL_BASE,1);CW(w.screen,w.base);if(S16(w.base)>S16(w.screen))goto tiny_done;W(value,RL_VALUE,w.control);w=consume(h,RL_CALL_C3028A);
tiny_done:
 L(value,RL_VALUE,1);return;
tiny_line:
 w=push_long(w,h,rd_u32(0xc456e6));TB(rd_u8(0xc457a2));if(!rd_u8(0xc457a2))longword(h,0xc456e6,0xfffff);
 w=consume(h,RL_CALL_C302B6);observe(h,RL_POP_MEMORY_LONG,RL_VALUE,0xc456e6,0);w=restored(h);L(value,RL_VALUE,1);return;
fill:
 sum=S16(w.value)-1;SW(value,RL_VALUE,1);if(sum<0)W(value,RL_VALUE,0);w=exchange(w,h,RL_BASE,RL_CONTROL);store_words(h,0xc4597c,15);word(h,0xc45956,rd_u16(0xc45954));
 P(stream,RL_STREAM,0xc4b390);P(descriptor,RL_DESCRIPTOR,0xc45970);word(h,w.descriptor,rd_u16(w.stream));P(stream,RL_STREAM,w.stream+2);decrement_word(h,w.descriptor);P(registers,RL_REGISTERS,0xdff000);
 do{
  w=load_words(w,h,w.stream,15);P(stream,RL_STREAM,w.stream+4);w=push_word(w,h,(uint16_t)w.screen);w=consume(h,RL_CALL_C30324);
  observe(h,RL_POP_POINTER_WORD,RL_SCREEN,0,0);w=restored(h);
 }while(decrement_word(h,w.descriptor)>0);
 W(value,RL_VALUE,rd_u16(w.stream));P(stream,RL_STREAM,w.stream+2);W(base,RL_BASE,rd_u16(w.stream));w=load_words(w,h,0xc4b392,12);
 w=push_word(w,h,(uint16_t)w.screen);w=consume(h,RL_CALL_C3033C);observe(h,RL_POP_POINTER_WORD,RL_SCREEN,0,0);w=restored(h);
 w=load_words(w,h,0xc4597c,10);ASR(secondary,RL_SECONDARY,4);W(control,RL_CONTROL,w.secondary);ASR(base,RL_BASE,4);SW(secondary,RL_SECONDARY,w.base);W(size,RL_SIZE,w.secondary);AW(secondary,RL_SECONDARY,w.secondary);NEG(secondary,RL_SECONDARY);AW(secondary,RL_SECONDARY,39);
 W(stride,RL_STRIDE,rd_u16(0xc45982));W(base,RL_BASE,w.stride);W(offset,RL_OFFSET,w.stride);ASL(base,RL_BASE,3);W(value,RL_VALUE,w.base);AW(value,RL_VALUE,w.value);AW(value,RL_VALUE,w.value);AW(base,RL_BASE,w.value);AW(control,RL_CONTROL,w.control);EL(control,RL_CONTROL);AL(base,RL_BASE,w.control);L(control,RL_CONTROL,w.base);
 CW(w.screen,w.offset);if(S16(w.offset)>S16(w.screen)){
  SW(offset,RL_OFFSET,w.screen);W(value,RL_VALUE,w.offset);ASL(offset,RL_OFFSET,3);W(source,RL_SOURCE,w.offset);AW(source,RL_SOURCE,w.source);AW(source,RL_SOURCE,w.source);AW(offset,RL_OFFSET,w.source);EL(offset,RL_OFFSET);SL(base,RL_BASE,w.offset);
 }else {L(value,RL_VALUE,0);L(offset,RL_OFFSET,0);}
 longword(h,0xc45968,w.base);L(base,RL_BASE,w.control);AL(base,RL_BASE,rd_u32(0xc456e2));SL(base,RL_BASE,w.offset);longword(h,0xc45960,w.base);L(control,RL_CONTROL,w.base);longword(h,0xc45964,w.base);
 SW(stride,RL_STRIDE,rd_u16(0xc45980));AW(stride,RL_STRIDE,1);W(offset,RL_OFFSET,w.stride);AW(size,RL_SIZE,1);SW(offset,RL_OFFSET,w.value);LSL(offset,RL_OFFSET,6);AW(offset,RL_OFFSET,w.size);word(h,0xc4596e,(uint16_t)w.offset);
 L(value,RL_VALUE,0xffffffffu);P(registers,RL_REGISTERS,0xdff000);blitter_ready(h,w.registers,0xc303d2);
 word(h,w.registers+0x40,0x9f0);word(h,w.registers+0x42,0xa);longword(h,w.registers+0x50,w.control);longword(h,w.registers+0x54,w.control);longword(h,w.registers+0x44,w.value);word(h,w.registers+0x64,(uint16_t)w.secondary);word(h,w.registers+0x62,(uint16_t)w.secondary);word(h,w.registers+0x66,(uint16_t)w.secondary);word(h,w.registers+0x58,(uint16_t)w.offset);L(value,RL_VALUE,0);
}

static RenderLeafState counted_wait(RenderLeafState w,const RenderLeafHooks *h,gaddr counter,gaddr pc) {
 for(;;){observe(h,RL_POLL_BUSY,RL_VALUE,pc,w.registers);w=restored(h);if(w.zero)break;increment_word(h,counter,1);observe(h,RL_POLL_REPEAT,RL_VALUE,0,0);}
 L(value,RL_VALUE,rd_u32(counter));W(secondary,RL_SECONDARY,w.value);SWAP(value,RL_VALUE);CW(w.secondary,w.value);
 if(S16(w.value)>S16(w.secondary))W(secondary,RL_SECONDARY,w.value);
 W(value,RL_VALUE,0);SWAP(value,RL_VALUE);W(value,RL_VALUE,w.secondary);longword(h,counter,w.value);return w;
}
/* C2FD8C: all four submissions, source busy peaks and actual display children. */
void render_leaf_submit_planes(RenderLeafState w,const RenderLeafHooks *h) {
 unsigned plane;static const gaddr counters[]={0xc4591c,0xc45920,0xc45924};static const gaddr polls[]={0xc2fe02,0xc2fe58,0xc2fea2};
 P(stream,RL_STREAM,rd_u32(0xc456b6));P(registers,RL_REGISTERS,0xdff000);W(size,RL_SIZE,rd_u16(0xc45984));ASL(size,RL_SIZE,6);AW(size,RL_SIZE,20);
 L(source,RL_SOURCE,rd_u32(w.stream));AL(source,RL_SOURCE,40);W(control,RL_CONTROL,0x100);L(stride,RL_STRIDE,0xffffffffu);blitter_ready(h,w.registers,0xc2fdb2);
 word(h,w.registers+0x40,(uint16_t)w.control);word(h,w.registers+0x42,0);word(h,w.registers+0x74,(uint16_t)w.stride);word(h,w.registers+0x44,(uint16_t)w.stride);word(h,w.registers+0x46,(uint16_t)w.stride);
 word(h,w.registers+0x62,1);word(h,w.registers+0x60,1);word(h,w.registers+0x66,1);longword(h,w.registers+0x48,w.source);longword(h,w.registers+0x54,w.source);word(h,w.registers+0x58,(uint16_t)w.size);
 for(plane=1;plane<4;++plane){
  L(source,RL_SOURCE,rd_u32(w.stream+4*plane));AL(source,RL_SOURCE,40);W(control,RL_CONTROL,plane==1?0x3fa:0x100);
  if(plane==2){TB(rd_u8(0xc4589b));if(rd_u8(0xc4589b))W(control,RL_CONTROL,0x3fa);}
  /* Timing adapter keeps the source interval from the previous BLTSIZE
   * write to this poll. C2FDF0..FE and C2FE90..9E cost 52; the middle
   * C2FE3A..54 interval costs 78/84 with its original condition byte. */
  observe(h,RL_POLL_PREFIX,RL_VALUE,plane==2?(rd_u8(0xc4589b)?84:78):52,0);
  w=counted_wait(w,h,counters[plane-1],polls[plane-1]);
  word(h,w.registers+0x40,(uint16_t)w.control);longword(h,w.registers+0x48,w.source);longword(h,w.registers+0x54,w.source);word(h,w.registers+0x58,(uint16_t)w.size);
 }
 w=push_long(w,h,rd_u32(0xc456e2));longword(h,0xc456e2,rd_u32(w.stream+12));w=consume(h,RL_CALL_C2FEEC);
 if(w.zero){w=consume(h,RL_CALL_C2FEF4);if(w.zero){TB(rd_u8(0xc4589b));if(rd_u8(0xc4589b)){L(value,RL_VALUE,8);L(secondary,RL_SECONDARY,0);L(source,RL_SOURCE,0);w=consume(h,RL_CALL_C2FF08);}}}
 observe(h,RL_POP_MEMORY_LONG,RL_VALUE,0xc456e2,0);w=restored(h);TB(rd_u8(0xc45785));if(rd_u8(0xc45785))goto drop_record;
 w=consume(h,RL_CALL_C2FF1A);if(!w.zero)goto drop_record;
 P(registers,RL_REGISTERS,0xc4b390);P(screen,RL_SCREEN,0xc4b432);word(h,w.screen,rd_u16(w.registers));P(registers,RL_REGISTERS,w.registers+2);P(screen,RL_SCREEN,w.screen+2);
 for(plane=0;plane<5;++plane){longword(h,w.screen,rd_u32(w.registers));P(registers,RL_REGISTERS,w.registers+4);P(screen,RL_SCREEN,w.screen+4);}return;
drop_record:
 word(h,0xc4b432,0);
}

void render_leaf_polygon_to_row(RenderLeafState w,const RenderLeafHooks *h) {P(screen,RL_SCREEN,199);polygon_body(w,h);}
void render_leaf_polygon(RenderLeafState w,const RenderLeafHooks *h) {P(screen,RL_SCREEN,SADDR(rd_u16(0xc45984)));polygon_body(w,h);}
void render_leaf_pixel_shared(RenderLeafState w,const RenderLeafHooks *h,unsigned rows) {pixel_body(w,h,rows);}

/* C2F64E: actual two-row child, preserving each original word stack slot. */
void render_leaf_square(RenderLeafState w,const RenderLeafHooks *h) {
 w=push_word(w,h,(uint16_t)w.value);w=push_word(w,h,(uint16_t)w.base);
 P(table,RL_TABLE,rd_u32(0xc456b6));P(descriptor,RL_DESCRIPTOR,0xc2f7c6);P(screen,RL_SCREEN,0xc2f7e6);
 w=consume(h,RL_CALL_C2F664);w=pop_word(w,h,RL_BASE);w=pop_word(w,h,RL_VALUE);
}
/* C2F63A: ADD's signed overflow condition precedes the 319-column bound. */
void render_leaf_square_in_view(RenderLeafState w,const RenderLeafHooks *h) {
 int sum=S16(w.value)+rd_s16(0xc45988);AW(value,RL_VALUE,rd_u16(0xc45988));
 if(sum<0){L(control,RL_CONTROL,0xffffffffu);return;}
 CW(319,w.value);if(S16(w.value)>=319){L(control,RL_CONTROL,0xffffffffu);return;}
 AW(base,RL_BASE,rd_u16(0xc458d8));render_leaf_square(w,h);
}

/* C330FE: eight-pixel glyph, individual long saves and original DBRA count. */
void render_leaf_glyph8(RenderLeafState w,const RenderLeafHooks *h) {
 unsigned mode,n;uint16_t b;
 w=push_long(w,h,w.value);w=push_long(w,h,w.size);
 SWAP(control,RL_CONTROL);W(control,RL_CONTROL,w.secondary);SWAP(control,RL_CONTROL);
 W(size,RL_SIZE,w.offset);P(registers,RL_REGISTERS,w.source);P(descriptor,RL_DESCRIPTOR,w.base);
 w.size=low_word(w.size,(uint16_t)w.size>>6);observe(h,RL_LSR_WORD,RL_SIZE,6,0);SW(size,RL_SIZE,1);W(value,RL_VALUE,w.control);
 b=(uint16_t)w.control;w.control=low_word(w.control,(uint16_t)((b<<4)|(b>>12)));observe(h,RL_ROL_WORD,RL_CONTROL,4,0);
 ANDW(control,RL_CONTROL,15);ANDW(value,RL_VALUE,0xf0);mode=(uint16_t)w.value;
 do {
  L(value,RL_VALUE,0);B(value,RL_VALUE,rd_u8(w.registers));ASL(value,RL_VALUE,8);SWAP(value,RL_VALUE);
  L(secondary,RL_SECONDARY,rd_u32(w.descriptor));n=(unsigned)w.control&63u;
  w.value=n<32?w.value>>n:0;observe(h,RL_LSR_LONG,RL_VALUE,n,0);
  L(value,RL_VALUE,~w.value);ANDL(secondary,RL_SECONDARY,w.value);
  if(mode){L(value,RL_VALUE,~w.value);L(secondary,RL_SECONDARY,w.secondary|w.value);}
  longword(h,w.descriptor,w.secondary);P(registers,RL_REGISTERS,w.registers+1);P(descriptor,RL_DESCRIPTOR,w.descriptor+40);
 }while(dbra(&w.size,h,RL_SIZE));
 SWAP(control,RL_CONTROL);W(secondary,RL_SECONDARY,w.control);SWAP(size,RL_SIZE);
 observe(h,RL_POP_LONG,RL_SIZE,0,0);w=restored(h);observe(h,RL_POP_LONG,RL_VALUE,0,0);
}
