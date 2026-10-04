/* Complete C12950/C131BE readout/action owners and their original helpers.
 * The two bounded magnitude jumps reuse the enclosing C131BE LINK frame.
 * See analysis/data/control_readouts_source_scope.json for the original bytes. */
#include "control_readouts.h"
#include <stdlib.h>
#define CURRENT_READOUT_RECORD 0xc18210u
#define READOUT_RECORDS 0xc46184u
#define READOUT_RECORD_INDEX 0xc458dcu
#define READOUT_ACTION 0xc458b0u
#define READOUT_VIEW 0xc45785u
#define READOUT_VIEW_FACTOR 0xc457b5u
#define READOUT_COUNTDOWN 0xc45797u
#define READOUT_DIVISOR 0xc45b42u
static void observe(const ControlReadoutHooks *h,enum ControlReadoutPhase p,enum ControlReadoutField f,uint32_t v,uint32_t o) {if(h&&h->observe)h->observe(h->context,p,f,v,o);}
static ControlReadoutState restored(const ControlReadoutHooks *h) {if(h&&h->restored)return h->restored(h->context);abort();}
static ControlReadoutState consume(const ControlReadoutHooks *h,gaddr ret) {if(h&&h->consume)return h->consume(h->context,ret);abort();}
static uint32_t low_byte(uint32_t v,uint8_t b){return (v&0xffffff00u)|b;}
static uint32_t low_word(uint32_t v,uint16_t w){return (v&0xffff0000u)|w;}
#define B(f,id,v) do {s.f=low_byte(s.f,(uint8_t)(v));observe(h,CR_BYTE,id,s.f,0);}while(0)
#define W(f,id,v) do {s.f=low_word(s.f,(uint16_t)(v));observe(h,CR_WORD,id,s.f,0);}while(0)
#define L(f,id,v) do {s.f=(uint32_t)(v);observe(h,CR_LONG,id,s.f,0);}while(0)
#define P(f,id,v) do {s.f=(gaddr)(v);observe(h,CR_POINTER,id,s.f,0);}while(0)
#define AW(f,id,v) do {uint16_t n_=(uint16_t)(v);s.f=low_word(s.f,(uint16_t)(s.f+n_));observe(h,CR_ADD_WORD,id,n_,0);}while(0)
#define SW(f,id,v) do {uint16_t n_=(uint16_t)(v);s.f=low_word(s.f,(uint16_t)(s.f-n_));observe(h,CR_SUB_WORD,id,n_,0);}while(0)
#define AL(f,id,v) do {uint32_t n_=(uint32_t)(v);s.f+=n_;observe(h,CR_ADD_LONG,id,n_,0);}while(0)
#define SL(f,id,v) do {uint32_t n_=(uint32_t)(v);s.f-=n_;observe(h,CR_SUB_LONG,id,n_,0);}while(0)
#define SB(f,id,v) do {uint8_t n_=(uint8_t)(v);s.f=low_byte(s.f,(uint8_t)(s.f-n_));observe(h,CR_SUB_BYTE,id,n_,0);}while(0)
#define AND_B(f,id,v) do {s.f=low_byte(s.f,(uint8_t)(s.f&(v)));observe(h,CR_AND_BYTE,id,v,0);}while(0)
#define OR_B(f,id,v) do {uint8_t n_=(uint8_t)(v);s.f=low_byte(s.f,(uint8_t)(s.f|n_));observe(h,CR_OR_BYTE,id,n_,0);}while(0)
#define ASR_W(f,id,n) do {s.f=low_word(s.f,(uint16_t)((int16_t)s.f>>(n)));observe(h,CR_ASR_WORD,id,n,0);}while(0)
#define ASR_L(f,id,n) do {s.f=(uint32_t)((int32_t)s.f>>(n));observe(h,CR_ASR_LONG,id,n,0);}while(0)
#define ASL_W(f,id,n) do {s.f=low_word(s.f,(uint16_t)(s.f<<(n)));observe(h,CR_ASL_WORD,id,n,0);}while(0)
#define ASL_L(f,id,n) do {unsigned n_=(n)&63u;uint32_t old_=s.f;s.f=n_<32?old_<<n_:0;s.extend=n_&&n_<=32?(old_>>(32-n_))&1u:s.extend;observe(h,CR_ASL_LONG,id,n_,0);}while(0)
#define EXT_W(f,id) do {s.f=low_word(s.f,(uint16_t)(int16_t)(int8_t)s.f);observe(h,CR_EXT_WORD,id,0,0);}while(0)
#define EXT_L(f,id) do {s.f=(uint32_t)(int32_t)(int16_t)s.f;observe(h,CR_EXT_LONG,id,0,0);}while(0)
#define NEG_L(f,id) do {s.f=0u-s.f;observe(h,CR_NEG_LONG,id,0,0);}while(0)
#define CW(src,dst) observe(h,CR_COMPARE_WORD,CR_VALUE,(uint16_t)(dst),(uint16_t)(src))
#define CB(src,dst) observe(h,CR_COMPARE_BYTE,CR_VALUE,(uint8_t)(dst),(uint8_t)(src))
#define CL(src,dst) observe(h,CR_COMPARE_LONG,CR_VALUE,(uint32_t)(dst),(uint32_t)(src))
#define TW(v) observe(h,CR_TEST_WORD,CR_VALUE,(uint16_t)(v),0)
#define TB(v) observe(h,CR_TEST_BYTE,CR_VALUE,(uint8_t)(v),0)
#define TL(v) observe(h,CR_TEST_LONG,CR_VALUE,(uint32_t)(v),0)
#define LINK(n) do {observe(h,CR_LINK,CR_FRAME,n,0);s=restored(h);}while(0)
#define UNLINK() do {observe(h,CR_UNLINK,CR_FRAME,0,0);s=restored(h);}while(0)
#define SAVE(mask) do {observe(h,CR_SAVE_LONGS,CR_STACK,mask,0);s=restored(h);}while(0)
#define RESTORE(mask) do {observe(h,CR_RESTORE_LONGS,CR_STACK,mask,0);s=restored(h);}while(0)
#define PUSH(v) do {uint32_t pushed_=(uint32_t)(v);s.stack-=4;wr_u32(s.stack,pushed_);observe(h,CR_PUSH_LONG,CR_STACK,pushed_,0);}while(0)
#define DROP(n) do {s.stack+=n;observe(h,CR_STACK_ADD,CR_STACK,n,0);}while(0)
#define CALL(ret) do {s=consume(h,ret);}while(0)
static void byte(const ControlReadoutHooks *h,gaddr at,uint8_t v){wr_u8(at,v);observe(h,CR_STORE_BYTE,CR_VALUE,v,0);}
static void word(const ControlReadoutHooks *h,gaddr at,uint16_t v){wr_u16(at,v);observe(h,CR_STORE_WORD,CR_VALUE,v,0);}
static void longword(const ControlReadoutHooks *h,gaddr at,uint32_t v){wr_u32(at,v);observe(h,CR_STORE_LONG,CR_VALUE,v,0);}
static int test_byte(const ControlReadoutHooks *h,gaddr at){uint8_t v=rd_u8(at);TB(v);return v!=0;}
static int test_word(const ControlReadoutHooks *h,gaddr at){uint16_t v=rd_u16(at);TW(v);return v!=0;}
static int bit(const ControlReadoutHooks *h,uint32_t v,unsigned n){observe(h,CR_BIT_TEST,CR_VALUE,v,n);return (v&(1u<<n))!=0;}
static void negate_word(const ControlReadoutHooks *h,gaddr at){uint16_t v=rd_u16(at);wr_u16(at,(uint16_t)(0u-v));observe(h,CR_NEGATE_MEMORY_WORD,CR_VALUE,v,0);}
static void add_word(const ControlReadoutHooks *h,gaddr at,uint16_t n){uint16_t v=rd_u16(at);wr_u16(at,(uint16_t)(v+n));observe(h,CR_ADD_MEMORY_WORD,CR_VALUE,v,n);}
static void subtract_word(const ControlReadoutHooks *h,gaddr at,uint16_t n){uint16_t v=rd_u16(at);wr_u16(at,(uint16_t)(v-n));observe(h,CR_SUB_MEMORY_WORD,CR_VALUE,v,n);}

/* C13396 replaces the low word of its original long argument. */
void scale_control_five_eighths(ControlReadoutState s,const ControlReadoutHooks *h){
 LINK(0);W(value,CR_VALUE,rd_u16(s.frame+10));ASR_W(value,CR_VALUE,1);
 W(amount,CR_AMOUNT,rd_u16(s.frame+10));ASR_W(amount,CR_AMOUNT,3);AW(value,CR_VALUE,s.amount);
 word(h,s.frame+10,(uint16_t)s.value);EXT_L(value,CR_VALUE);UNLINK();
}

/* C133B2 keeps the original signed-word absolute/shift behavior, including
 * the -32768 input which remains negative after NEG.W. */
void read_control_record_step(ControlReadoutState s,const ControlReadoutHooks *h){
 LINK(2);P(record,CR_RECORD,rd_u32(CURRENT_READOUT_RECORD));B(value,CR_VALUE,rd_u8(s.record+0x62));AND_B(value,CR_VALUE,0xf0);CB(0x30,s.value);
 if((uint8_t)s.value==0x30)word(h,s.frame-2,16);
 else {
  P(record,CR_RECORD,rd_u32(CURRENT_READOUT_RECORD));W(value,CR_VALUE,rd_u16(s.record+0x6e));word(h,s.frame-2,(uint16_t)s.value);TW(s.value);
  if((int16_t)s.value<0)negate_word(h,s.frame-2);
  L(value,CR_VALUE,9);W(amount,CR_AMOUNT,rd_u16(s.frame-2));ASR_W(amount,CR_AMOUNT,s.value);word(h,s.frame-2,(uint16_t)s.amount);CW(63,s.amount);
  if((int16_t)s.amount>63)word(h,s.frame-2,63);
  B(value,CR_VALUE,rd_u8(READOUT_VIEW));TB(s.value);
  if(!(uint8_t)s.value){W(value,CR_VALUE,rd_u16(s.frame-2));ASR_W(value,CR_VALUE,1);word(h,s.frame-2,(uint16_t)s.value);}
  if(!test_word(h,s.frame-2))word(h,s.frame-2,1);
 }
 W(value,CR_VALUE,rd_u16(s.frame-2));EXT_L(value,CR_VALUE);UNLINK();
}

/* C13176 consumes the actual original sound-child results. */
void play_control_record_event(ControlReadoutState s,const ControlReadoutHooks *h){
 LINK(0);
 if(bit(h,rd_u8(0xc45b5au),1)){
  W(value,CR_VALUE,rd_u16(s.frame+14));EXT_L(value,CR_VALUE);PUSH(s.value);CALL(0xc13192u);DROP(4);byte(h,READOUT_COUNTDOWN,0);
 }else{
  W(value,CR_VALUE,rd_u16(s.frame+10));EXT_L(value,CR_VALUE);AL(value,CR_VALUE,22);
  W(amount,CR_AMOUNT,rd_u16(s.frame+14));EXT_L(amount,CR_AMOUNT);PUSH(s.amount);PUSH(s.value);CALL(0xc131b8u);DROP(8);
 }
 UNLINK();
}

/* C52EC8: original 32-bit restoring division; D2-D5 and their stack writes
 * remain observable. No host signed-overflow or divide-by-zero exception. */
void divide_control_long(ControlReadoutState s,const ControlReadoutHooks *h){
 SAVE(0x3c00);L(divisor_sign,CR_DIVISOR_SIGN,s.amount);
 if(!s.divisor_sign){L(value,CR_VALUE,0);goto done;}
 if((int32_t)s.divisor_sign<0)NEG_L(amount,CR_AMOUNT);
 L(dividend_sign,CR_DIVIDEND_SIGN,s.value);
 if(!s.dividend_sign){L(amount,CR_AMOUNT,0);L(value,CR_VALUE,0);goto done;}
 if((int32_t)s.dividend_sign<0)NEG_L(value,CR_VALUE);
 L(scratch,CR_SCRATCH,0);L(counter,CR_COUNTER,31);
 do {
  unsigned next;
  ASL_L(value,CR_VALUE,1);next=s.scratch>>31;s.scratch=(s.scratch<<1)|s.extend;s.extend=next;observe(h,CR_ROXL_LONG,CR_SCRATCH,1,0);
  CL(s.amount,s.scratch);
  if(s.scratch>=s.amount){SL(scratch,CR_SCRATCH,s.amount);AL(value,CR_VALUE,1);}
  s.counter=low_word(s.counter,(uint16_t)(s.counter-1));observe(h,CR_DBRA,CR_COUNTER,0,0);
 }while((uint16_t)s.counter!=0xffffu);
 L(amount,CR_AMOUNT,s.scratch);s.divisor_sign^=s.dividend_sign;observe(h,CR_XOR_LONG,CR_DIVISOR_SIGN,s.dividend_sign,0);
 if((int32_t)s.divisor_sign<0)NEG_L(value,CR_VALUE);
 s.dividend_sign^=s.amount;observe(h,CR_XOR_LONG,CR_DIVIDEND_SIGN,s.amount,0);
 if((int32_t)s.dividend_sign<0)NEG_L(amount,CR_AMOUNT);
done:RESTORE(0x003c);
}

/* C131BE, including every arm behind the two original guarded jumps. */
void scale_control_record_magnitude(ControlReadoutState s,const ControlReadoutHooks *h){
 LINK(6);
 if(!test_word(h,s.frame+10)){
  P(record,CR_RECORD,rd_u32(CURRENT_READOUT_RECORD));W(value,CR_VALUE,rd_u16(s.record+2));
  if(bit(h,s.value,7)&&test_word(h,s.record+0x6e))word(h,s.frame+10,12);
  goto result;
 }
 if(test_byte(h,READOUT_COUNTDOWN))word(h,s.frame+10,63);
 else{
  W(value,CR_VALUE,rd_u16(s.frame+10));CW(8,s.value);
  if((int16_t)s.value>8){L(amount,CR_AMOUNT,120);SW(amount,CR_AMOUNT,s.value);ASR_W(amount,CR_AMOUNT,2);L(value,CR_VALUE,63);SW(value,CR_VALUE,s.amount);word(h,s.frame+10,(uint16_t)s.value);}
  else{L(value,CR_VALUE,120);SW(value,CR_VALUE,rd_u16(s.frame+10));ASR_W(value,CR_VALUE,1);L(amount,CR_AMOUNT,60);SW(amount,CR_AMOUNT,s.value);word(h,s.frame+10,(uint16_t)s.amount);}
 }
 if(!test_byte(h,READOUT_VIEW)){
  B(value,CR_VALUE,rd_u8(0xc45889u));CB(5,s.value);
  if((int8_t)s.value>5){W(value,CR_VALUE,rd_u16(s.frame+10));ASR_W(value,CR_VALUE,1);word(h,s.frame+10,(uint16_t)s.value);}
  goto result;
 }
 if(!test_word(h,s.frame+10))goto result;
 if(test_byte(h,READOUT_VIEW_FACTOR)){
  B(value,CR_VALUE,rd_u8(0xc458b2u));EXT_W(value,CR_VALUE);word(h,s.frame-2,(uint16_t)s.value);CW(4,s.value);
  if((int16_t)s.value>4){L(amount,CR_AMOUNT,8);SW(amount,CR_AMOUNT,s.value);word(h,s.frame-2,(uint16_t)s.amount);TW(s.amount);if((int16_t)s.amount<0)word(h,s.frame-2,0);}
  W(value,CR_VALUE,rd_u16(s.frame-2));EXT_L(value,CR_VALUE);CL(4,s.value);
  if(s.value<4){
   unsigned choice=s.value;ASL_L(value,CR_VALUE,1);
   switch(choice){
   case 0:W(value,CR_VALUE,rd_u16(s.frame+10));EXT_L(value,CR_VALUE);PUSH(s.value);CALL(0xc1326au);DROP(4);word(h,s.frame+10,(uint16_t)s.value);break;
   case 1:W(value,CR_VALUE,rd_u16(s.frame+10));EXT_L(value,CR_VALUE);PUSH(s.value);CALL(0xc1327eu);DROP(4);word(h,s.frame+10,(uint16_t)s.value);break;
   case 2:W(value,CR_VALUE,rd_u16(s.frame+10));ASR_W(value,CR_VALUE,2);subtract_word(h,s.frame+10,(uint16_t)s.value);break;
   case 3:W(value,CR_VALUE,rd_u16(s.frame+10));ASR_W(value,CR_VALUE,3);subtract_word(h,s.frame+10,(uint16_t)s.value);break;
   }
  }
  B(value,CR_VALUE,rd_u8(READOUT_VIEW));EXT_W(value,CR_VALUE);EXT_L(value,CR_VALUE);SL(value,CR_VALUE,1);
  if((int32_t)s.value>=0){CL(4,s.value);if((int32_t)s.value<4){
   unsigned choice=s.value;ASL_L(value,CR_VALUE,1);
   switch(choice){
   case 0:W(value,CR_VALUE,rd_u16(s.frame+10));ASR_W(value,CR_VALUE,3);subtract_word(h,s.frame+10,(uint16_t)s.value);break;
   case 1:W(value,CR_VALUE,rd_u16(s.frame+10));ASR_W(value,CR_VALUE,2);subtract_word(h,s.frame+10,(uint16_t)s.value);break;
   case 2:W(value,CR_VALUE,rd_u16(s.frame+10));EXT_L(value,CR_VALUE);PUSH(s.value);CALL(0xc132eau);DROP(4);word(h,s.frame+10,(uint16_t)s.value);break;
   case 3:W(value,CR_VALUE,rd_u16(s.frame+10));ASR_W(value,CR_VALUE,1);word(h,s.frame+10,(uint16_t)s.value);break;
   }
  }}
 }else if(test_word(h,READOUT_DIVISOR)){
  W(value,CR_VALUE,rd_u16(s.frame+10));EXT_L(value,CR_VALUE);ASL_L(value,CR_VALUE,5);
  W(amount,CR_AMOUNT,rd_u16(READOUT_DIVISOR));EXT_L(amount,CR_AMOUNT);longword(h,s.frame-6,s.value);CALL(0xc13320u);word(h,s.frame+10,(uint16_t)s.value);CW(84,s.value);
  if((int16_t)s.value>84)word(h,s.frame+10,84);
 }
 P(record,CR_RECORD,rd_u32(CURRENT_READOUT_RECORD));W(value,CR_VALUE,rd_u16(s.record+2));
 if(!bit(h,s.value,3)){W(value,CR_VALUE,rd_u16(s.frame+10));ASR_W(value,CR_VALUE,2);subtract_word(h,s.frame+10,(uint16_t)s.value);}
 TW(rd_u16(s.frame+10));if(rd_s16(s.frame+10)<=0)word(h,s.frame+10,1);
result:W(value,CR_VALUE,rd_u16(s.frame+10));EXT_L(value,CR_VALUE);UNLINK();
}

/* C12950's pending event priority and nine original voice arguments. Each
 * repeated push retains the original working value and stack byte order. */
static ControlReadoutState pending_readout_sound(ControlReadoutState s,const ControlReadoutHooks *h){
 if(bit(h,rd_u8(0xc45b57u),6)){
  L(value,CR_VALUE,4);PUSH(s.value);PUSH(s.value);L(value,CR_VALUE,3);PUSH(s.value);L(value,CR_VALUE,5);PUSH(s.value);
  L(value,CR_VALUE,16);PUSH(s.value);PUSH(200);L(value,CR_VALUE,8);PUSH(s.value);L(value,CR_VALUE,16);PUSH(s.value);PUSH(160);CALL(0xc12a56u);DROP(36);goto done;
 }
 P(record,CR_RECORD,rd_u32(CURRENT_READOUT_RECORD));B(value,CR_VALUE,rd_u8(s.record+0x7c));AND_B(value,CR_VALUE,15);TB(s.value);
 if((uint8_t)s.value)goto done;
 if(bit(h,rd_u8(0xc45b57u),7)){
  L(value,CR_VALUE,4);PUSH(s.value);L(value,CR_VALUE,1);PUSH(s.value);L(value,CR_VALUE,3);PUSH(s.value);L(value,CR_VALUE,5);PUSH(s.value);
  L(value,CR_VALUE,10);PUSH(s.value);PUSH(200);L(value,CR_VALUE,8);PUSH(s.value);L(value,CR_VALUE,10);PUSH(s.value);PUSH(324);CALL(0xc12aaau);DROP(36);
 }else if(bit(h,rd_u8(0xc45b57u),0)){
  L(value,CR_VALUE,4);PUSH(s.value);L(value,CR_VALUE,3);PUSH(s.value);PUSH(s.value);L(value,CR_VALUE,5);PUSH(s.value);
  L(value,CR_VALUE,16);PUSH(s.value);PUSH(180);L(value,CR_VALUE,8);PUSH(s.value);L(value,CR_VALUE,16);PUSH(s.value);L(value,CR_VALUE,124);PUSH(s.value);
  CALL(0xc12ae6u);DROP(36);byte(h,0xc4588au,3);
 }else if(bit(h,rd_u8(0xc45b57u),2)){
  L(value,CR_VALUE,1);PUSH(s.value);PUSH(0);PUSH(s.value);L(value,CR_VALUE,5);PUSH(s.value);L(value,CR_VALUE,3);PUSH(s.value);
  L(value,CR_VALUE,229);PUSH(s.value);L(value,CR_VALUE,5);PUSH(s.value);L(value,CR_VALUE,3);PUSH(s.value);PUSH(229);CALL(0xc12b2cu);DROP(36);
 }else if(bit(h,rd_u8(0xc45b56u),0)){
  L(value,CR_VALUE,1);PUSH(s.value);L(value,CR_VALUE,0);PUSH(s.value);L(value,CR_VALUE,1);PUSH(s.value);L(value,CR_VALUE,3);PUSH(s.value);
  PUSH(0);PUSH(200);L(value,CR_VALUE,9);PUSH(s.value);L(value,CR_VALUE,4);PUSH(s.value);PUSH(192);CALL(0xc12b6au);DROP(36);
 }else if(bit(h,rd_u8(0xc45b57u),4)){
  L(value,CR_VALUE,5);PUSH(s.value);L(value,CR_VALUE,1);PUSH(s.value);L(value,CR_VALUE,3);PUSH(s.value);L(value,CR_VALUE,5);PUSH(s.value);
  L(value,CR_VALUE,15);PUSH(s.value);L(value,CR_VALUE,500);PUSH(s.value);L(value,CR_VALUE,5);PUSH(s.value);L(value,CR_VALUE,15);PUSH(s.value);PUSH(500);CALL(0xc12bacu);DROP(36);
 }else if(bit(h,rd_u8(0xc45b57u),1)){
  L(value,CR_VALUE,5);PUSH(s.value);L(value,CR_VALUE,3);PUSH(s.value);PUSH(s.value);L(value,CR_VALUE,1);PUSH(s.value);PUSH(0);PUSH(130);
  L(value,CR_VALUE,5);PUSH(s.value);L(value,CR_VALUE,10);PUSH(s.value);PUSH(500);CALL(0xc12be8u);DROP(36);
 }else if(bit(h,rd_u8(0xc45b57u),5)){
  L(value,CR_VALUE,5);PUSH(s.value);L(value,CR_VALUE,2);PUSH(s.value);L(value,CR_VALUE,3);PUSH(s.value);L(value,CR_VALUE,1);PUSH(s.value);PUSH(0);PUSH(330);
  L(value,CR_VALUE,5);PUSH(s.value);L(value,CR_VALUE,8);PUSH(s.value);PUSH(400);CALL(0xc12c26u);DROP(36);
 }else if(bit(h,rd_u8(0xc45b56u),3)){
  L(value,CR_VALUE,5);PUSH(s.value);L(value,CR_VALUE,1);PUSH(s.value);L(value,CR_VALUE,3);PUSH(s.value);L(value,CR_VALUE,1);PUSH(s.value);PUSH(0);PUSH(130);
  L(value,CR_VALUE,5);PUSH(s.value);L(value,CR_VALUE,10);PUSH(s.value);PUSH(500);CALL(0xc12c62u);DROP(36);
 }else if(bit(h,rd_u8(0xc45b57u),3)){
  L(value,CR_VALUE,3);PUSH(s.value);CALL(0xc12c7cu);DROP(4);
 }
done:longword(h,0xc45b54u,0);return s;
}

/* C12950 is the complete enclosing control-record readout/action selector.
 * Sound children own their effects; subsequent decisions reread real RAM. */
void update_control_record_readouts(ControlReadoutState s,const ControlReadoutHooks *h){
 LINK(30);SAVE(0x2000);
 W(value,CR_VALUE,rd_u16(READOUT_RECORD_INDEX));L(amount,CR_AMOUNT,9);EXT_L(value,CR_VALUE);ASL_L(value,CR_VALUE,s.amount);
 P(record,CR_RECORD,s.value);P(record,CR_RECORD,s.record+READOUT_RECORDS);L(value,CR_VALUE,s.record);longword(h,CURRENT_READOUT_RECORD,s.value);
 byte(h,s.frame-17,rd_u8(READOUT_ACTION));P(record,CR_RECORD,s.value);P(record,CR_RECORD,s.record+2);B(value,CR_VALUE,rd_u8(0xc458aeu));longword(h,s.frame-16,s.record);TB(s.value);
 if((uint8_t)s.value)goto done;
 B(value,CR_VALUE,rd_u8(0xc457adu));B(amount,CR_AMOUNT,rd_u8(0xc457aeu));OR_B(value,CR_VALUE,s.amount);TB(s.value);if((uint8_t)s.value)goto done;
 if(!test_byte(h,0xc45795u))goto done;
 B(value,CR_VALUE,rd_u8(0xc45885u));TB(s.value);
 if((int8_t)s.value<0){
  if(test_byte(h,READOUT_VIEW)){
   if(test_byte(h,READOUT_VIEW_FACTOR)){L(value,CR_VALUE,5);PUSH(s.value);CALL(0xc129d2u);DROP(4);}
  }else{L(value,CR_VALUE,10);PUSH(s.value);CALL(0xc129e0u);DROP(4);}
  B(value,CR_VALUE,rd_u8(0xc45885u));AND_B(value,CR_VALUE,0x7f);byte(h,0xc45885u,(uint8_t)s.value);
 }else{
  B(value,CR_VALUE,rd_u8(0xc45885u));SB(value,CR_VALUE,1);
  if(!(uint8_t)s.value){CALL(0xc12a04u);byte(h,0xc45885u,0);}
 }
 TL(rd_u32(0xc45b54u));
 if(rd_u32(0xc45b54u)){
  B(value,CR_VALUE,rd_u8(0xc4588au));TB(s.value);
  if((int8_t)s.value<=0)s=pending_readout_sound(s,h);
 }
 if(test_byte(h,0xc457b8u)){
  B(value,CR_VALUE,rd_u8(0xc457b8u));SB(value,CR_VALUE,1);byte(h,0xc457b8u,(uint8_t)s.value);TB(s.value);
  if((int8_t)s.value<=0){
   if(test_byte(h,READOUT_VIEW)){
    B(value,CR_VALUE,rd_u8(0xc4586au));EXT_W(value,CR_VALUE);EXT_L(value,CR_VALUE);
    B(amount,CR_AMOUNT,rd_u8(0xc45869u));EXT_W(amount,CR_AMOUNT);EXT_L(amount,CR_AMOUNT);PUSH(s.amount);PUSH(s.value);CALL(0xc12cc4u);DROP(8);
   }else{
    B(value,CR_VALUE,rd_u8(0xc4586au));EXT_W(value,CR_VALUE);EXT_L(value,CR_VALUE);ASR_L(value,CR_VALUE,1);
    B(amount,CR_AMOUNT,rd_u8(0xc45869u));EXT_W(amount,CR_AMOUNT);EXT_L(amount,CR_AMOUNT);PUSH(s.amount);PUSH(s.value);CALL(0xc12ce8u);DROP(8);
   }
   byte(h,0xc457b8u,0);
  }
 }
 if(!test_byte(h,s.frame-17)){
  B(value,CR_VALUE,rd_u8(0xc457c1u));AND_B(value,CR_VALUE,3);TB(s.value);if((uint8_t)s.value)goto countdown;
 }
 P(record,CR_RECORD,rd_u32(CURRENT_READOUT_RECORD));B(value,CR_VALUE,rd_u8(s.record+0x62));AND_B(value,CR_VALUE,0xf0);CB(0x30,s.value);
 if((uint8_t)s.value==0x30){CALL(0xc13128u);EXT_L(value,CR_VALUE);PUSH(s.value);CALL(0xc13132u);DROP(4);goto consumed_action;}
 B(value,CR_VALUE,rd_u8(s.record+0x2b));EXT_W(value,CR_VALUE);word(h,s.frame-2,(uint16_t)s.value);TW(s.value);
 if((int16_t)s.value<0)negate_word(h,s.frame-2);
 word(h,s.frame-6,61);W(value,CR_VALUE,rd_u16(s.frame-2));EXT_L(value,CR_VALUE);PUSH(s.value);CALL(0xc12d40u);DROP(4);word(h,s.frame-4,(uint16_t)s.value);TW(s.value);
 if(!(uint16_t)s.value){CALL(0xc13116u);EXT_L(value,CR_VALUE);PUSH(s.value);CALL(0xc13120u);DROP(4);goto consumed_action;}
 P(record,CR_RECORD,rd_u32(CURRENT_READOUT_RECORD));W(value,CR_VALUE,rd_u16(s.record+0x5a));ASR_W(value,CR_VALUE,4);word(h,s.frame-12,(uint16_t)s.value);TW(s.value);
 if((int16_t)s.value<0)negate_word(h,s.frame-12);
 P(record,CR_RECORD,rd_u32(CURRENT_READOUT_RECORD));W(value,CR_VALUE,rd_u16(s.record+0x56));ASR_W(value,CR_VALUE,4);word(h,s.frame-10,(uint16_t)s.value);TW(s.value);
 if((int16_t)s.value<0)negate_word(h,s.frame-10);
 W(value,CR_VALUE,rd_u16(s.frame-12));CW(rd_u16(s.frame-10),s.value);
 if((int16_t)s.value<rd_s16(s.frame-10)){
  P(record,CR_RECORD,rd_u32(CURRENT_READOUT_RECORD));W(amount,CR_AMOUNT,rd_u16(s.record+0x56));ASR_W(amount,CR_AMOUNT,4);add_word(h,s.frame-12,(uint16_t)s.amount);
 }
 W(value,CR_VALUE,rd_u16(s.frame-2));CW(120,s.value);
 if((int16_t)s.value>=120){W(value,CR_VALUE,rd_u16(s.frame-12));subtract_word(h,s.frame-2,(uint16_t)s.value);}
 else{W(value,CR_VALUE,rd_u16(s.frame-2));W(amount,CR_AMOUNT,rd_u16(s.frame-12));AW(amount,CR_AMOUNT,s.value);word(h,s.frame-2,(uint16_t)s.amount);CW(120,s.amount);if((int16_t)s.amount>120)word(h,s.frame-2,120);}
 if(test_byte(h,READOUT_VIEW)){
  B(value,CR_VALUE,rd_u8(0xc458aeu));TB(s.value);if((uint8_t)s.value)goto outside_view;
  W(value,CR_VALUE,rd_u16(s.frame-4));ASR_W(value,CR_VALUE,2);subtract_word(h,s.frame-4,(uint16_t)s.value);
  B(value,CR_VALUE,rd_u8(READOUT_VIEW_FACTOR));TB(s.value);
  if(!(uint8_t)s.value&&test_word(h,READOUT_DIVISOR)){
   W(value,CR_VALUE,rd_u16(s.frame-6));EXT_L(value,CR_VALUE);ASL_L(value,CR_VALUE,5);
   W(amount,CR_AMOUNT,rd_u16(READOUT_DIVISOR));EXT_L(amount,CR_AMOUNT);longword(h,s.frame-22,s.value);CALL(0xc12e10u);word(h,s.frame-6,(uint16_t)s.value);CW(63,s.value);
   if((int16_t)s.value>63)word(h,s.frame-6,63);
  }
  W(value,CR_VALUE,rd_u16(s.frame-2));ASL_W(value,CR_VALUE,1);W(amount,CR_AMOUNT,800);SW(amount,CR_AMOUNT,s.value);
  W(value,CR_VALUE,rd_u16(s.frame-2));ASR_W(value,CR_VALUE,1);SW(amount,CR_AMOUNT,s.value);
  W(value,CR_VALUE,rd_u16(s.frame-2));ASR_W(value,CR_VALUE,2);SW(amount,CR_AMOUNT,s.value);word(h,s.frame-10,(uint16_t)s.amount);
  CB(0xfe,rd_u8(s.frame-17));
  if(rd_u8(s.frame-17)==0xfe){EXT_L(amount,CR_AMOUNT);W(value,CR_VALUE,rd_u16(s.frame-4));EXT_L(value,CR_VALUE);PUSH(s.value);PUSH(s.amount);CALL(0xc12e5au);DROP(8);}
  else{
   CB(0xfd,rd_u8(s.frame-17));
   if(rd_u8(s.frame-17)==0xfd){W(value,CR_VALUE,rd_u16(s.frame-10));EXT_L(value,CR_VALUE);AL(value,CR_VALUE,32);W(amount,CR_AMOUNT,rd_u16(s.frame-6));EXT_L(amount,CR_AMOUNT);PUSH(s.amount);PUSH(s.value);CALL(0xc12e84u);DROP(8);}
   else{
    CB(0xfc,rd_u8(s.frame-17));
    if(rd_u8(s.frame-17)==0xfc){W(value,CR_VALUE,rd_u16(s.frame-10));EXT_L(value,CR_VALUE);AL(value,CR_VALUE,32);W(amount,CR_AMOUNT,rd_u16(s.frame-4));EXT_L(amount,CR_AMOUNT);PUSH(s.amount);PUSH(s.value);CALL(0xc12eaeu);DROP(8);}
    else{
     CB(0xfb,rd_u8(s.frame-17));
     if(rd_u8(s.frame-17)==0xfb){W(value,CR_VALUE,rd_u16(s.frame-10));EXT_L(value,CR_VALUE);W(amount,CR_AMOUNT,rd_u16(s.frame-6));EXT_L(amount,CR_AMOUNT);PUSH(s.amount);PUSH(s.value);CALL(0xc12ed0u);DROP(8);}
     else{
      CB(0xfa,rd_u8(s.frame-17));
      if(rd_u8(s.frame-17)==0xfa){
       W(value,CR_VALUE,rd_u16(s.frame-6));ASL_W(value,CR_VALUE,1);word(h,s.frame-8,(uint16_t)s.value);CW(63,s.value);if((int16_t)s.value>63)word(h,s.frame-8,63);
       W(value,CR_VALUE,rd_u16(s.frame-10));EXT_L(value,CR_VALUE);AL(value,CR_VALUE,22);W(amount,CR_AMOUNT,rd_u16(s.frame-8));EXT_L(amount,CR_AMOUNT);PUSH(s.amount);PUSH(s.value);CALL(0xc12f10u);DROP(8);
      }else{W(value,CR_VALUE,rd_u16(s.frame-10));EXT_L(value,CR_VALUE);W(amount,CR_AMOUNT,rd_u16(s.frame-4));EXT_L(amount,CR_AMOUNT);L(scratch,CR_SCRATCH,32);PUSH(s.scratch);PUSH(s.amount);PUSH(s.value);CALL(0xc12f30u);DROP(12);}
     }
    }
   }
  }
  goto consumed_action;
 }
outside_view:
 CW(40,rd_u16(s.frame-4));if(rd_s16(s.frame-4)<40)word(h,s.frame-4,40);
 W(value,CR_VALUE,rd_u16(s.frame-2));ASL_W(value,CR_VALUE,1);W(amount,CR_AMOUNT,784);SW(amount,CR_AMOUNT,s.value);
 W(value,CR_VALUE,rd_u16(s.frame-2));ASR_W(value,CR_VALUE,2);SW(amount,CR_AMOUNT,s.value);word(h,s.frame-10,(uint16_t)s.amount);
 CB(0xfe,rd_u8(s.frame-17));
 if(rd_u8(s.frame-17)==0xfe){
  P(record,CR_RECORD,rd_u32(s.frame-16));W(value,CR_VALUE,rd_u16(s.record));
  if(!bit(h,s.value,3)){EXT_L(amount,CR_AMOUNT);W(value,CR_VALUE,rd_u16(s.frame-4));EXT_L(value,CR_VALUE);PUSH(s.value);PUSH(s.amount);CALL(0xc12f84u);DROP(8);}
  else{W(value,CR_VALUE,rd_u16(s.frame-10));EXT_L(value,CR_VALUE);W(amount,CR_AMOUNT,rd_u16(s.frame-4));EXT_L(amount,CR_AMOUNT);ASR_L(amount,CR_AMOUNT,2);PUSH(s.amount);PUSH(s.value);CALL(0xc12fa2u);DROP(8);}
 }else{
  CB(0xfd,rd_u8(s.frame-17));
  if(rd_u8(s.frame-17)==0xfd){W(value,CR_VALUE,rd_u16(s.frame-10));EXT_L(value,CR_VALUE);AL(value,CR_VALUE,35);W(amount,CR_AMOUNT,rd_u16(s.frame-6));EXT_L(amount,CR_AMOUNT);ASR_L(amount,CR_AMOUNT,1);PUSH(s.amount);PUSH(s.value);CALL(0xc12fceu);DROP(8);}
  else{
   CB(0xfc,rd_u8(s.frame-17));
   if(rd_u8(s.frame-17)==0xfc){W(value,CR_VALUE,rd_u16(s.frame-10));EXT_L(value,CR_VALUE);AL(value,CR_VALUE,32);W(amount,CR_AMOUNT,rd_u16(s.frame-4));EXT_L(amount,CR_AMOUNT);PUSH(s.amount);PUSH(s.value);CALL(0xc12ff8u);DROP(8);}
   else{
    CB(0xfb,rd_u8(s.frame-17));
    if(rd_u8(s.frame-17)==0xfb){W(value,CR_VALUE,rd_u16(s.frame-10));EXT_L(value,CR_VALUE);W(amount,CR_AMOUNT,rd_u16(s.frame-6));EXT_L(amount,CR_AMOUNT);PUSH(s.amount);PUSH(s.value);CALL(0xc1301au);DROP(8);}
    else{
     CB(0xfa,rd_u8(s.frame-17));
     if(rd_u8(s.frame-17)==0xfa){
      W(value,CR_VALUE,rd_u16(s.frame-6));ASL_W(value,CR_VALUE,1);word(h,s.frame-8,(uint16_t)s.value);CW(40,s.value);if((int16_t)s.value>40)word(h,s.frame-8,40);
      W(value,CR_VALUE,rd_u16(s.frame-10));EXT_L(value,CR_VALUE);AL(value,CR_VALUE,22);W(amount,CR_AMOUNT,rd_u16(s.frame-8));EXT_L(amount,CR_AMOUNT);PUSH(s.amount);PUSH(s.value);CALL(0xc1305au);DROP(8);
     }else{
      if(test_byte(h,READOUT_COUNTDOWN)){
       if(bit(h,rd_u8(0xc45b5bu),0)){
        W(value,CR_VALUE,rd_u16(s.frame-10));EXT_L(value,CR_VALUE);L(amount,CR_AMOUNT,32);PUSH(s.amount);L(amount,CR_AMOUNT,10);PUSH(s.amount);PUSH(s.value);CALL(0xc13088u);DROP(12);
       }else{
        W(value,CR_VALUE,rd_u16(s.frame-10));EXT_L(value,CR_VALUE);L(amount,CR_AMOUNT,32);PUSH(s.amount);L(amount,CR_AMOUNT,40);PUSH(s.amount);PUSH(s.value);CALL(0xc130a6u);DROP(12);
       }
      }else{
       int scaled=0;P(record,CR_RECORD,rd_u32(s.frame-16));W(value,CR_VALUE,rd_u16(s.record));
       if(bit(h,s.value,3))scaled=bit(h,rd_u8(0xc45b5bu),0);
       W(value,CR_VALUE,rd_u16(s.frame-10));EXT_L(value,CR_VALUE);W(amount,CR_AMOUNT,rd_u16(s.frame-4));EXT_L(amount,CR_AMOUNT);
       if(scaled){
        /* Original C130F0 writes into the caller-visible stack slot before
         * the three sound arguments are pushed. */
        longword(h,s.stack+8,s.amount);ASR_L(amount,CR_AMOUNT,2);L(scratch,CR_SCRATCH,rd_u32(s.stack+8));ASR_L(scratch,CR_SCRATCH,3);AL(amount,CR_AMOUNT,s.scratch);
        L(scratch,CR_SCRATCH,32);PUSH(s.scratch);PUSH(s.amount);PUSH(s.value);CALL(0xc1310cu);DROP(12);
       }else{L(scratch,CR_SCRATCH,32);PUSH(s.scratch);PUSH(s.amount);PUSH(s.value);CALL(0xc130deu);DROP(12);}
      }
     }
    }
   }
  }
 }
consumed_action:byte(h,READOUT_ACTION,0);
countdown:
 B(value,CR_VALUE,rd_u8(READOUT_COUNTDOWN));TB(s.value);
 if((int8_t)s.value>0){
  SB(value,CR_VALUE,1);byte(h,READOUT_COUNTDOWN,(uint8_t)s.value);TB(s.value);
  if(!(uint8_t)s.value){B(value,CR_VALUE,rd_u8(READOUT_VIEW));TB(s.value);
   if(!(uint8_t)s.value){P(record,CR_RECORD,rd_u32(s.frame-16));W(value,CR_VALUE,rd_u16(s.record));if(!bit(h,s.value,3))byte(h,READOUT_ACTION,0xfe);}
  }
 }
done:RESTORE(0x0004);UNLINK();
}
