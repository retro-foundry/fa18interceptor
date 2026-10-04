/* Complete C1FF9C/C1FFA4, C2ED70/C2EE4A and eight original view crossings. */
#include "segment_projection.h"
#include <stdlib.h>
static void observe(const SegmentProjectionHooks *h,enum SegmentProjectionPhase p,enum SegmentProjectionField f,uint32_t v,uint32_t o) {if(h&&h->observe)h->observe(h->context,p,f,v,o);}
static SegmentProjectionState restored(const SegmentProjectionHooks *h) {if(h&&h->restored)return h->restored(h->context);abort();}
static SegmentProjectionState consume(const SegmentProjectionHooks *h,enum SegmentProjectionChild c) {if(h&&h->consume)return h->consume(h->context,c);abort();}
static uint32_t low_word(uint32_t old,uint16_t n) {return (old&0xffff0000u)|n;}
#define S(v) ((int16_t)(v))
#define W(f,id,v) do {w.f=low_word(w.f,(uint16_t)(v));observe(h,SP_WORD,id,w.f,0);}while(0)
#define L(f,id,v) do {w.f=(uint32_t)(v);observe(h,SP_LONG,id,w.f,0);}while(0)
#define P(f,id,v) do {w.f=(gaddr)(v);observe(h,SP_POINTER,id,w.f,0);}while(0)
#define AW(f,id,v) do {uint16_t n_=(uint16_t)(v);w.f=low_word(w.f,(uint16_t)(w.f+n_));observe(h,SP_ADD_WORD,id,n_,0);}while(0)
#define SW(f,id,v) do {uint16_t n_=(uint16_t)(v);w.f=low_word(w.f,(uint16_t)(w.f-n_));observe(h,SP_SUB_WORD,id,n_,0);}while(0)
#define NEG(f,id) do {w.f=low_word(w.f,(uint16_t)(0u-w.f));observe(h,SP_NEG_WORD,id,0,0);}while(0)
#define MUL(f,id,v) do {uint16_t n_=(uint16_t)(v);w.f=(uint32_t)((int32_t)S(w.f)*(int16_t)n_);observe(h,SP_MULS,id,n_,0);}while(0)
#define CW(src,dst) observe(h,SP_COMPARE_WORD,SP_FIRST_X,(uint16_t)(dst),(uint16_t)(src))
#define TW(v) observe(h,SP_TEST_WORD,SP_FIRST_X,(uint16_t)(v),0)
static void word(const SegmentProjectionHooks *h,gaddr a,uint16_t v) {wr_u16(a,v);observe(h,SP_STORE_WORD,SP_FIRST_X,v,0);}
static void longword(const SegmentProjectionHooks *h,gaddr a,uint32_t v) {wr_u32(a,v);observe(h,SP_STORE_LONG,SP_FIRST_X,v,0);}
static SegmentProjectionState save(SegmentProjectionState w,const SegmentProjectionHooks *h,uint16_t mask) {observe(h,SP_SAVE_LONGS,SP_FIRST_X,mask,0);return restored(h);}
static SegmentProjectionState restore(SegmentProjectionState w,const SegmentProjectionHooks *h,uint16_t mask) {(void)w;observe(h,SP_RESTORE_LONGS,SP_FIRST_X,mask,0);return restored(h);}
static SegmentProjectionState load_words(SegmentProjectionState w,const SegmentProjectionHooks *h,gaddr at,uint16_t mask) {(void)w;observe(h,SP_LOAD_WORDS,SP_FIRST_X,at,mask);return restored(h);}
static void store_words(const SegmentProjectionHooks *h,gaddr at,uint16_t mask) {observe(h,SP_STORE_WORDS,SP_FIRST_X,at,mask);}
static void divide(uint32_t *v,enum SegmentProjectionField f,const SegmentProjectionHooks *h,int16_t divisor) {
 if(divisor){int64_t q=(int64_t)(int32_t)*v/divisor;if(q>=-32768&&q<=32767)*v=((uint32_t)(uint16_t)((int64_t)(int32_t)*v%divisor)<<16)|(uint16_t)q;}
 observe(h,SP_DIVS,f,(uint16_t)divisor,0);
}
static void set_word(uint32_t *v,enum SegmentProjectionField f,const SegmentProjectionHooks *h,uint16_t n) {*v=low_word(*v,n);observe(h,SP_WORD,f,*v,0);}
static void subtract(uint32_t *v,enum SegmentProjectionField f,const SegmentProjectionHooks *h,uint16_t n) {*v=low_word(*v,(uint16_t)(*v-n));observe(h,SP_SUB_WORD,f,n,0);}
static void negate(uint32_t *v,enum SegmentProjectionField f,const SegmentProjectionHooks *h) {*v=low_word(*v,(uint16_t)(0u-*v));observe(h,SP_NEG_WORD,f,0,0);}
static void swap(uint32_t *v,enum SegmentProjectionField f,const SegmentProjectionHooks *h) {*v=(*v<<16)|(*v>>16);observe(h,SP_SWAP,f,0,0);}
static void multiply(uint32_t *v,enum SegmentProjectionField f,const SegmentProjectionHooks *h,uint16_t n) {*v=(uint32_t)((int32_t)S(*v)*(int16_t)n);observe(h,SP_MULS,f,n,0);}

/* A projected coordinate retains quotient/remainder words and the original
 * ADD overflow branch before its wrapped-word bounds comparison. */
static void project_coordinate(uint32_t *v,enum SegmentProjectionField f,const SegmentProjectionHooks *h,int16_t depth,unsigned scale,unsigned limit) {
 int sum;multiply(v,f,h,(uint16_t)scale);divide(v,f,h,depth);sum=S(*v)+(int)scale;
 *v=low_word(*v,(uint16_t)(*v+scale));observe(h,SP_ADD_WORD,f,scale,0);
 if(sum<0)set_word(v,f,h,0);
 else {CW(limit,*v);if(S(*v)>=(int)limit)set_word(v,f,h,(uint16_t)(limit-1));}
}
static void mirror_coordinate(uint32_t *v,enum SegmentProjectionField f,const SegmentProjectionHooks *h,unsigned limit) {
 subtract(v,f,h,(uint16_t)(limit-1));negate(v,f,h);
}

/* C1FF9C/C1FFA4: the actual selected child consumes all copied vertices. */
void segment_selected(SegmentProjectionState w,const SegmentProjectionHooks *h,int clipped) {
 if(clipped){
  int32_t y=rd_s32(0xc45a78);observe(h,SP_COMPARE_LONG,SP_FIRST_X,(uint32_t)y,0xffffff40u);
  if(y>=-192){P(stream,SP_STREAM,w.stream+6);L(first_x,SP_FIRST_X,0);return;}
 }
 P(target,SP_TARGET,clipped?0xc2ee4au:0xc2ed70u);longword(h,0xc456e6,0xffffffffu);
 P(cursor,SP_CURSOR,0xc4c592u);P(workspaces,SP_WORKSPACES,0xc48390u);
 W(first_y,SP_FIRST_Y,rd_u16(w.stream));P(stream,SP_STREAM,w.stream+2);
 longword(h,w.cursor,rd_u32(w.workspaces+(gaddr)(int32_t)S(w.first_y)));P(cursor,SP_CURSOR,w.cursor+4);
 W(scratch,SP_SCRATCH,rd_u16(w.workspaces+(gaddr)(int32_t)S(w.first_y)+4));word(h,w.cursor,(uint16_t)w.scratch);P(cursor,SP_CURSOR,w.cursor+2);
 W(first_y,SP_FIRST_Y,rd_u16(w.stream));P(stream,SP_STREAM,w.stream+2);
 longword(h,w.cursor,rd_u32(w.workspaces+(gaddr)(int32_t)S(w.first_y)));P(cursor,SP_CURSOR,w.cursor+4);
 W(selector,SP_SELECTOR,rd_u16(w.workspaces+(gaddr)(int32_t)S(w.first_y)+4));word(h,0xc45954,rd_u16(w.stream));P(stream,SP_STREAM,w.stream+2);
 w.scratch=low_word(w.scratch,(uint16_t)(w.scratch&w.selector));observe(h,SP_AND_WORD,SP_SCRATCH,w.selector,0);
 if(S(w.scratch)<0){L(first_x,SP_FIRST_X,0);return;}
 word(h,w.cursor,(uint16_t)w.selector);P(cursor,SP_CURSOR,w.cursor+2);
 w=save(w,h,0x0064);w=consume(h,clipped?SP_SELECTED_CLIPPED:SP_SELECTED_PROJECTED);(void)restore(w,h,0x2600);
}

static SegmentProjectionState next_first_point(SegmentProjectionState w,const SegmentProjectionHooks *h) {
 W(first_x,SP_FIRST_X,rd_u16(w.points));P(points,SP_POINTS,w.points+2);
 W(first_y,SP_FIRST_Y,rd_u16(w.points));P(points,SP_POINTS,w.points+2);
 W(depth,SP_DEPTH,rd_u16(w.points));P(points,SP_POINTS,w.points+2);return w;
}
/* C2ED70: reject each original axis before projecting the complete pair. */
void segment_projected(SegmentProjectionState w,const SegmentProjectionHooks *h) {
 P(points,SP_POINTS,0xc4c592u);w=next_first_point(w,h);if(S(w.depth)<=0)goto reject;
 CW(w.depth,w.first_x);if(S(w.first_x)>S(w.depth))goto reject;
 W(z,SP_Z,w.first_x);NEG(z,SP_Z);CW(w.depth,w.z);if(S(w.z)>S(w.depth))goto reject;
 CW(w.depth,w.first_y);if(S(w.first_y)>S(w.depth))goto reject;
 W(z,SP_Z,w.first_y);NEG(z,SP_Z);CW(w.depth,w.z);if(S(w.z)>S(w.depth))goto reject;
 project_coordinate(&w.first_x,SP_FIRST_X,h,S(w.depth),160,320);project_coordinate(&w.first_y,SP_FIRST_Y,h,S(w.depth),90,180);
 mirror_coordinate(&w.first_x,SP_FIRST_X,h,320);mirror_coordinate(&w.first_y,SP_FIRST_Y,h,180);
 W(depth,SP_DEPTH,rd_u16(w.points));P(points,SP_POINTS,w.points+2);
 W(x,SP_X,rd_u16(w.points));P(points,SP_POINTS,w.points+2);
 W(y,SP_Y,rd_u16(w.points));P(points,SP_POINTS,w.points+2);if(S(w.y)<=0)goto reject;
 CW(w.y,w.depth);if(S(w.depth)>S(w.y))goto reject;
 W(z,SP_Z,w.depth);NEG(z,SP_Z);CW(w.y,w.z);if(S(w.z)>S(w.y))goto reject;
 CW(w.y,w.x);if(S(w.x)>S(w.y))goto reject;
 W(z,SP_Z,w.x);NEG(z,SP_Z);CW(w.y,w.z);if(S(w.z)>S(w.y))goto reject;
 project_coordinate(&w.depth,SP_DEPTH,h,S(w.y),160,320);project_coordinate(&w.x,SP_X,h,S(w.y),90,180);
 mirror_coordinate(&w.depth,SP_DEPTH,h,320);mirror_coordinate(&w.x,SP_X,h,180);
 w=consume(h,SP_PROJECTED_LINE);L(first_x,SP_FIRST_X,1);return;
reject:L(first_x,SP_FIRST_X,0);
}

/* C2EE4A: clip each endpoint against the four view planes, then exchange
 * the actual point words for the second pass. Each crossing consumes its
 * returned registers, flags and CLIP_POINT; none of them is reconstructed. */
void segment_clipped(SegmentProjectionState w,const SegmentProjectionHooks *h) {
 uint16_t counter;int16_t clip_depth;
 observe(h,SP_LINK,SP_FRAME,4,0);w=restored(h);
 P(points,SP_POINTS,0xc4c592u);P(cursor,SP_CURSOR,0xc4b390u);word(h,w.frame-2,1);
endpoint:
 w=load_words(w,h,w.points,0x38);
 CW(w.z,w.x);if(S(w.x)<S(w.z))goto negative_x;
 W(depth,SP_DEPTH,rd_u16(w.points+6));W(scratch,SP_SCRATCH,rd_u16(w.points+10));
 CW(w.depth,w.scratch);if(S(w.scratch)<=S(w.depth))goto reject;
 w=consume(h,SP_CROSS_EE74);if(w.zero)goto crossing;
 clip_depth=rd_s16(0xc45aca);TW(clip_depth);if(clip_depth>=0)goto other_y;
 NEG(depth,SP_DEPTH);CW(w.depth,w.scratch);if(S(w.scratch)<=S(w.depth))goto other_y;
 w=consume(h,SP_CROSS_EE8A);if(w.zero)goto crossing;goto other_y;
negative_x:
 W(scratch,SP_SCRATCH,w.x);NEG(scratch,SP_SCRATCH);CW(w.z,w.scratch);
 if(S(w.scratch)<S(w.z))goto positive_y;
 W(depth,SP_DEPTH,rd_u16(w.points+6));W(scratch,SP_SCRATCH,rd_u16(w.points+10));NEG(depth,SP_DEPTH);
 CW(w.depth,w.scratch);if(S(w.scratch)<=S(w.depth))goto reject;
 w=consume(h,SP_CROSS_EEAC);if(w.zero)goto crossing;
 clip_depth=rd_s16(0xc45aca);TW(clip_depth);if(clip_depth>=0)goto other_y;
 NEG(depth,SP_DEPTH);CW(w.depth,w.scratch);if(S(w.scratch)<=S(w.depth))goto other_y;
 w=consume(h,SP_CROSS_EEC2);if(w.zero)goto crossing;
other_y:
 TW(w.y);if(S(w.y)>=0)goto other_y_positive;
 W(scratch,SP_SCRATCH,w.y);NEG(scratch,SP_SCRATCH);CW(w.z,w.scratch);if(S(w.scratch)<S(w.z))goto reject;
 W(depth,SP_DEPTH,rd_u16(w.points+8));W(scratch,SP_SCRATCH,rd_u16(w.points+10));NEG(depth,SP_DEPTH);
 CW(w.depth,w.scratch);if(S(w.scratch)<=S(w.depth))goto reject;
 w=consume(h,SP_CROSS_EEE8);if(w.zero)goto crossing;
 clip_depth=rd_s16(0xc45aca);TW(clip_depth);if(clip_depth>=0)goto reject;
 CW(w.z,w.y);if(S(w.y)<S(w.z))goto reject;
 NEG(depth,SP_DEPTH);CW(w.depth,w.scratch);if(S(w.scratch)<=S(w.depth))goto reject;
 w=consume(h,SP_CROSS_EF06);if(w.zero)goto crossing;goto reject;
other_y_positive:
 CW(w.z,w.y);if(S(w.y)<S(w.z))goto reject;
 W(depth,SP_DEPTH,rd_u16(w.points+8));W(scratch,SP_SCRATCH,rd_u16(w.points+10));
 CW(w.depth,w.scratch);if(S(w.scratch)<=S(w.depth))goto reject;
 w=consume(h,SP_CROSS_EF26);if(w.zero)goto crossing;
 clip_depth=rd_s16(0xc45aca);TW(clip_depth);if(clip_depth>=0)goto reject;
 W(scratch,SP_SCRATCH,w.y);NEG(scratch,SP_SCRATCH);CW(w.z,w.scratch);if(S(w.scratch)<S(w.z))goto reject;
 W(scratch,SP_SCRATCH,rd_u16(w.points+10));NEG(depth,SP_DEPTH);CW(w.depth,w.scratch);if(S(w.scratch)<=S(w.depth))goto reject;
 w=consume(h,SP_CROSS_EF4C);if(w.zero)goto crossing;goto reject;
positive_y:
 CW(w.z,w.y);if(S(w.y)<S(w.z))goto negative_y;
 W(depth,SP_DEPTH,rd_u16(w.points+8));W(scratch,SP_SCRATCH,rd_u16(w.points+10));
 CW(w.depth,w.scratch);if(S(w.scratch)<=S(w.depth))goto reject;
 w=consume(h,SP_CROSS_EF6A);if(w.zero)goto crossing;
 clip_depth=rd_s16(0xc45aca);TW(clip_depth);if(clip_depth>=0)goto other_x;
 NEG(depth,SP_DEPTH);CW(w.depth,w.scratch);if(S(w.scratch)<=S(w.depth))goto other_x;
 w=consume(h,SP_CROSS_EF7E);if(w.zero)goto crossing;goto other_x;
negative_y:
 W(scratch,SP_SCRATCH,w.y);NEG(scratch,SP_SCRATCH);CW(w.z,w.scratch);if(S(w.scratch)<S(w.z)) {
  TW(w.z);if(S(w.z)>=0)goto project;goto reject;
 }
 W(depth,SP_DEPTH,rd_u16(w.points+8));W(scratch,SP_SCRATCH,rd_u16(w.points+10));NEG(depth,SP_DEPTH);
 CW(w.depth,w.scratch);if(S(w.scratch)<=S(w.depth))goto reject;
 w=consume(h,SP_CROSS_EFA2);if(w.zero)goto crossing;
 clip_depth=rd_s16(0xc45aca);TW(clip_depth);if(clip_depth>=0)goto other_x;
 NEG(depth,SP_DEPTH);CW(w.depth,w.scratch);if(S(w.scratch)<=S(w.depth))goto other_x;
 w=consume(h,SP_CROSS_EFB6);if(w.zero)goto crossing;
other_x:
 TW(w.x);if(S(w.x)>=0)goto other_x_positive;
 W(scratch,SP_SCRATCH,w.x);NEG(scratch,SP_SCRATCH);CW(w.z,w.scratch);if(S(w.scratch)<S(w.z))goto reject;
 W(depth,SP_DEPTH,rd_u16(w.points+6));W(scratch,SP_SCRATCH,rd_u16(w.points+10));NEG(depth,SP_DEPTH);
 CW(w.depth,w.scratch);if(S(w.scratch)<=S(w.depth))goto reject;
 w=consume(h,SP_CROSS_EFD8);if(w.zero)goto crossing;
 clip_depth=rd_s16(0xc45aca);TW(clip_depth);if(clip_depth>=0)goto reject;
 CW(w.z,w.x);if(S(w.x)<S(w.z))goto reject;
 NEG(depth,SP_DEPTH);CW(w.depth,w.scratch);if(S(w.scratch)<=S(w.depth))goto reject;
 w=consume(h,SP_CROSS_EFF0);if(w.zero)goto crossing;goto reject;
other_x_positive:
 CW(w.z,w.x);if(S(w.x)<S(w.z))goto reject;
 W(depth,SP_DEPTH,rd_u16(w.points+6));W(scratch,SP_SCRATCH,rd_u16(w.points+10));
 CW(w.depth,w.scratch);if(S(w.scratch)<=S(w.depth))goto reject;
 w=consume(h,SP_CROSS_F008);if(w.zero)goto crossing;
 clip_depth=rd_s16(0xc45aca);TW(clip_depth);if(clip_depth>=0)goto reject;
 W(scratch,SP_SCRATCH,w.x);NEG(scratch,SP_SCRATCH);CW(w.z,w.scratch);if(S(w.scratch)<S(w.z))goto reject;
 W(scratch,SP_SCRATCH,rd_u16(w.points+10));NEG(depth,SP_DEPTH);CW(w.depth,w.scratch);if(S(w.scratch)<=S(w.depth))goto reject;
 w=consume(h,SP_CROSS_F028);if(!w.zero)goto reject;
crossing:
 w=load_words(w,h,0xc45ac6,0x38);
project:
 TW(w.z);if(S(w.z)<=0){word(h,0xc4599e,0x16);goto reject;}
 project_coordinate(&w.x,SP_X,h,S(w.z),160,320);project_coordinate(&w.y,SP_Y,h,S(w.z),90,180);
 mirror_coordinate(&w.x,SP_X,h,320);mirror_coordinate(&w.y,SP_Y,h,180);
 word(h,w.cursor,(uint16_t)w.x);P(cursor,SP_CURSOR,w.cursor+2);word(h,w.cursor,(uint16_t)w.y);P(cursor,SP_CURSOR,w.cursor+2);
 counter=rd_u16(w.frame-2);wr_u16(w.frame-2,(uint16_t)(counter-1));observe(h,SP_MEMORY_SUB_WORD,SP_FIRST_X,counter,1);
 if((uint16_t)(counter-1)==0){
  w=load_words(w,h,w.points,0x3f);store_words(h,w.points,0x38);store_words(h,w.points+6,7);goto endpoint;
 }
 w=load_words(w,h,0xc4b390,0xf);w=consume(h,SP_CLIPPED_LINE);
 observe(h,SP_UNLINK,SP_FRAME,0,0);w=restored(h);L(first_x,SP_FIRST_X,1);return;
reject:
 observe(h,SP_UNLINK,SP_FRAME,0,0);w=restored(h);L(first_x,SP_FIRST_X,0);
}

/* Original rounded crossing quotient: both SWAPs and word-only remainder
 * negation are observable on overflow; comparison uses the signed half. */
static void rounded_divide(uint32_t *v,enum SegmentProjectionField f,const SegmentProjectionHooks *h,int16_t divisor,int16_t half) {
 divide(v,f,h,divisor);swap(v,f,h);TW(*v);if(S(*v)<0)negate(v,f,h);
 CW(*v,half);
 if(half>S(*v)){swap(v,f,h);return;}
 swap(v,f,h);TW(*v);
 if(S(*v)<0)subtract(v,f,h,1);
 else {*v=low_word(*v,(uint16_t)(*v+1));observe(h,SP_ADD_WORD,f,1,0);}
}

/* C2EA5A/C2EAD0/C2EB4C/C2EBC2 round; C2F0C6/C2F0F4/C2F128/C2F156
 * truncate and retain their original parallel-case BEQ-to-self loops. */
void segment_crossing(SegmentProjectionState w,const SegmentProjectionHooks *h,int axis,int side,int rounded) {
 uint32_t *far_axis,*far_other,*close_axis,*close_other;enum SegmentProjectionField af,of,cf,co;
 int16_t denominator,half=0;uint16_t distance;gaddr source_loop;
 gaddr at=w.points+(rounded?(gaddr)(int32_t)S(w.first_y):6u);
 w=save(w,h,0xfe00);w=load_words(w,h,at,7);
 if(rounded){SW(x,SP_X,1);SW(y,SP_Y,1);}
 far_axis=axis?&w.first_y:&w.first_x;far_other=axis?&w.first_x:&w.first_y;
 close_axis=axis?&w.y:&w.x;close_other=axis?&w.x:&w.y;
 af=axis?SP_FIRST_Y:SP_FIRST_X;of=axis?SP_FIRST_X:SP_FIRST_Y;cf=axis?SP_Y:SP_X;co=axis?SP_X:SP_Y;
 if(side<0){negate(far_axis,af,h);negate(close_axis,cf,h);}
 W(scratch,SP_SCRATCH,w.depth);SW(scratch,SP_SCRATCH,w.z);
 subtract(far_axis,af,h,(uint16_t)w.depth);negate(far_axis,af,h);
 subtract(close_axis,cf,h,(uint16_t)w.z);
 *close_axis=low_word(*close_axis,(uint16_t)(*close_axis+*far_axis));observe(h,SP_ADD_WORD,cf,*far_axis,0);
 denominator=S(*close_axis);distance=(uint16_t)*far_axis;
 if(!denominator){
  if(rounded)goto outside;
  source_loop=axis?(side<0?0xc2f170u:0xc2f13eu):(side<0?0xc2f10eu:0xc2f0dcu);
  for(;;)observe(h,SP_PARALLEL_LOOP,SP_FIRST_X,source_loop,0);
 }
 subtract(close_other,co,h,(uint16_t)*far_other);negate(close_other,co,h);multiply(close_other,co,h,distance);
 if(rounded){
  W(z,SP_Z,*close_axis);if(S(w.z)<0)NEG(z,SP_Z);
  w.z=low_word(w.z,(uint16_t)(S(w.z)>>1));observe(h,SP_ASR_WORD,SP_Z,1,0);half=S(w.z);
  rounded_divide(close_other,co,h,denominator,half);
 }else divide(close_other,co,h,denominator);
 subtract(far_other,of,h,(uint16_t)*close_other);MUL(scratch,SP_SCRATCH,distance);
 if(rounded)rounded_divide(&w.scratch,SP_SCRATCH,h,denominator,half);else divide(&w.scratch,SP_SCRATCH,h,denominator);
 SW(depth,SP_DEPTH,w.scratch);set_word(far_axis,af,h,(uint16_t)w.depth);if(side<0)negate(far_axis,af,h);
 store_words(h,0xc45ac6,7);W(z,SP_Z,w.depth);if(S(w.z)<0)goto outside;
 W(scratch,SP_SCRATCH,w.depth);CW(w.z,w.first_x);if(S(w.first_x)>S(w.z))goto outside;
 NEG(first_x,SP_FIRST_X);CW(w.z,w.first_x);if(S(w.first_x)>S(w.z))goto outside;
 CW(w.scratch,w.first_y);if(S(w.first_y)>S(w.scratch))goto outside;
 NEG(first_y,SP_FIRST_Y);CW(w.scratch,w.first_y);if(S(w.first_y)>S(w.scratch))goto outside;
 L(first_x,SP_FIRST_X,0);(void)restore(w,h,0x007f);return;
outside:L(first_x,SP_FIRST_X,1);(void)restore(w,h,0x007f);
}
