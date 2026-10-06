/* Original C2CA26/C2CA92/C2CAA0 and C2CB82/C2CB86/C2CBBC.
 * Roll and pitch selection retain original signed word comparisons. */
#include "record_steering.h"
static void observe(const RecordSteeringHooks *h,enum SteeringPhase p,enum SteeringField f,uint32_t v,uint32_t other) {
 if(h && h->observe) h->observe(h->context,p,f,v,other);
}
static uint32_t byte_part(uint32_t old,uint32_t v){return (old&0xffffff00u)|(uint8_t)v;}
static uint32_t word_part(uint32_t old,uint32_t v){return (old&0xffff0000u)|(uint16_t)v;}
#define B(field,id,v) do{w.field=byte_part(w.field,(v));observe(h,ST_BYTE,id,w.field,0);}while(0)
#define W(field,id,v) do{w.field=word_part(w.field,(v));observe(h,ST_WORD,id,w.field,0);}while(0)
#define L(field,id,v) do{w.field=(uint32_t)(v);observe(h,ST_LONG,id,w.field,0);}while(0)
#define AND(field,id,v) do{w.field=byte_part(w.field,w.field&(v));observe(h,ST_AND_BYTE,id,(v),0);}while(0)
#define OR(field,id,v) do{w.field=byte_part(w.field,w.field|(v));observe(h,ST_OR_BYTE,id,(v),0);}while(0)
#define TW(v) observe(h,ST_TEST_WORD,ST_TURN,(uint16_t)(v),0)
#define CW(v,n) observe(h,ST_COMPARE_WORD,ST_TURN,(uint16_t)(v),(uint16_t)(n))
static RecordSteeringState publish_roll(RecordSteeringState w,const RecordSteeringHooks *h){
 B(controls,ST_CONTROLS,rd_u8(w.record+0x65));AND(controls,ST_CONTROLS,3);OR(controls,ST_CONTROLS,w.selection);
 wr_u8(w.record+0x65,(uint8_t)w.controls);observe(h,ST_STORE_BYTE,ST_CONTROLS,w.controls,0);
 return w;
}
RecordSteeringState select_record_neutral(RecordSteeringState w,const RecordSteeringHooks *h){
 L(selection,ST_SELECTION,0);return publish_roll(w,h);
}
RecordSteeringState select_record_roll(RecordSteeringState w,const RecordSteeringHooks *h){
 TW(w.turn);L(selection,ST_SELECTION,(int16_t)w.turn==0?0:(int16_t)w.turn<0?4:8);return publish_roll(w,h);
}
RecordSteeringState select_record_turn(RecordSteeringState w,const RecordSteeringHooks *h){
 B(selection,ST_SELECTION,rd_u8(w.record+0x64));AND(selection,ST_SELECTION,0x60);
 observe(h,ST_COMPARE_BYTE,ST_SELECTION,(uint8_t)w.selection,0x60);
 if((uint8_t)w.selection==0x60){
  TW(w.turn);if(!(uint16_t)w.turn){return select_record_neutral(w,h);}
  CW(w.turn,rd_u16(w.record+0x58));
  if((int16_t)w.turn<0){
   if((int16_t)w.turn>=rd_s16(w.record+0x58)){return select_record_neutral(w,h);}
   B(selection,ST_SELECTION,0x40);
  }else{
   if((int16_t)w.turn<=rd_s16(w.record+0x58)){return select_record_neutral(w,h);}
   B(selection,ST_SELECTION,0x80);
  }
  W(turn,ST_TURN,rd_u16(w.record+0x6a));CW(w.turn,0x3840);
  if((int16_t)w.turn>0x3840){CW(w.turn,0x7030);if((int16_t)w.turn<=0x7030)OR(selection,ST_SELECTION,8);}
  else{CW(w.turn,0x50);if((int16_t)w.turn>0x50)OR(selection,ST_SELECTION,4);}
  return publish_roll(w,h);
 }
 {uint8_t flags=rd_u8(w.record+0x64);observe(h,ST_BIT_TEST,ST_SELECTION,flags,7);
  if(flags&0x80){
   W(selection,ST_SELECTION,rd_u16(w.record+0x6a));CW(w.selection,0x3840);
   if((int16_t)w.selection>0x3840){CW(w.selection,0x6ef0);if((int16_t)w.selection<=0x6ef0){L(selection,ST_SELECTION,8);return publish_roll(w,h);}}
   else{CW(w.selection,0x190);if((int16_t)w.selection>=0x190){L(selection,ST_SELECTION,4);return publish_roll(w,h);}}
  }
 }
 return select_record_roll(w,h);
}
static RecordSteeringState pitch_selection(RecordSteeringState w,const RecordSteeringHooks *h){
 TW(w.turn);CW(w.turn,rd_u16(w.record+0x56));
 if((int16_t)w.turn<0)L(selection,ST_SELECTION,(int16_t)w.turn<rd_s16(w.record+0x56)?0x20:0);
 else L(selection,ST_SELECTION,(int16_t)w.turn>rd_s16(w.record+0x56)?0x10:0);
 B(controls,ST_CONTROLS,rd_u8(w.record+0x65));TW(w.controls);
 AND(controls,ST_CONTROLS,(int16_t)w.controls<0?0xcf:3);OR(controls,ST_CONTROLS,w.selection);
 wr_u8(w.record+0x65,(uint8_t)w.controls);observe(h,ST_STORE_BYTE,ST_CONTROLS,w.controls,0);
 return w;
}
RecordSteeringState select_record_pitch(RecordSteeringState w,const RecordSteeringHooks *h){
 L(controls,ST_CONTROLS,0);return pitch_selection(w,h);
}
RecordSteeringState select_record_pitch_preserving_controls(RecordSteeringState w,const RecordSteeringHooks *h){
 L(controls,ST_CONTROLS,0xffffffffu);return pitch_selection(w,h);
}
RecordSteeringState select_record_pitch_branch(RecordSteeringState w,const RecordSteeringHooks *h){
 return select_record_pitch(w,h);
}
