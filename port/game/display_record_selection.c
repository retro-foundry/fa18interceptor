/* Complete C0D74A/C0D752 owners and C0DAA0/C0DAD0/C0DAD4/C0DADC/C0DAE6.
 * Rounded matrix products, actual corner-child outputs and original pair
 * writers replace the older enclosing register replay. */
#include "display_record_selection.h"
#include <stdlib.h>
static void observe(const DisplaySelectionHooks *h,enum DisplaySelectionPhase p,enum DisplaySelectionField f,uint32_t v,uint32_t other){if(h&&h->observe)h->observe(h->context,p,f,v,other);}
static DisplaySelectionState restored(const DisplaySelectionHooks *h){if(h&&h->restored)return h->restored(h->context);abort();}
static DisplaySelectionState consume(const DisplaySelectionHooks *h,gaddr ret){if(h&&h->consume)return h->consume(h->context,ret);abort();}
static uint32_t low_word(uint32_t old,uint32_t value){return (old&0xffff0000u)|(uint16_t)value;}
#define W(f,id,v) do{s.f=low_word(s.f,(v));observe(h,DS_WORD,id,s.f,0);}while(0)
#define L(f,id,v) do{s.f=(uint32_t)(v);observe(h,DS_LONG,id,s.f,0);}while(0)
#define P(f,id,v) do{s.f=(gaddr)(v);observe(h,DS_POINTER,id,s.f,0);}while(0)
#define AW(f,id,v) do{uint16_t n_=(uint16_t)(v);s.f=low_word(s.f,s.f+n_);observe(h,DS_ADD_WORD,id,n_,0);}while(0)
#define SW(f,id,v) do{uint16_t n_=(uint16_t)(v);s.f=low_word(s.f,s.f-n_);observe(h,DS_SUB_WORD,id,n_,0);}while(0)
#define AL(f,id,v) do{uint32_t n_=(v);s.f+=n_;observe(h,DS_ADD_LONG,id,n_,0);}while(0)
#define AND(f,id,v) do{s.f=low_word(s.f,s.f&(v));observe(h,DS_AND_WORD,id,v,0);}while(0)
#define ASW(f,id,n) do{s.f=low_word(s.f,(uint16_t)((int16_t)s.f>>(n)));observe(h,DS_ASR_WORD,id,n,0);}while(0)
#define ALW(f,id,n) do{s.f=low_word(s.f,(uint16_t)(s.f<<(n)));observe(h,DS_ASL_WORD,id,n,0);}while(0)
#define SWAP(f,id) do{s.f=(s.f<<16)|(s.f>>16);observe(h,DS_SWAP,id,0,0);}while(0)
#define MUL(f,id,v) do{uint16_t n_=(v);s.f=(uint32_t)((int32_t)(int16_t)s.f*(int16_t)n_);observe(h,DS_MULTIPLY_WORD,id,n_,0);}while(0)
#define CALL(ret) do{s=consume(h,ret);}while(0)
static void word(const DisplaySelectionHooks *h,gaddr at,uint16_t value){wr_u16(at,value);observe(h,DS_STORE_WORD,DS_FIRST,value,0);}
static void longword(const DisplaySelectionHooks *h,gaddr at,uint32_t value){wr_u32(at,value);observe(h,DS_STORE_LONG,DS_FIRST,value,0);}
static void byte(const DisplaySelectionHooks *h,gaddr at,uint8_t value){wr_u8(at,value);observe(h,DS_STORE_BYTE,DS_FIRST,value,0);}
static int test_word(const DisplaySelectionHooks *h,gaddr at){uint16_t v=rd_u16(at);observe(h,DS_TEST_WORD,DS_FIRST,v,0);return v!=0;}
static int test_byte(const DisplaySelectionHooks *h,gaddr at){uint8_t v=rd_u8(at);observe(h,DS_TEST_BYTE,DS_FIRST,v,0);return v!=0;}
static void compare(const DisplaySelectionHooks *h,uint16_t value,uint16_t bound){observe(h,DS_COMPARE_WORD,DS_FIRST,value,bound);}
static void rounded_row(uint32_t *value,enum DisplaySelectionField field,const DisplaySelectionHooks *h){
 unsigned carry=(*value>>7)&1u;*value=(uint32_t)((int32_t)*value>>8);observe(h,DS_ASR_LONG,field,8,0);
 if(carry){*value=low_word(*value,*value+1);observe(h,DS_ADD_WORD,field,1,0);}
}
void append_display_mirrored_pair(DisplaySelectionState s,const DisplaySelectionHooks *h){
 ALW(first,DS_FIRST,3);ALW(second,DS_SECOND,3);P(counter,DS_COUNTER,0xc4b990u);
 W(x,DS_X,319);W(z,DS_Z,s.x);SW(x,DS_X,rd_u16(s.counter+(uint32_t)(int32_t)(int16_t)s.first));
 W(y,DS_Y,179);W(row_x,DS_ROW_X,s.y);SW(y,DS_Y,rd_u16(s.counter+2+(uint32_t)(int32_t)(int16_t)s.first));
 word(h,s.input,(uint16_t)s.x);P(input,DS_INPUT,s.input+2);word(h,s.input,(uint16_t)s.y);P(input,DS_INPUT,s.input+2);
 SW(z,DS_Z,rd_u16(s.counter+(uint32_t)(int32_t)(int16_t)s.second));SW(row_x,DS_ROW_X,rd_u16(s.counter+2+(uint32_t)(int32_t)(int16_t)s.second));
 word(h,s.input,(uint16_t)s.z);P(input,DS_INPUT,s.input+2);word(h,s.input,(uint16_t)s.row_x);P(input,DS_INPUT,s.input+2);
}
void append_display_zero_pair(DisplaySelectionState s,const DisplaySelectionHooks *h){longword(h,s.input,0);P(input,DS_INPUT,s.input+4);}
static void fixed_pair(DisplaySelectionState s,uint16_t x,uint16_t y,const DisplaySelectionHooks *h){word(h,s.input,x);P(input,DS_INPUT,s.input+2);word(h,s.input,y);P(input,DS_INPUT,s.input+2);}
void append_display_upper_right(DisplaySelectionState s,const DisplaySelectionHooks *h){fixed_pair(s,319,0,h);}
void append_display_lower_right(DisplaySelectionState s,const DisplaySelectionHooks *h){fixed_pair(s,319,179,h);}
void append_display_lower_left(DisplaySelectionState s,const DisplaySelectionHooks *h){fixed_pair(s,0,179,h);}
void prepare_display_record_selection(DisplaySelectionState s,int wide,const DisplaySelectionHooks *h){
 unsigned pair;uint16_t count;int extended;
 P(coefficients,DS_COEFFICIENTS,wide?0xc45beau:0xc45bd8u);P(matrix,DS_MATRIX,s.coefficients);
 observe(h,DS_LINK,DS_FRAME,2,0);s=restored(h);
 P(input,DS_INPUT,0xc0d720u);P(output,DS_OUTPUT,0xc4b390u);P(counter,DS_COUNTER,s.frame-2);word(h,s.counter,4);
 do{
  W(x,DS_X,rd_u16(s.input));P(input,DS_INPUT,s.input+2);L(y,DS_Y,rd_u32(0xc45a66u));SWAP(y,DS_Y);ASW(y,DS_Y,2);
  W(z,DS_Z,rd_u16(s.input));P(input,DS_INPUT,s.input+2);P(coefficients,DS_COEFFICIENTS,s.matrix);
  for(pair=0;pair<2;++pair){
   W(row_x,DS_ROW_X,s.x);W(row_y,DS_ROW_Y,s.y);W(row_z,DS_ROW_Z,s.z);
   MUL(row_x,DS_ROW_X,rd_u16(s.coefficients));P(coefficients,DS_COEFFICIENTS,s.coefficients+2);
   MUL(row_y,DS_ROW_Y,rd_u16(s.coefficients));P(coefficients,DS_COEFFICIENTS,s.coefficients+2);
   MUL(row_z,DS_ROW_Z,rd_u16(s.coefficients));P(coefficients,DS_COEFFICIENTS,s.coefficients+2);
   AL(row_z,DS_ROW_Z,s.row_y);AL(row_z,DS_ROW_Z,s.row_x);rounded_row(&s.row_z,DS_ROW_Z,h);
   word(h,s.output,(uint16_t)s.row_z);P(output,DS_OUTPUT,s.output+2);
  }
  MUL(x,DS_X,rd_u16(s.coefficients));P(coefficients,DS_COEFFICIENTS,s.coefficients+2);
  MUL(y,DS_Y,rd_u16(s.coefficients));P(coefficients,DS_COEFFICIENTS,s.coefficients+2);
  MUL(z,DS_Z,rd_u16(s.coefficients));P(coefficients,DS_COEFFICIENTS,s.coefficients+2);
  AL(z,DS_Z,s.y);AL(z,DS_Z,s.x);rounded_row(&s.z,DS_Z,h);word(h,s.output,(uint16_t)s.z);P(output,DS_OUTPUT,s.output+2);
  W(y,DS_Y,s.z);P(output,DS_OUTPUT,s.output+0x1a);count=rd_u16(s.counter);wr_u16(s.counter,(uint16_t)(count-1));observe(h,DS_MEMORY_SUB_WORD,DS_FIRST,count,1);
 }while((int16_t)(count-1)>0);
 P(matrix,DS_MATRIX,0xc4e854u);for(pair=0;pair<3;++pair){longword(h,s.matrix,0);P(matrix,DS_MATRIX,s.matrix+4);}longword(h,s.matrix,0);
 CALL(0xc0d7e0u);P(input,DS_INPUT,0xc4e854u);
 if(test_word(h,s.input+2)){
  if(test_word(h,s.input+6)){W(first,DS_FIRST,rd_u16(s.input+10));W(second,DS_SECOND,rd_u16(s.input+14));goto bottom;}
  if(test_word(h,s.input)){W(first,DS_FIRST,rd_u16(s.input+10));W(second,DS_SECOND,rd_u16(s.input+8));goto right;}
  if(test_word(h,s.input+4)){W(first,DS_FIRST,rd_u16(s.input+10));W(second,DS_SECOND,rd_u16(s.input+12));goto right_reverse;}
  goto reject;
 }
 if(test_word(h,s.input)){
  if(test_word(h,s.input+6)){W(first,DS_FIRST,rd_u16(s.input+8));W(second,DS_SECOND,rd_u16(s.input+14));goto left_reverse;}
  if(test_word(h,s.input+4)){W(first,DS_FIRST,rd_u16(s.input+8));W(second,DS_SECOND,rd_u16(s.input+12));goto top;}
  goto reject;
 }
 if(!test_word(h,s.input+4))goto full_rectangle;
 if(test_word(h,s.input+6)){W(first,DS_FIRST,rd_u16(s.input+12));W(second,DS_SECOND,rd_u16(s.input+14));goto left;}
 goto reject;
bottom:
 P(input,DS_INPUT,0xc4b390u);word(h,s.input,4);P(input,DS_INPUT,s.input+2);
 extended=test_byte(h,0xc45785u);if(!extended){W(row_y,DS_ROW_Y,rd_u16(0xc458cau));AND(row_y,DS_ROW_Y,2);extended=!(uint16_t)s.row_y;}
 if(extended){CALL(0xc0d894u);CALL(0xc0d898u);CALL(0xc0d89cu);P(input,DS_INPUT,0xc4b39au);CALL(0xc0d8a6u);CALL(0xc0d8aau);}
 else{CALL(0xc0d8b2u);CALL(0xc0d8b6u);CALL(0xc0d8bau);}goto accept;
right:
 P(input,DS_INPUT,0xc4b390u);word(h,s.input,5);P(input,DS_INPUT,s.input+2);W(row_y,DS_ROW_Y,rd_u16(0xc458cau));AND(row_y,DS_ROW_Y,2);
 if(!(uint16_t)s.row_y){CALL(0xc0d8d8u);CALL(0xc0d8dcu);CALL(0xc0d8e0u);CALL(0xc0d8e4u);P(input,DS_INPUT,0xc4b390u);word(h,s.input,3);P(input,DS_INPUT,s.input+2);P(input,DS_INPUT,s.input+8);CALL(0xc0d8f4u);}
 else{CALL(0xc0d8fcu);CALL(0xc0d900u);CALL(0xc0d904u);CALL(0xc0d908u);}goto accept;
right_reverse:
 P(input,DS_INPUT,0xc4b390u);word(h,s.input,5);P(input,DS_INPUT,s.input+2);W(row_y,DS_ROW_Y,rd_u16(0xc458cau));AND(row_y,DS_ROW_Y,2);
 if(!(uint16_t)s.row_y){CALL(0xc0d926u);CALL(0xc0d92au);CALL(0xc0d92eu);CALL(0xc0d932u);}
 else{CALL(0xc0d93au);CALL(0xc0d93eu);CALL(0xc0d942u);CALL(0xc0d946u);P(input,DS_INPUT,0xc4b390u);word(h,s.input,3);P(input,DS_INPUT,s.input+2);P(input,DS_INPUT,s.input+8);CALL(0xc0d956u);}goto accept;
left_reverse:
 P(input,DS_INPUT,0xc4b390u);word(h,s.input,5);P(input,DS_INPUT,s.input+2);W(row_y,DS_ROW_Y,rd_u16(0xc458cau));AND(row_y,DS_ROW_Y,2);
 if(!(uint16_t)s.row_y){CALL(0xc0d974u);CALL(0xc0d978u);CALL(0xc0d97cu);CALL(0xc0d980u);P(input,DS_INPUT,0xc4b390u);word(h,s.input,3);P(input,DS_INPUT,s.input+2);P(input,DS_INPUT,s.input+8);CALL(0xc0d990u);}
 else{CALL(0xc0d998u);CALL(0xc0d99cu);CALL(0xc0d9a0u);CALL(0xc0d9a4u);}goto accept;
top:
 P(input,DS_INPUT,0xc4b390u);word(h,s.input,4);P(input,DS_INPUT,s.input+2);compare(h,rd_u16(0xc45a92u),0x3840);
 if(rd_s16(0xc45a92u)<0x3840){CALL(0xc0d9c0u);CALL(0xc0d9c4u);CALL(0xc0d9c8u);P(input,DS_INPUT,0xc4b39au);CALL(0xc0d9d2u);CALL(0xc0d9d6u);}
 else{CALL(0xc0d9deu);CALL(0xc0d9e2u);CALL(0xc0d9e6u);}goto accept;
left:
 P(input,DS_INPUT,0xc4b390u);word(h,s.input,5);P(input,DS_INPUT,s.input+2);W(row_y,DS_ROW_Y,rd_u16(0xc458cau));AND(row_y,DS_ROW_Y,2);
 if(!(uint16_t)s.row_y){CALL(0xc0da04u);CALL(0xc0da08u);CALL(0xc0da0cu);CALL(0xc0da10u);}
 else{CALL(0xc0da18u);CALL(0xc0da1cu);CALL(0xc0da20u);CALL(0xc0da24u);P(input,DS_INPUT,0xc4b390u);word(h,s.input,3);P(input,DS_INPUT,s.input+2);P(input,DS_INPUT,s.input+8);CALL(0xc0da34u);}goto accept;
full_rectangle:
 P(input,DS_INPUT,0xc4b390u);word(h,s.input,4);P(input,DS_INPUT,s.input+2);CALL(0xc0da46u);CALL(0xc0da4au);CALL(0xc0da4eu);CALL(0xc0da52u);
 if(test_byte(h,0xc45785u))W(first,DS_FIRST,rd_u16(0xc45a94u));else W(first,DS_FIRST,rd_u16(0xc45a8au));
 compare(h,(uint16_t)s.first,0x3840);if((int16_t)s.first<=0x3840)goto reject;
accept:
 word(h,0xc456e6u,1);word(h,0xc456e8u,1);longword(h,0xc456eau,0);byte(h,0xc4589eu,1);L(first,DS_FIRST,0);observe(h,DS_UNLINK,DS_FRAME,0,0);return;
reject:
 byte(h,0xc4589eu,0);L(first,DS_FIRST,1);observe(h,DS_UNLINK,DS_FRAME,0,0);
}
