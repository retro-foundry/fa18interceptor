/* Complete corner and view-record construction; original owners in corner_view_source_scope.json. */
#include "corner_view.h"
#include "projection.h"
#include "render_line.h"
#include <stdlib.h>
static void observe(const CornerViewHooks *h,enum CornerViewPhase p,enum CornerViewField f,uint32_t v,uint32_t o) {if(h&&h->observe)h->observe(h->context,p,f,v,o);}
static CornerViewState restored(const CornerViewHooks *h) {if(h&&h->restored)return h->restored(h->context);abort();}
static CornerViewState consume(const CornerViewHooks *h,enum CornerViewChild c) {if(h&&h->consume)return h->consume(h->context,c);abort();}
static uint32_t low_word(uint32_t old,uint16_t n) {return (old&0xffff0000u)|n;}
#define S(v) ((int16_t)(v))
#define W(f,id,v) do {w.f=low_word(w.f,(uint16_t)(v));observe(h,CV_WORD,id,w.f,0);}while(0)
#define L(f,id,v) do {w.f=(uint32_t)(v);observe(h,CV_LONG,id,w.f,0);}while(0)
#define P(f,id,v) do {w.f=(gaddr)(v);observe(h,CV_POINTER,id,w.f,0);}while(0)
#define AW(f,id,v) do {uint16_t n_=(uint16_t)(v);w.f=low_word(w.f,(uint16_t)(w.f+n_));observe(h,CV_ADD_WORD,id,n_,0);}while(0)
#define SW(f,id,v) do {uint16_t n_=(uint16_t)(v);w.f=low_word(w.f,(uint16_t)(w.f-n_));observe(h,CV_SUB_WORD,id,n_,0);}while(0)
#define NEG(f,id) do {w.f=low_word(w.f,(uint16_t)(0u-w.f));observe(h,CV_NEG_WORD,id,0,0);}while(0)
#define MUL(f,id,v) do {uint16_t n_=(uint16_t)(v);w.f=(uint32_t)((int32_t)S(w.f)*(int16_t)n_);observe(h,CV_MULS,id,n_,0);}while(0)
#define CW(src,dst) observe(h,CV_COMPARE_WORD,CV_FIRST_X,(uint16_t)(dst),(uint16_t)(src))
#define TW(v) observe(h,CV_TEST_WORD,CV_FIRST_X,(uint16_t)(v),0)
static void word(const CornerViewHooks *h,gaddr a,uint16_t v) {wr_u16(a,v);observe(h,CV_STORE_WORD,CV_FIRST_X,v,0);}
static void longword(const CornerViewHooks *h,gaddr a,uint32_t v) {wr_u32(a,v);observe(h,CV_STORE_LONG,CV_FIRST_X,v,0);}
static CornerViewState save(CornerViewState w,const CornerViewHooks *h,uint16_t mask) {observe(h,CV_SAVE_LONGS,CV_FIRST_X,mask,0);return restored(h);}
static CornerViewState restore(CornerViewState w,const CornerViewHooks *h,uint16_t mask) {(void)w;observe(h,CV_RESTORE_LONGS,CV_FIRST_X,mask,0);return restored(h);}
static CornerViewState load_words(CornerViewState w,const CornerViewHooks *h,gaddr at,uint16_t mask) {(void)w;observe(h,CV_LOAD_WORDS,CV_FIRST_X,at,mask);return restored(h);}
static void store_words(const CornerViewHooks *h,gaddr at,uint16_t mask) {observe(h,CV_STORE_WORDS,CV_FIRST_X,at,mask);}
static void divide(uint32_t *v,enum CornerViewField f,const CornerViewHooks *h,int16_t divisor) {
 if(divisor){int64_t q=(int64_t)(int32_t)*v/divisor;if(q>=-32768&&q<=32767)*v=((uint32_t)(uint16_t)((int64_t)(int32_t)*v%divisor)<<16)|(uint16_t)q;}
 observe(h,CV_DIVS,f,(uint16_t)divisor,0);
}
static void set_word(uint32_t *v,enum CornerViewField f,const CornerViewHooks *h,uint16_t n) {*v=low_word(*v,n);observe(h,CV_WORD,f,*v,0);}
static void subtract(uint32_t *v,enum CornerViewField f,const CornerViewHooks *h,uint16_t n) {*v=low_word(*v,(uint16_t)(*v-n));observe(h,CV_SUB_WORD,f,n,0);}
static void negate(uint32_t *v,enum CornerViewField f,const CornerViewHooks *h) {*v=low_word(*v,(uint16_t)(0u-*v));observe(h,CV_NEG_WORD,f,0,0);}
static void swap(uint32_t *v,enum CornerViewField f,const CornerViewHooks *h) {*v=(*v<<16)|(*v>>16);observe(h,CV_SWAP,f,0,0);}
static void multiply(uint32_t *v,enum CornerViewField f,const CornerViewHooks *h,uint16_t n) {*v=(uint32_t)((int32_t)S(*v)*(int16_t)n);observe(h,CV_MULS,f,n,0);}

#define AL(f,id,v) do {uint32_t n_=(uint32_t)(v);int64_t sum_=(int64_t)(int32_t)w.f+(int32_t)n_;w.f+=n_;w.less=sum_<0;observe(h,CV_ADD_LONG,id,n_,0);}while(0)
#define ASL(f,id,n) do {w.f=low_word(w.f,(uint16_t)(w.f<<(n)));observe(h,CV_ASL_WORD,id,n,0);}while(0)
#define ASR(f,id,n) do {unsigned n_=(n)&63u;w.f=n_<32?(uint32_t)((int32_t)w.f>>n_):((int32_t)w.f<0?0xffffffffu:0);observe(h,CV_ASR_LONG,id,n_,0);}while(0)
#define EXT(f,id) do {w.f=(uint32_t)(int32_t)S(w.f);observe(h,CV_EXT_LONG,id,0,0);}while(0)
#define NL(f,id) do {w.f=0u-w.f;observe(h,CV_NEG_LONG,id,0,0);}while(0)
static void byte(const CornerViewHooks *h,gaddr at,uint8_t v) {wr_u8(at,v);observe(h,CV_STORE_BYTE,CV_FIRST_X,v,0);}
static CornerViewState load_post(CornerViewState w,const CornerViewHooks *h,uint16_t mask) {
 if(h && h->restored){observe(h,CV_LOAD_WORDS_POST,CV_FIRST_X,mask,0);return restored(h);}
 uint32_t *values[]={&w.first_x,&w.first_y,&w.depth,&w.x,&w.y,&w.z,&w.scratch,&w.selector};
 for(unsigned i=0;i<8;++i) if(mask&(1u<<i)){*values[i]=(uint32_t)(int32_t)rd_s16(w.cursor);w.cursor+=2;}
 return w;
}
static CornerViewState stack_words(CornerViewState w,const CornerViewHooks *h,uint16_t mask,int restore_words) {(void)w;observe(h,restore_words?CV_RESTORE_WORDS:CV_SAVE_WORDS,CV_FIRST_X,mask,0);return restored(h);}
static void bit_zero(const CornerViewHooks *h,uint32_t v) {observe(h,CV_BIT_ZERO,CV_FIRST_X,v,0);}
static void memory_add(const CornerViewHooks *h,gaddr at) {uint16_t old=rd_u16(at);wr_u16(at,(uint16_t)(old+1));observe(h,CV_MEMORY_ADD_WORD,CV_FIRST_X,old,1);}

/* Each accepted rounded plane has its own original count/index pair. */
static CornerViewState crossing_child(CornerViewState w,const CornerViewHooks *h,enum CornerViewChild child) {
 static const unsigned planes[]={1,3,3,1,2,0,0,2,0,2,2,0,3,1,1,3};
 unsigned plane=planes[child];w=consume(h,child);
 if(w.zero){memory_add(h,0xc4e854u+2*plane);word(h,0xc4e85cu+2*plane,(uint16_t)w.first_x);}
 return w;
}
static void screen_coordinate(uint32_t *v,enum CornerViewField f,const CornerViewHooks *h,int16_t depth,unsigned span) {
 int sum;unsigned carry;multiply(v,f,h,(uint16_t)span);divide(v,f,h,depth);carry=*v&1u;
 *v=low_word(*v,(uint16_t)(S(*v)>>1));observe(h,CV_ASR_WORD,f,1,0);
 if(carry){*v=low_word(*v,(uint16_t)(*v+1));observe(h,CV_ADD_WORD,f,1,0);}
 sum=S(*v)+(int)(span/2);*v=low_word(*v,(uint16_t)(*v+span/2));observe(h,CV_ADD_WORD,f,span/2,0);
 if(sum<0)set_word(v,f,h,0);
 else {CW(span,*v);if(S(*v)>=(int)span)set_word(v,f,h,(uint16_t)(span-1));}
}
/* C2E758: follow each corner's original neighbours and four plane chain.
 * The caller argument remains live and is reread after actual children. */
void corner_project_edges(CornerViewState w,const CornerViewHooks *h) {
 int16_t clip_depth;
 observe(h,CV_LINK,CV_FRAME,4,0);w=restored(h);byte(h,w.frame-2,0);
 P(cursor,CV_CURSOR,0xc4b990u);P(points,CV_POINTS,0xc4b390u);W(first_x,CV_FIRST_X,0);word(h,w.frame+8,7);
endpoint:
 bit_zero(h,w.first_x);
 if(w.first_x&1u){W(x,CV_X,w.first_x);AW(x,CV_X,1);CW(rd_u16(w.frame+8),w.x);if(S(w.x)>rd_s16(w.frame+8))W(x,CV_X,0);W(first_y,CV_FIRST_Y,w.first_x);SW(first_y,CV_FIRST_Y,1);}
 else {W(x,CV_X,w.first_x);W(first_y,CV_FIRST_Y,w.first_x);AW(first_y,CV_FIRST_Y,2);CW(rd_u16(w.frame+8),w.first_y);if(S(w.first_y)>rd_s16(w.frame+8))W(first_y,CV_FIRST_Y,0);}
 ASL(first_y,CV_FIRST_Y,4);W(scratch,CV_SCRATCH,w.first_x);ASL(scratch,CV_SCRATCH,3);P(workspaces,CV_WORKSPACES,w.cursor+(gaddr)(int32_t)S(w.scratch));
 observe(h,CV_TEST_BYTE,CV_FIRST_X,rd_u8(w.frame-2),0);
 if(rd_u8(w.frame-2)){byte(h,w.frame-2,0);goto next;}
 ASL(x,CV_X,4);w=load_words(w,h,w.points+(gaddr)(int32_t)S(w.x),0x38);AW(x,CV_X,1);AW(y,CV_Y,1);
 /* Complete plane decisions are below. */
 CW(w.z,w.x);if(S(w.x)<S(w.z))goto negative_x;
 W(depth,CV_DEPTH,rd_u16(w.points+(gaddr)(int32_t)S(w.first_y)+0));W(scratch,CV_SCRATCH,rd_u16(w.points+(gaddr)(int32_t)S(w.first_y)+4));
 CW(w.depth,w.scratch);if(S(w.scratch)<=S(w.depth))goto reject;
 w=crossing_child(w,h,CV_CROSS_E7D0);if(w.zero)goto crossing;
 clip_depth=rd_s16(0xc45aca);TW(clip_depth);if(clip_depth>=0)goto other_y;
 NEG(depth,CV_DEPTH);CW(w.depth,w.scratch);if(S(w.scratch)<=S(w.depth))goto other_y;
 w=crossing_child(w,h,CV_CROSS_E7E6);if(w.zero)goto crossing;goto other_y;
negative_x:
 W(scratch,CV_SCRATCH,w.x);NEG(scratch,CV_SCRATCH);CW(w.z,w.scratch);
 if(S(w.scratch)<S(w.z))goto positive_y;
 W(depth,CV_DEPTH,rd_u16(w.points+(gaddr)(int32_t)S(w.first_y)+0));W(scratch,CV_SCRATCH,rd_u16(w.points+(gaddr)(int32_t)S(w.first_y)+4));NEG(depth,CV_DEPTH);
 CW(w.depth,w.scratch);if(S(w.scratch)<=S(w.depth))goto reject;
 w=crossing_child(w,h,CV_CROSS_E816);if(w.zero)goto crossing;
 clip_depth=rd_s16(0xc45aca);TW(clip_depth);if(clip_depth>=0)goto other_y;
 NEG(depth,CV_DEPTH);CW(w.depth,w.scratch);if(S(w.scratch)<=S(w.depth))goto other_y;
 w=crossing_child(w,h,CV_CROSS_E82C);if(w.zero)goto crossing;
other_y:
 TW(w.y);if(S(w.y)>=0)goto other_y_positive;
 W(scratch,CV_SCRATCH,w.y);NEG(scratch,CV_SCRATCH);CW(w.z,w.scratch);if(S(w.scratch)<S(w.z))goto reject;
 W(depth,CV_DEPTH,rd_u16(w.points+(gaddr)(int32_t)S(w.first_y)+2));W(scratch,CV_SCRATCH,rd_u16(w.points+(gaddr)(int32_t)S(w.first_y)+4));NEG(depth,CV_DEPTH);
 CW(w.depth,w.scratch);if(S(w.scratch)<=S(w.depth))goto reject;
 w=crossing_child(w,h,CV_CROSS_E852);if(w.zero)goto crossing;
 clip_depth=rd_s16(0xc45aca);TW(clip_depth);if(clip_depth>=0)goto reject;
 CW(w.z,w.y);if(S(w.y)<S(w.z))goto reject;
 NEG(depth,CV_DEPTH);CW(w.depth,w.scratch);if(S(w.scratch)<=S(w.depth))goto reject;
 w=crossing_child(w,h,CV_CROSS_E870);if(w.zero)goto crossing;goto reject;
other_y_positive:
 CW(w.z,w.y);if(S(w.y)<S(w.z))goto reject;
 W(depth,CV_DEPTH,rd_u16(w.points+(gaddr)(int32_t)S(w.first_y)+2));W(scratch,CV_SCRATCH,rd_u16(w.points+(gaddr)(int32_t)S(w.first_y)+4));
 CW(w.depth,w.scratch);if(S(w.scratch)<=S(w.depth))goto reject;
 w=crossing_child(w,h,CV_CROSS_E89C);if(w.zero)goto crossing;
 clip_depth=rd_s16(0xc45aca);TW(clip_depth);if(clip_depth>=0)goto reject;
 W(scratch,CV_SCRATCH,w.y);NEG(scratch,CV_SCRATCH);CW(w.z,w.scratch);if(S(w.scratch)<S(w.z))goto reject;
 W(scratch,CV_SCRATCH,rd_u16(w.points+(gaddr)(int32_t)S(w.first_y)+4));NEG(depth,CV_DEPTH);CW(w.depth,w.scratch);if(S(w.scratch)<=S(w.depth))goto reject;
 w=crossing_child(w,h,CV_CROSS_E8C2);if(w.zero)goto crossing;goto reject;
positive_y:
 CW(w.z,w.y);if(S(w.y)<S(w.z))goto negative_y;
 W(depth,CV_DEPTH,rd_u16(w.points+(gaddr)(int32_t)S(w.first_y)+2));W(scratch,CV_SCRATCH,rd_u16(w.points+(gaddr)(int32_t)S(w.first_y)+4));
 CW(w.depth,w.scratch);if(S(w.scratch)<=S(w.depth))goto reject;
 w=crossing_child(w,h,CV_CROSS_E8EC);if(w.zero)goto crossing;
 clip_depth=rd_s16(0xc45aca);TW(clip_depth);if(clip_depth>=0)goto other_x;
 NEG(depth,CV_DEPTH);CW(w.depth,w.scratch);if(S(w.scratch)<=S(w.depth))goto other_x;
 w=crossing_child(w,h,CV_CROSS_E900);if(w.zero)goto crossing;goto other_x;
negative_y:
 W(scratch,CV_SCRATCH,w.y);NEG(scratch,CV_SCRATCH);CW(w.z,w.scratch);if(S(w.scratch)<S(w.z)) {
  TW(w.z);if(S(w.z)>=0)goto inside;goto reject;
 }
 W(depth,CV_DEPTH,rd_u16(w.points+(gaddr)(int32_t)S(w.first_y)+2));W(scratch,CV_SCRATCH,rd_u16(w.points+(gaddr)(int32_t)S(w.first_y)+4));NEG(depth,CV_DEPTH);
 CW(w.depth,w.scratch);if(S(w.scratch)<=S(w.depth))goto reject;
 w=crossing_child(w,h,CV_CROSS_E930);if(w.zero)goto crossing;
 clip_depth=rd_s16(0xc45aca);TW(clip_depth);if(clip_depth>=0)goto other_x;
 NEG(depth,CV_DEPTH);CW(w.depth,w.scratch);if(S(w.scratch)<=S(w.depth))goto other_x;
 w=crossing_child(w,h,CV_CROSS_E944);if(w.zero)goto crossing;
other_x:
 TW(w.x);if(S(w.x)>=0)goto other_x_positive;
 W(scratch,CV_SCRATCH,w.x);NEG(scratch,CV_SCRATCH);CW(w.z,w.scratch);if(S(w.scratch)<S(w.z))goto reject;
 W(depth,CV_DEPTH,rd_u16(w.points+(gaddr)(int32_t)S(w.first_y)+0));W(scratch,CV_SCRATCH,rd_u16(w.points+(gaddr)(int32_t)S(w.first_y)+4));NEG(depth,CV_DEPTH);
 CW(w.depth,w.scratch);if(S(w.scratch)<=S(w.depth))goto reject;
 w=crossing_child(w,h,CV_CROSS_E968);if(w.zero)goto crossing;
 clip_depth=rd_s16(0xc45aca);TW(clip_depth);if(clip_depth>=0)goto reject;
 CW(w.z,w.x);if(S(w.x)<S(w.z))goto reject;
 NEG(depth,CV_DEPTH);CW(w.depth,w.scratch);if(S(w.scratch)<=S(w.depth))goto reject;
 w=crossing_child(w,h,CV_CROSS_E980);if(w.zero)goto crossing;goto reject;
other_x_positive:
 CW(w.z,w.x);if(S(w.x)<S(w.z))goto reject;
 W(depth,CV_DEPTH,rd_u16(w.points+(gaddr)(int32_t)S(w.first_y)+0));W(scratch,CV_SCRATCH,rd_u16(w.points+(gaddr)(int32_t)S(w.first_y)+4));
 CW(w.depth,w.scratch);if(S(w.scratch)<=S(w.depth))goto reject;
 w=crossing_child(w,h,CV_CROSS_E9A4);if(w.zero)goto crossing;
 clip_depth=rd_s16(0xc45aca);TW(clip_depth);if(clip_depth>=0)goto reject;
 W(scratch,CV_SCRATCH,w.x);NEG(scratch,CV_SCRATCH);CW(w.z,w.scratch);if(S(w.scratch)<S(w.z))goto reject;
 W(scratch,CV_SCRATCH,rd_u16(w.points+(gaddr)(int32_t)S(w.first_y)+4));NEG(depth,CV_DEPTH);CW(w.depth,w.scratch);if(S(w.scratch)<=S(w.depth))goto reject;
 w=crossing_child(w,h,CV_CROSS_E9C4);if(!w.zero)goto reject;

crossing:
 w=load_words(w,h,0xc45ac6,0x38);TW(w.z);
 if(S(w.z)<=0)for(;;)observe(h,CV_PARALLEL_LOOP,CV_FIRST_X,0xc2ea02u,0);
 screen_coordinate(&w.x,CV_X,h,S(w.z),320);screen_coordinate(&w.y,CV_Y,h,S(w.z),180);
 store_words(h,w.workspaces,0x18);goto next;
inside:
 bit_zero(h,w.first_x);if(w.first_x&1u)byte(h,w.frame-2,1);goto next;
reject:
 longword(h,w.workspaces,0);W(scratch,CV_SCRATCH,w.first_x);ASL(scratch,CV_SCRATCH,4);word(h,w.points+(gaddr)(int32_t)S(w.scratch)+14,0);
next:
 AW(first_x,CV_FIRST_X,1);CW(rd_u16(w.frame+8),w.first_x);if(S(w.first_x)<=rd_s16(w.frame+8))goto endpoint;
 observe(h,CV_UNLINK,CV_FRAME,0,0);
}

/* C2CE82: retain all three exact row-product/sum orders and depth write. */
CornerViewState corner_rotate_view(CornerViewState w,const CornerViewHooks *h) {
 P(cursor,CV_CURSOR,0xc45bd8);w=load_post(w,h,7);
 MUL(first_x,CV_FIRST_X,w.x);MUL(first_y,CV_FIRST_Y,w.y);MUL(depth,CV_DEPTH,w.z);
 AL(first_x,CV_FIRST_X,w.first_y);AL(first_x,CV_FIRST_X,w.depth);ASR(first_x,CV_FIRST_X,8);W(scratch,CV_SCRATCH,w.first_x);
 w=load_post(w,h,7);MUL(first_x,CV_FIRST_X,w.x);MUL(first_y,CV_FIRST_Y,w.y);MUL(depth,CV_DEPTH,w.z);
 AL(depth,CV_DEPTH,w.first_x);AL(depth,CV_DEPTH,w.first_y);ASR(depth,CV_DEPTH,8);
 MUL(x,CV_X,rd_u16(w.cursor));P(cursor,CV_CURSOR,w.cursor+2);
 MUL(y,CV_Y,rd_u16(w.cursor));P(cursor,CV_CURSOR,w.cursor+2);
 MUL(z,CV_Z,rd_u16(w.cursor));P(cursor,CV_CURSOR,w.cursor+2);
 AL(z,CV_Z,w.x);AL(z,CV_Z,w.y);ASR(z,CV_Z,8);word(h,0xc45ab6,(uint16_t)w.z);
 return w;
}

/* Shared record-position construction at C2CCBA and C2CDA6. */
static CornerViewState record_position(CornerViewState w,const CornerViewHooks *h) {
 W(x,CV_X,rd_u16(w.workspaces+0x30));SW(x,CV_X,rd_u16(0xc4594c));ASL(x,CV_X,8);ASL(x,CV_X,6);EXT(x,CV_X);
 W(z,CV_Z,rd_u16(w.workspaces+0x32));SW(z,CV_Z,rd_u16(0xc4594e));ASL(z,CV_Z,8);ASL(z,CV_Z,6);EXT(z,CV_Z);
 L(y,CV_Y,rd_u32(w.workspaces));ASR(y,CV_Y,8);AW(x,CV_X,w.y);AW(x,CV_X,rd_u16(0xc45a72));
 L(y,CV_Y,rd_u32(w.workspaces+8));ASR(y,CV_Y,8);AW(z,CV_Z,w.y);AW(z,CV_Z,rd_u16(0xc45a76));
 L(y,CV_Y,rd_u32(w.workspaces+4));AL(y,CV_Y,rd_u32(0xc45a66));ASR(y,CV_Y,8);return w;
}
void draw_control_record(int16_t index,enum ControlRecordDrawing drawing) {
 CornerViewState w={0};w.workspaces=0xc45c72u+(gaddr)(int32_t)(int16_t)((uint16_t)index<<6);
 wr_u16(0xc45954u,drawing==CONTROL_RECORD_LAYERS?13:9);
 if(drawing==CONTROL_RECORD_LAYERS && rd_s16(w.workspaces+40)<=58) {
  /* C2CD4C -> C2D082: one kind-0 layer; the original caller sets no shift. */
  int16_t cx=(int16_t)((uint16_t)(rd_u16(w.workspaces+48)-rd_u16(0xc4594cu))<<14);
  int16_t cz=(int16_t)((uint16_t)(rd_u16(w.workspaces+50)-rd_u16(0xc4594eu))<<14);
  uint32_t x=(uint32_t)(rd_s32(w.workspaces)>>8)+(uint32_t)(int32_t)cx;
  uint32_t y=(uint32_t)(rd_s32(w.workspaces+4)>>8)+rd_u32(0xc45a78u);
  uint32_t z=(uint32_t)(rd_s32(w.workspaces+8)>>8)+(uint32_t)(int32_t)cz;
  wr_u16(0xc456e6u,15);wr_u16(0xc456e8u,0xffff);wr_u32(0xc456eau,0);
  int32_t height=(int32_t)y;if(height<0) height=(int32_t)(0u-y);
  if(height>0x8000) return;
  int16_t scale=(int16_t)(58-rd_s16(w.workspaces+40));if(scale>12) scale=12;
  draw_shape((int16_t)(x+(uint32_t)(int32_t)rd_s16(0xc45a72u)),(int16_t)y,
             (int16_t)(z+(uint32_t)(int32_t)rd_s16(0xc45a76u)),(uint16_t)scale,0,0,0);
  return;
 }
 w=record_position(w,NULL);
 if(drawing==CONTROL_RECORD_PAIRS) {
  /* C2CD94: three original template pairs, transformed with C2CE82. */
  const int16_t origin[3]={(int16_t)w.x,(int16_t)w.y,(int16_t)w.z};
  gaddr stream=0xc2ce5eu;
  wr_u16(0xc456e6u,15);wr_u16(0xc456e8u,0xffff);wr_u32(0xc456eau,0);
  for(unsigned pair=0;pair<3;++pair) {
   for(unsigned point=0;point<2;++point) {
    w.x=(uint16_t)(rd_u16(stream)+origin[0]);w.y=(uint16_t)(rd_u16(stream+2)+origin[1]);
    w.z=(uint16_t)(rd_u16(stream+4)+origin[2]);stream+=6;
    w=corner_rotate_view(w,NULL);
    wr_u16(0xc4c592u+6*point,(uint16_t)w.scratch);
    wr_u16(0xc4c594u+6*point,(uint16_t)w.depth);wr_u16(0xc4c596u+6*point,(uint16_t)w.z);
   }
   draw_projected_segment();
  }
  return;
 }
 w=corner_rotate_view(w,NULL);
 const int visible=project_view_point_mode((int16_t)w.scratch,(int16_t)w.depth,(int16_t)w.z,-1,0,0);
 if(!visible || (rd_u16(w.workspaces+38)&0x202u))
  wr_u16(w.workspaces+38,rd_u16(w.workspaces+38)&0xfdffu);
}
static CornerViewState record_frame(CornerViewState w,const CornerViewHooks *h) {
 observe(h,CV_LINK,CV_FRAME,0x3e,0);w=restored(h);L(first_x,CV_FIRST_X,rd_u32(w.frame+8));ASL(first_x,CV_FIRST_X,6);
 P(workspaces,CV_WORKSPACES,0xc45c72);P(workspaces,CV_WORKSPACES,w.workspaces+(gaddr)(int32_t)S(w.first_x));return w;
}
/* C2CCA0/C2CD28: the latter's high record type shares the full test tail.
 * The low type unlinks and tail-transfers into the actual layer owner. */
void corner_test_record(CornerViewState w,const CornerViewHooks *h,int layered) {
 w=record_frame(w,h);word(h,0xc45954,layered?13:9);
 if(layered){
  CW(0x3a,rd_u16(w.workspaces+0x28));
  if(rd_s16(w.workspaces+0x28)<=0x3a){
   W(x,CV_X,rd_u16(w.workspaces+0x30));SW(x,CV_X,rd_u16(0xc4594c));ASL(x,CV_X,8);ASL(x,CV_X,6);EXT(x,CV_X);
   W(z,CV_Z,rd_u16(w.workspaces+0x32));SW(z,CV_Z,rd_u16(0xc4594e));ASL(z,CV_Z,8);ASL(z,CV_Z,6);EXT(z,CV_Z);
   observe(h,CV_LOAD_LONGS,CV_FIRST_X,w.workspaces,7);w=restored(h);
   ASR(first_x,CV_FIRST_X,8);ASR(first_y,CV_FIRST_Y,8);ASR(depth,CV_DEPTH,8);AL(first_x,CV_FIRST_X,w.x);AL(depth,CV_DEPTH,w.z);
   L(x,CV_X,0x3a);SW(x,CV_X,rd_u16(w.workspaces+0x28));CW(12,w.x);if(S(w.x)>12)L(x,CV_X,12);
   L(y,CV_Y,0);L(selector,CV_SELECTOR,0);observe(h,CV_UNLINK,CV_FRAME,0,0);w=restored(h);corner_draw_layers(w,h);return;
  }
 }
 w=record_position(w,h);w=consume(h,CV_RECORD_ROTATION);W(first_x,CV_FIRST_X,w.scratch);W(first_y,CV_FIRST_Y,w.depth);W(depth,CV_DEPTH,w.z);
 w=consume(h,CV_RECORD_TEST);
 if(!w.zero){W(first_x,CV_FIRST_X,rd_u16(w.workspaces+0x26));w.first_x=low_word(w.first_x,(uint16_t)(w.first_x&0x202));observe(h,CV_AND_WORD,CV_FIRST_X,0x202,0);if(!(uint16_t)w.first_x)goto finish;}
 word(h,w.workspaces+0x26,rd_u16(w.workspaces+0x26)&0xfdffu);
finish:observe(h,CV_UNLINK,CV_FRAME,0,0);
}

/* C2CD94 constructs three template pairs; each real child return governs
 * the next record and cursor, including changes to the loop's frame word. */
void corner_draw_record_pairs(CornerViewState w,const CornerViewHooks *h) {
 w=record_frame(w,h);w=record_position(w,h);store_words(h,w.frame-8,0x38);word(h,w.frame-10,3);
 word(h,0xc456e6,15);word(h,0xc456e8,0xffff);longword(h,0xc456ea,0);word(h,0xc45954,9);
 P(stream,CV_STREAM,0xc2ce5e);P(target,CV_TARGET,(gaddr)(int32_t)rd_s16(w.frame-8));P(modulo,CV_MODULO,(gaddr)(int32_t)rd_s16(w.frame-6));
pair:
 P(points,CV_POINTS,0xc4c592);
point:
 observe(h,CV_LOAD_WORDS_STREAM,CV_FIRST_X,0x38,0);w=restored(h);AW(x,CV_X,w.target);AW(y,CV_Y,w.modulo);AW(z,CV_Z,rd_u16(w.frame-4));
 w=consume(h,CV_PAIR_ROTATION);word(h,w.points,(uint16_t)w.scratch);P(points,CV_POINTS,w.points+2);
 word(h,w.points,(uint16_t)w.depth);P(points,CV_POINTS,w.points+2);word(h,w.points,(uint16_t)w.z);P(points,CV_POINTS,w.points+2);
 observe(h,CV_COMPARE_LONG,CV_FIRST_X,w.points,0xc4c59e);if((int32_t)w.points<(int32_t)0xc4c59e)goto point;
 observe(h,CV_PUSH_POINTER,CV_STREAM,w.stream,0);w=restored(h);w=consume(h,CV_PAIR_LINE);observe(h,CV_POP_POINTER,CV_STREAM,0,0);w=restored(h);
 {uint16_t old=rd_u16(w.frame-10);wr_u16(w.frame-10,(uint16_t)(old-1));observe(h,CV_MEMORY_SUB_WORD,CV_FIRST_X,old,1);if((int16_t)(old-1)>0)goto pair;}
 observe(h,CV_UNLINK,CV_FRAME,0,0);
}

static void unsigned_divide(uint32_t *v,enum CornerViewField f,const CornerViewHooks *h,uint16_t divisor) {
 if(divisor&&*v/divisor<=0xffffu)*v=((*v%divisor)<<16)|(*v/divisor);observe(h,CV_DIVU,f,divisor,0);
}
static void or_frame_word(const CornerViewHooks *h,gaddr at,uint16_t value) {word(h,at,rd_u16(at)|value);}
/* C2D082, also the C2CD28 tail: all three layers consume the original
 * C2D16C returns; MOVEM.W deliberately sign-extends restored high words. */
void corner_draw_layers(CornerViewState w,const CornerViewHooks *h) {
 word(h,0xc456e6,15);word(h,0xc456e8,0xffff);longword(h,0xc456ea,0);
 L(z,CV_Z,w.first_y);AL(z,CV_Z,rd_u32(0xc45a78));if(w.less)NL(z,CV_Z);
 observe(h,CV_COMPARE_LONG,CV_FIRST_X,w.z,0x8000);if((int32_t)w.z>0x8000)return;
 observe(h,CV_COMPARE_BYTE,CV_FIRST_X,(uint8_t)w.y,3);
 if((uint8_t)w.y==3){
  w=save(w,h,0xf900);w=consume(h,CV_SHAPE_DEPTH);W(z,CV_Z,w.first_y);w=restore(w,h,0x9f);
  W(scratch,CV_SCRATCH,0x2800);w.scratch=(uint32_t)(uint16_t)w.scratch*(uint16_t)w.x;observe(h,CV_MULU,CV_SCRATCH,(uint16_t)w.x,0);
  unsigned_divide(&w.scratch,CV_SCRATCH,h,6);AW(scratch,CV_SCRATCH,0x2800);EXT(scratch,CV_SCRATCH);TW(w.z);
  if(S(w.z)>0){unsigned_divide(&w.scratch,CV_SCRATCH,h,(uint16_t)w.z);CW(0x7f,w.scratch);if(S(w.scratch)<=0x7f)goto scale_ready;}
  W(scratch,CV_SCRATCH,0x7f);
scale_ready:
  swap(&w.selector,CV_SELECTOR,h);W(selector,CV_SELECTOR,w.scratch);swap(&w.selector,CV_SELECTOR,h);
 }
 AL(first_y,CV_FIRST_Y,rd_u32(0xc45a78));W(z,CV_Z,rd_u16(0xc45a72));W(scratch,CV_SCRATCH,rd_u16(0xc45a76));EXT(z,CV_Z);EXT(scratch,CV_SCRATCH);
 AL(first_x,CV_FIRST_X,w.z);AL(depth,CV_DEPTH,w.scratch);ASR(first_x,CV_FIRST_X,w.selector);ASR(first_y,CV_FIRST_Y,w.selector);ASR(depth,CV_DEPTH,w.selector);
 observe(h,CV_LINK,CV_FRAME,4,0);w=restored(h);word(h,w.frame-2,0);swap(&w.selector,CV_SELECTOR,h);W(scratch,CV_SCRATCH,w.selector);swap(&w.selector,CV_SELECTOR,h);
 w=stack_words(w,h,0xfb00,0);w=consume(h,CV_SHAPE_FIRST);or_frame_word(h,w.frame-2,(uint16_t)w.first_x);w=stack_words(w,h,0xdf,1);
 observe(h,CV_COMPARE_BYTE,CV_FIRST_X,(uint8_t)w.y,3);if((int8_t)w.y<3)goto done;
 AW(y,CV_Y,1);{int n=S(w.x)-1;SW(x,CV_X,1);if(n<0)L(x,CV_X,0);}
 w=stack_words(w,h,0xfb00,0);w=consume(h,CV_SHAPE_SECOND);or_frame_word(h,w.frame-2,(uint16_t)w.first_x);w=stack_words(w,h,0xdf,1);
 observe(h,CV_COMPARE_BYTE,CV_FIRST_X,(uint8_t)w.y,4);if((int8_t)w.y<4)goto done;
 AW(y,CV_Y,1);{int n=S(w.x)-1;SW(x,CV_X,1);if(n<0)L(x,CV_X,0);}AW(first_x,CV_FIRST_X,20);AW(first_y,CV_FIRST_Y,10);AW(depth,CV_DEPTH,20);
 w=consume(h,CV_SHAPE_THIRD);or_frame_word(h,w.frame-2,(uint16_t)w.first_x);
done:
 W(first_x,CV_FIRST_X,rd_u16(w.frame-2));observe(h,CV_UNLINK,CV_FRAME,0,0);
}
/* C2D3A4: original unsigned-width additions use signed branch flags before
 * optional 4/8-bit scaling and the existing three-coordinate distance child. */
void corner_distance(CornerViewState w,const CornerViewHooks *h) {
 w=load_words(w,h,0xc45a72,0x38);L(selector,CV_SELECTOR,0);
 AL(first_x,CV_FIRST_X,w.x);if(w.less)NL(first_x,CV_FIRST_X);AL(first_y,CV_FIRST_Y,w.y);if(w.less)NL(first_y,CV_FIRST_Y);AL(depth,CV_DEPTH,w.z);if(w.less)NL(depth,CV_DEPTH);
 L(scratch,CV_SCRATCH,0x4000);observe(h,CV_COMPARE_LONG,CV_FIRST_X,w.first_x,w.scratch);
 if((int32_t)w.first_x>(int32_t)w.scratch)goto scaled;
 observe(h,CV_COMPARE_LONG,CV_FIRST_X,w.first_y,w.scratch);if((int32_t)w.first_y>(int32_t)w.scratch)goto scaled;
 observe(h,CV_COMPARE_LONG,CV_FIRST_X,w.depth,w.scratch);if((int32_t)w.depth<=(int32_t)w.scratch)goto distance;
scaled:
 L(selector,CV_SELECTOR,8);L(scratch,CV_SCRATCH,0x40000);observe(h,CV_COMPARE_LONG,CV_FIRST_X,w.first_x,w.scratch);
 if((int32_t)w.first_x>(int32_t)w.scratch)goto shift;
 observe(h,CV_COMPARE_LONG,CV_FIRST_X,w.first_y,w.scratch);if((int32_t)w.first_y>(int32_t)w.scratch)goto shift;
 observe(h,CV_COMPARE_LONG,CV_FIRST_X,w.depth,w.scratch);if((int32_t)w.depth<=(int32_t)w.scratch)L(selector,CV_SELECTOR,4);
shift:ASR(first_x,CV_FIRST_X,w.selector);ASR(first_y,CV_FIRST_Y,w.selector);ASR(depth,CV_DEPTH,w.selector);
distance:W(y,CV_Y,w.depth);W(x,CV_X,w.first_y);W(depth,CV_DEPTH,w.first_x);(void)consume(h,CV_DISTANCE);
}
/* Original separately callable renderer return tails. */
void corner_return_list(CornerViewState w,const CornerViewHooks *h) {observe(h,CV_RESTORE_LONGS_POST,CV_FIRST_X,0x2200,0);w=restored(h);W(first_x,CV_FIRST_X,rd_u16(w.frame-0x7e));}
void corner_reject(CornerViewState w,const CornerViewHooks *h,int value) {L(first_x,CV_FIRST_X,(uint32_t)value);}
