/* Complete flight-record actions: C230E8..C23A24 and C257EC..C25862.
 * Shared exits are named state operations; all consumers remain original. */
#include "flight_record_actions.h"
#include <stdlib.h>
static void observe(const FlightActionHooks *h,enum FlightActionPhase p,enum FlightActionValue field,uint32_t v,uint32_t operand) {
    if(h && h->observe) h->observe(h->context,p,field,v,operand);
}
static FlightActionState consume(const FlightActionHooks *h,enum FlightActionChild child) {
    if(h && h->consume) return h->consume(h->context,child);
    abort();
}
static uint32_t word_half(uint32_t old,uint16_t v) { return (old&0xffff0000u)|v; }
static uint32_t byte_half(uint32_t old,uint8_t v) { return (old&0xffffff00u)|v; }
static uint32_t extended_word(uint32_t v) { return (uint32_t)(int32_t)(int16_t)v; }
static uint32_t asr(uint32_t v,unsigned count) { count&=63u; return (uint32_t)((int32_t)v>>(count<32?count:31)); }
static uint32_t swapped(uint32_t v) { return (v<<16)|(v>>16); }
static void byte(const FlightActionHooks *h,gaddr a,uint8_t v) { wr_u8(a,v); observe(h,FA_STORE_BYTE,FA_PRIMARY,v,0); }
static void word(const FlightActionHooks *h,gaddr a,uint16_t v) { wr_u16(a,v); observe(h,FA_STORE_WORD,FA_PRIMARY,v,0); }
static void longword(const FlightActionHooks *h,gaddr a,uint32_t v) { wr_u32(a,v); observe(h,FA_STORE_LONG,FA_PRIMARY,v,0); }
static int test_byte(const FlightActionHooks *h,gaddr a) { uint8_t v=rd_u8(a); observe(h,FA_TEST_BYTE,FA_PRIMARY,v,0); return v!=0; }
static int test_word(const FlightActionHooks *h,gaddr a) { uint16_t v=rd_u16(a); observe(h,FA_TEST_WORD,FA_PRIMARY,v,0); return v!=0; }
static uint8_t bit_change(const FlightActionHooks *h,gaddr a,unsigned bit,int set) {
    uint8_t old=rd_u8(a); wr_u8(a,set?(uint8_t)(old|(1u<<bit)):(uint8_t)(old&~(1u<<bit)));
    observe(h,set?FA_MEMORY_BIT_SET:FA_MEMORY_BIT_CLEAR,FA_PRIMARY,old,bit); return old;
}
static int byte_bit(const FlightActionHooks *h,gaddr a,unsigned bit) { uint8_t v=rd_u8(a); observe(h,FA_BIT_TEST,FA_PRIMARY,v,bit); return (v&(1u<<bit))!=0; }
static void increment_word(const FlightActionHooks *h,gaddr a) { uint16_t v=rd_u16(a); wr_u16(a,(uint16_t)(v+1)); observe(h,FA_MEMORY_ADD_WORD,FA_PRIMARY,v,1); }
static void copy_record(FlightActionState *w,const FlightActionHooks *h) {
    uint32_t last=0; unsigned i;
    for(i=0;i<41;++i) { last=rd_u32(w->source+4*i); wr_u32(w->record+4*i,last); }
    w->coefficients=w->source+164; w->copy=w->record+164; w->primary=0xffff;
    observe(h,FA_COPY_RECORD,FA_PRIMARY,w->source,w->record); observe(h,FA_STORE_LONG,FA_PRIMARY,last,0);
}
#define B(field,id,v) do { w.field=byte_half(w.field,(uint8_t)(v)); observe(h,FA_BYTE,id,w.field,0); } while(0)
#define W(field,id,v) do { w.field=word_half(w.field,(uint16_t)(v)); observe(h,FA_WORD,id,w.field,0); } while(0)
#define L(field,id,v) do { w.field=(uint32_t)(v); observe(h,FA_LONG,id,w.field,0); } while(0)
#define P(field,id,v) do { w.field=(gaddr)(v); observe(h,FA_POINTER,id,w.field,0); } while(0)
#define AW(field,id,v) do { uint16_t n_=(uint16_t)(v); w.field=word_half(w.field,(uint16_t)(w.field+n_)); observe(h,FA_ADD_WORD,id,n_,0); } while(0)
#define AB(field,id,v) do { uint8_t n_=(uint8_t)(v); w.field=byte_half(w.field,(uint8_t)(w.field+n_)); observe(h,FA_ADD_BYTE,id,n_,0); } while(0)
#define EW(field,id) do { w.field=word_half(w.field,(uint16_t)(int16_t)(int8_t)w.field); observe(h,FA_EXT_WORD,id,0,0); } while(0)
#define EL(field,id) do { w.field=extended_word(w.field); observe(h,FA_EXT_LONG,id,0,0); } while(0)
#define ASW(field,id,n) do { w.field=word_half(w.field,(uint16_t)((int16_t)w.field>>(n))); observe(h,FA_ASR_WORD,id,n,0); } while(0)
#define ASL(field,id,n) do { w.field=asr(w.field,n); observe(h,FA_ASR_LONG,id,n,0); } while(0)
#define ALW(field,id,n) do { w.field=word_half(w.field,(uint16_t)(w.field<<(n))); observe(h,FA_ASL_WORD,id,n,0); } while(0)
#define CWORD(a,b) observe(h,FA_COMPARE_WORD,FA_PRIMARY,(uint16_t)(a),(uint16_t)(b))
#define CBYTE(a,b) observe(h,FA_COMPARE_BYTE,FA_PRIMARY,(uint8_t)(a),(uint8_t)(b))
#define CLONG(a,b) observe(h,FA_COMPARE_LONG,FA_PRIMARY,(uint32_t)(a),(uint32_t)(b))
void select_flight_record_action(FlightActionState w,int allow_release,const FlightActionHooks *h) {
    uint8_t code;
    if(test_word(h,0xc459c2u)) goto no_action;
    P(source,FA_SOURCE,0xc46184u); B(primary,FA_PRIMARY,rd_u8(w.source+124));
    if(allow_release) B(selector,FA_SELECTOR,w.primary);
    w.primary=byte_half(w.primary,w.primary&15); observe(h,FA_AND_BYTE,FA_PRIMARY,15,0); code=(uint8_t)w.primary;
    if(allow_release) {
        CBYTE(code,14);
        if((int8_t)code>=14) { w.selector=byte_half(w.selector,w.selector&0xf0); observe(h,FA_AND_BYTE,FA_SELECTOR,0xf0,0); byte(h,w.source+124,(uint8_t)w.selector); consume(h,FA_RELEASE_ACTION); goto action_done; }
    }
    CBYTE(code,9);
    if(code==9 && !test_byte(h,0xc45785u)) { consume(h,FA_ACTION_SOUND); goto no_action; }
    CBYTE(code,1);
    if(code==1) { byte(h,0xc4586au,0x3f); byte(h,0xc45869u,0x78); byte(h,0xc457b8u,255); goto no_action; }
    CBYTE(code,3); if(code!=3) goto no_action;
    consume(h,FA_MANOEUVRE_ACTION);
action_done:
    L(primary,FA_PRIMARY,1); return;
no_action:
    L(primary,FA_PRIMARY,0);
}
void queue_flight_record_action_sound(const FlightActionHooks *h) {
    observe(h,FA_POINTER,FA_RECORD,0xc23174u,0); observe(h,FA_SOUND_ARGUMENTS,FA_PRIMARY,0xc23174u,0); consume(h,FA_SOUND_MESSAGE);
}
void append_flight_record_stream(FlightActionState w,const FlightActionHooks *h) {
    B(primary,FA_PRIMARY,rd_u8(0xc45799u)); EW(primary,FA_PRIMARY); ALW(primary,FA_PRIMARY,2);
    P(stream,FA_STREAM,0xc2366au); P(stream,FA_STREAM,rd_u32(w.stream+(uint32_t)(int32_t)(int16_t)w.primary));
    P(stream,FA_STREAM,rd_u32(w.stream)); P(stream,FA_STREAM,w.stream+2); P(coefficients,FA_COEFFICIENTS,0xc4fda2u);
    P(stream,FA_STREAM,w.stream+(uint32_t)(int32_t)rd_s16(w.coefficients)); byte(h,w.stream,rd_u8(w.record+101)); increment_word(h,w.coefficients);
    CWORD(rd_u16(w.coefficients),0x1fd); if(rd_s16(w.coefficients)<0x1fd) return;
    byte(h,w.stream,255); byte(h,0xc4579cu,255); B(primary,FA_PRIMARY,rd_u8(0xc45799u)); EW(primary,FA_PRIMARY); ALW(primary,FA_PRIMARY,2);
    P(stream,FA_STREAM,0xc2366au); P(stream,FA_STREAM,rd_u32(w.stream+(uint32_t)(int32_t)(int16_t)w.primary)); P(stream,FA_STREAM,rd_u32(w.stream));
    word(h,w.stream,rd_u16(w.coefficients));
}
static void play_flight_record_stream(FlightActionState w,const FlightActionHooks *h) {
    gaddr saved_record; uint8_t old;
    if(test_byte(h,0xc457aeu) || !byte_bit(h,w.record+2,0)) return;
    CBYTE(rd_u8(0xc458a6u),2); if(rd_u8(0xc458a6u)!=2) goto choose_stream;
    if(!test_byte(h,0xc45793u)) return;
    old=bit_change(h,0xc46186u,3,0); if(!(old&8)) goto choose_stream;
    saved_record=w.record; observe(h,FA_SAVE_RECORD,FA_PRIMARY,0,0); word(h,w.record,0); w=consume(h,FA_RESET_STREAM_RECORD);
    observe(h,FA_RESTORE_RECORD,FA_PRIMARY,0,0); w.record=saved_record; bit_change(h,w.record+2,0,1); byte(h,w.record+43,0x60); w=consume(h,FA_BEGIN_STREAM);
    P(stream,FA_STREAM,0xc2366au); B(primary,FA_PRIMARY,rd_u8(0xc45799u)); EW(primary,FA_PRIMARY); ALW(primary,FA_PRIMARY,3); goto read_stream;
choose_stream:
    P(stream,FA_STREAM,0xc2366au); B(primary,FA_PRIMARY,rd_u8(0xc45799u)); EW(primary,FA_PRIMARY); ALW(primary,FA_PRIMARY,3);
    if((int16_t)w.primary>0) goto read_stream;
    CBYTE(rd_u8(0xc458a6u),2); if(rd_u8(0xc458a6u)!=2) goto read_stream;
    byte(h,w.record+43,0x60); byte(h,0xc458b0u,0xfe); L(primary,FA_PRIMARY,rd_u32(w.stream+4+(uint32_t)(int32_t)(int16_t)w.primary));
    if(!w.primary) return; if((int32_t)w.primary<0) goto stream_end;
    CWORD(rd_u16(0xc4fda2u),20); if(rd_s16(0xc4fda2u)>20) goto stream_end; goto read_command;
read_stream:
    L(primary,FA_PRIMARY,rd_u32(w.stream+4+(uint32_t)(int32_t)(int16_t)w.primary)); if(!w.primary) return; if((int32_t)w.primary<0) goto negative_stream;
read_command:
    P(stream,FA_STREAM,w.primary); P(stream,FA_STREAM,rd_u32(w.stream)); P(stream,FA_STREAM,w.stream+2); P(coefficients,FA_COEFFICIENTS,0xc4fda2u);
    P(stream,FA_STREAM,w.stream+(uint32_t)(int32_t)rd_s16(w.coefficients)); B(primary,FA_PRIMARY,rd_u8(w.stream)); if((int8_t)w.primary<0) goto stream_end;
    bit_change(h,w.record+2,3,0); byte(h,w.record+101,(uint8_t)w.primary); increment_word(h,w.coefficients); CWORD(rd_u16(w.coefficients),0x1fd);
    if(rd_s16(w.coefficients)<0x1fd) return; W(primary,FA_PRIMARY,29); consume(h,FA_STREAM_LIMIT_MESSAGE); bit_change(h,w.record+2,0,0); return;
negative_stream:
    if(!test_word(h,0xc4fda2u)) { bit_change(h,w.record+2,3,0); byte(h,w.record+5,(uint8_t)w.primary); word(h,0xc4fda2u,1); return; }
    if(test_byte(h,w.record+5)) return; byte(h,w.record+101,rd_u8(w.record+101)&0xc3);
stream_end:
    CBYTE(rd_u8(0xc458a6u),2); if(rd_u8(0xc458a6u)==2) goto begin_next;
    old=bit_change(h,w.record+2,3,0); if(old&8) goto begin_next;
    { uint8_t pending=rd_u8(0xc4579au); observe(h,FA_TEST_BYTE,FA_PRIMARY,pending,0); if((int8_t)pending<0) return; }
    byte(h,0xc4579au,255); B(primary,FA_PRIMARY,rd_u8(0xc45799u)); AB(primary,FA_PRIMARY,1); CBYTE(w.primary,7);
    if((int8_t)w.primary>7) B(primary,FA_PRIMARY,1); EW(primary,FA_PRIMARY); ALW(primary,FA_PRIMARY,3);
    P(copy,FA_COPY,0xc23622u); W(primary,FA_PRIMARY,rd_u16(w.copy+(uint32_t)(int32_t)(int16_t)w.primary)); consume(h,FA_STREAM_END_MESSAGE); return;
begin_next:
    byte(h,0xc4579au,0); CBYTE(rd_u8(0xc45799u),7);
    if(rd_s8(0xc45799u)>=7) {
        saved_record=w.record; observe(h,FA_SAVE_RECORD,FA_PRIMARY,0,0); word(h,w.record,0); w=consume(h,FA_RESET_NEXT_RECORD);
        observe(h,FA_RESTORE_RECORD,FA_PRIMARY,0,0); w.record=saved_record; bit_change(h,w.record+2,0,1); byte(h,w.record+43,0x60);
    }
    w=consume(h,FA_NEXT_STREAM); goto choose_stream;
}
void advance_flight_record_stream(FlightActionState w,const FlightActionHooks *h) {
    CBYTE(rd_u8(0xc458a6u),125);
    if(rd_u8(0xc458a6u)==125 && !byte_bit(h,w.record+2,0)) {
        byte(h,w.record+5,0); if(!test_word(h,0xc461f2u)) return; bit_change(h,w.record+2,0,1); word(h,0xc4fda2u,0);
    }
    play_flight_record_stream(w,h);
}
void advance_flight_record_control(FlightActionState w,const FlightActionHooks *h) {
    uint8_t saved_class,code,old;
    W(primary,FA_PRIMARY,rd_u16(w.record+86)); W(selector,FA_SELECTOR,w.primary); ASW(selector,FA_SELECTOR,3); AW(primary,FA_PRIMARY,w.selector); AW(primary,FA_PRIMARY,rd_u16(0xc45946u)); ASW(primary,FA_PRIMARY,3);
    if((int16_t)w.primary<0) { w.primary=word_half(w.primary,(uint16_t)(0u-w.primary)); observe(h,FA_NEG_WORD,FA_PRIMARY,0,0); }
    CWORD(w.primary,78);
    if((int16_t)w.primary>=78) { if(!test_byte(h,0xc45888u)) { byte(h,0xc45888u,1); L(primary,FA_PRIMARY,8); w=consume(h,FA_MAGNITUDE_ALERT); } }
    else byte(h,0xc45888u,0);
    CBYTE(rd_u8(0xc458a6u),125);
    if(rd_u8(0xc458a6u)==125) {
        old=bit_change(h,w.record+2,3,0);
        if(old&8) {
            B(primary,FA_PRIMARY,rd_u8(0xc45799u)); CBYTE(rd_u8(0xc4579au),255);
            if(rd_u8(0xc4579au)!=255) { AB(primary,FA_PRIMARY,1); CBYTE(w.primary,7); if((int8_t)w.primary>7) B(primary,FA_PRIMARY,1); }
            byte(h,0xc45799u,(uint8_t)w.primary); AB(primary,FA_PRIMARY,1); CBYTE(w.primary,7); if((int8_t)w.primary>7) B(primary,FA_PRIMARY,1);
            EW(primary,FA_PRIMARY); ALW(primary,FA_PRIMARY,3); P(copy,FA_COPY,0xc23622u); W(primary,FA_PRIMARY,rd_u16(w.copy+(uint32_t)(int32_t)(int16_t)w.primary)); w=consume(h,FA_CLONE_MESSAGE);
            byte(h,0xc4579au,0xfe); saved_class=rd_u8(w.record+99); B(selector,FA_SELECTOR,saved_class); copy_record(&w,h); byte(h,w.record+99,saved_class);
            byte(h,w.record+94,0); byte(h,w.record+98,17); bit_change(h,w.record+2,0,0); byte(h,w.record+5,0); byte(h,w.record+101,0);
            L(x,FA_X,9); L(y,FA_Y,1); W(z,FA_Z,0xff9f); w=consume(h,FA_CLONE_PROJECTION);
            longword(h,w.record+20,w.primary); longword(h,w.record+24,w.selector); longword(h,w.record+28,w.detail);
            L(x,FA_X,w.primary); L(y,FA_Y,w.detail); ASL(x,FA_X,8); ASL(y,FA_Y,8);
            w.x=word_half(w.x,w.x&0x3fff); observe(h,FA_AND_WORD,FA_X,0x3fff,0); w.y=word_half(w.y,w.y&0x3fff); observe(h,FA_AND_WORD,FA_Y,0x3fff,0);
            w.primary=swapped(w.primary); observe(h,FA_SWAP,FA_PRIMARY,0,0); w.detail=swapped(w.detail); observe(h,FA_SWAP,FA_DETAIL,0,0); ASW(primary,FA_PRIMARY,6); ASW(detail,FA_DETAIL,6);
            wr_u16(w.record+6,(uint16_t)w.primary); wr_u16(w.record+8,(uint16_t)w.detail); wr_u16(w.record+12,(uint16_t)w.x); wr_u16(w.record+14,(uint16_t)w.y);
            ASL(selector,FA_SELECTOR,8); longword(h,w.record+16,w.selector); byte(h,0xc45858u,255);
        }
    }
    code=rd_u8(0xc4579cu); observe(h,FA_TEST_BYTE,FA_PRIMARY,code,0); if((int8_t)code<0) return;
    if(test_byte(h,0xc457aeu)) return;
    /* C2334E BNE and C23350 BEQ consume the same TST result. The sealed
     * entry always takes the latter here. The append arm is separately kept
     * above and proved as an internal segment, without fabricating an entry. */
    play_flight_record_stream(w,h);
}
void select_next_flight_record_stream(FlightActionState w,const FlightActionHooks *h) {
    B(primary,FA_PRIMARY,rd_u8(0xc45799u)); AB(primary,FA_PRIMARY,1); CBYTE(w.primary,7); if((int8_t)w.primary>7) B(primary,FA_PRIMARY,1);
    byte(h,0xc45799u,(uint8_t)w.primary); byte(h,0xc457a6u,0); word(h,0xc4fda2u,0); byte(h,w.record+101,0);
    B(primary,FA_PRIMARY,rd_u8(0xc45799u)); if((int8_t)w.primary<=0) L(primary,FA_PRIMARY,1); EW(primary,FA_PRIMARY); W(selector,FA_SELECTOR,w.primary);
    W(primary,FA_PRIMARY,rd_u16(0xc459b4u)); CWORD(w.primary,rd_u16(0xc458dcu));
    if((uint16_t)w.primary==rd_u16(0xc458dcu)) { ALW(primary,FA_PRIMARY,3); P(copy,FA_COPY,0xc2366au); P(copy,FA_COPY,w.copy+(uint32_t)(int32_t)(int16_t)w.primary); L(primary,FA_PRIMARY,rd_u32(w.copy)); byte(h,0xc458b2u,(uint8_t)w.primary); }
    CBYTE(rd_u8(0xc458a6u),2);
    if(rd_u8(0xc458a6u)==2 && test_byte(h,0xc45785u)) {
        w.primary=word_half(w.primary,(uint16_t)w.primary>>8); observe(h,FA_LSR_WORD,FA_PRIMARY,8,0); w.primary=byte_half(w.primary,w.primary&255); observe(h,FA_AND_BYTE,FA_PRIMARY,255,0);
        if((int8_t)w.primary<=0) L(primary,FA_PRIMARY,0xffffffffu); byte(h,0xc45785u,(uint8_t)w.primary);
    }
    byte(h,0xc45858u,255); CBYTE(w.selector,4);
    if((uint8_t)w.selector==4) { CLONG(rd_u32(w.record+24),0x100000); if(rd_s32(w.record+24)<=0x100000) { W(primary,FA_PRIMARY,53); goto message; } }
    P(copy,FA_COPY,0xc23622u); ALW(selector,FA_SELECTOR,3); W(primary,FA_PRIMARY,rd_u16(w.copy+2+(uint32_t)(int32_t)(int16_t)w.selector));
message:
    consume(h,FA_NEXT_MESSAGE);
}
void apply_flight_record_action_motion(FlightActionState w,const FlightActionHooks *h) {
    uint32_t last;
    P(coefficients,FA_COEFFICIENTS,0xc22048u); P(coefficients,FA_COEFFICIENTS,w.coefficients+(uint32_t)(int32_t)(int16_t)w.primary); P(copy,FA_COPY,0xc22188u);
    W(x,FA_X,rd_u16(0xc459b4u)); AW(x,FA_X,w.x); AW(x,FA_X,w.x); W(y,FA_Y,w.x); AW(x,FA_X,w.x); AW(x,FA_X,w.x); AW(x,FA_X,w.y);
    P(copy,FA_COPY,w.copy+(uint32_t)(int32_t)(int16_t)w.x);
    w.detail=rd_u32(w.coefficients); w.x=rd_u32(w.coefficients+4); w.y=rd_u32(w.coefficients+8); w.z=rd_u32(w.coefficients+12); w.product_a=rd_u32(w.coefficients+16);
    observe(h,FA_LOAD_DESCRIPTOR,FA_PRIMARY,w.coefficients,0);
    wr_u32(w.copy,w.detail); wr_u32(w.copy+4,w.x); wr_u32(w.copy+8,w.y); wr_u32(w.copy+12,w.z); wr_u32(w.copy+16,w.product_a);
    word(h,w.record+86,0); word(h,w.record+88,0); word(h,w.record+90,0); byte(h,w.record+100,0); bit_change(h,w.record+3,2,0);
    B(selector,FA_SELECTOR,rd_u8(w.record+98)); w.selector=byte_half(w.selector,w.selector&0xf0); observe(h,FA_AND_BYTE,FA_SELECTOR,0xf0,0); CBYTE(w.selector,48);
    if((uint8_t)w.selector==48) { P(coefficients,FA_COEFFICIENTS,0xc23a26u); L(primary,FA_PRIMARY,0); CBYTE(rd_u8(w.record+98),48); if(rd_u8(w.record+98)!=48) L(primary,FA_PRIMARY,1); }
    else {
        B(selector,FA_SELECTOR,rd_u8(w.source+95)); B(detail,FA_DETAIL,w.selector); B(primary,FA_PRIMARY,rd_u8(w.source+99)); w.primary=byte_half(w.primary,w.primary&0xf0); observe(h,FA_AND_BYTE,FA_PRIMARY,0xf0,0); CBYTE(w.primary,48);
        if((uint8_t)w.primary!=48) {
            w.selector=byte_half(w.selector,w.selector&0xf0); observe(h,FA_AND_BYTE,FA_SELECTOR,0xf0,0); w.detail=byte_half(w.detail,w.detail&15); observe(h,FA_AND_BYTE,FA_DETAIL,15,0);
            w.selector=byte_half(w.selector,(uint8_t)(w.selector-16)); observe(h,FA_SUB_BYTE,FA_SELECTOR,16,0); B(primary,FA_PRIMARY,w.selector); w.primary=byte_half(w.primary,(uint8_t)w.primary>>4); observe(h,FA_LSR_BYTE,FA_PRIMARY,4,0);
        } else {
            w.selector=byte_half(w.selector,w.selector&15); observe(h,FA_AND_BYTE,FA_SELECTOR,15,0); w.detail=byte_half(w.detail,w.detail&0xf0); observe(h,FA_AND_BYTE,FA_DETAIL,0xf0,0);
            w.selector=byte_half(w.selector,(uint8_t)(w.selector-1)); observe(h,FA_SUB_BYTE,FA_SELECTOR,1,0); B(primary,FA_PRIMARY,w.selector); AB(primary,FA_PRIMARY,2);
        }
        w.selector=byte_half(w.selector,w.selector|w.detail); observe(h,FA_OR_BYTE,FA_SELECTOR,w.detail,0); byte(h,w.source+95,(uint8_t)w.selector); byte(h,0xc45843u,3); byte(h,0xc45844u,3);
        CBYTE(rd_u8(w.source+98),16); P(coefficients,FA_COEFFICIENTS,rd_u8(w.source+98)==16?0xc23a32u:0xc23a56u);
    }
    EW(primary,FA_PRIMARY); AW(primary,FA_PRIMARY,w.primary); W(selector,FA_SELECTOR,w.primary); AW(selector,FA_SELECTOR,w.selector); AW(primary,FA_PRIMARY,w.selector);
    last=w.coefficients+(uint32_t)(int32_t)(int16_t)w.primary;
    w.x=extended_word(rd_u16(last)); w.y=extended_word(rd_u16(last+2)); w.z=extended_word(rd_u16(last+4));
    /* MOVEM.W sign-extends all coefficients and leaves CCR unchanged. */
    observe(h,FA_LOAD_COEFFICIENTS,FA_PRIMARY,last,0);
#define MULT(field,id,offset) do { uint16_t operand=rd_u16(w.source+(offset)); w.field=(uint32_t)((int32_t)(int16_t)w.field*(int32_t)(int16_t)operand); observe(h,FA_MULTIPLY,id,operand,0); } while(0)
#define AL(field,id,v) do { uint32_t n_=(uint32_t)(v); w.field+=n_; observe(h,FA_ADD_LONG,id,n_,0); } while(0)
    W(product_a,FA_PRODUCT_A,w.x); W(primary,FA_PRIMARY,w.y); W(product_b,FA_PRODUCT_B,w.z); MULT(product_a,FA_PRODUCT_A,146); MULT(primary,FA_PRIMARY,148); MULT(product_b,FA_PRODUCT_B,150); AL(primary,FA_PRIMARY,w.product_a); AL(primary,FA_PRIMARY,w.product_b);
    W(product_a,FA_PRODUCT_A,w.x); W(selector,FA_SELECTOR,w.y); W(product_b,FA_PRODUCT_B,w.z); MULT(product_a,FA_PRODUCT_A,152); MULT(selector,FA_SELECTOR,154); MULT(product_b,FA_PRODUCT_B,156); AL(selector,FA_SELECTOR,w.product_a); AL(selector,FA_SELECTOR,w.product_b);
    W(detail,FA_DETAIL,w.y); MULT(x,FA_X,158); MULT(detail,FA_DETAIL,160); MULT(z,FA_Z,162); AL(detail,FA_DETAIL,w.x); AL(detail,FA_DETAIL,w.z);
    ASL(primary,FA_PRIMARY,6); ASL(selector,FA_SELECTOR,6); ASL(detail,FA_DETAIL,6);
    last=rd_u32(w.record+20); wr_u32(w.record+20,last+w.primary); observe(h,FA_ADD_LONG,FA_STATS,last,w.primary);
    last=rd_u32(w.record+24); wr_u32(w.record+24,last+w.selector); observe(h,FA_ADD_LONG,FA_STATS,last,w.selector);
    last=rd_u32(w.record+28); wr_u32(w.record+28,last+w.detail); observe(h,FA_ADD_LONG,FA_STATS,last,w.detail);
    W(primary,FA_PRIMARY,150); CBYTE(rd_u8(w.record+98),0); if(rd_u8(w.record+98)!=0) W(primary,FA_PRIMARY,200); word(h,w.record+76,(uint16_t)w.primary); word(h,w.record+38,20);
    word(h,w.record,rd_u16(w.record)|0x10c0); word(h,w.record,rd_u16(w.record)|0x100); word(h,w.record,rd_u16(w.record)|2); word(h,w.record+118,0);
    W(primary,FA_PRIMARY,rd_u16(w.record+2)); w.primary=word_half(w.primary,w.primary&0x80); observe(h,FA_AND_WORD,FA_PRIMARY,0x80,0); if((uint16_t)w.primary) longword(h,w.record+66,0);
    word(h,w.record+2,rd_u16(w.record+2)&0xff7f); word(h,w.record+44,0xffff); byte(h,0xc45858u,0x8c); byte(h,w.record+56,rd_u8(w.source+56));
    B(primary,FA_PRIMARY,rd_u8(w.record+98)); w.primary=byte_half(w.primary,w.primary&0xf0); observe(h,FA_AND_BYTE,FA_PRIMARY,0xf0,0); CBYTE(w.primary,48); if((uint8_t)w.primary!=48) return;
    W(z,FA_Z,rd_u16(w.source+148)); W(product_a,FA_PRODUCT_A,rd_u16(w.source+154)); W(product_b,FA_PRODUCT_B,rd_u16(w.source+160)); ASW(z,FA_Z,2); ASW(product_a,FA_PRODUCT_A,2); ASW(product_b,FA_PRODUCT_B,2);
    CBYTE(rd_u8(w.record+98),48);
    if(rd_u8(w.record+98)!=48) { w.z=word_half(w.z,(uint16_t)(0u-w.z)); observe(h,FA_NEG_WORD,FA_Z,0,0); w.product_a=word_half(w.product_a,(uint16_t)(0u-w.product_a)); observe(h,FA_NEG_WORD,FA_PRODUCT_A,0,0); w.product_b=word_half(w.product_b,(uint16_t)(0u-w.product_b)); observe(h,FA_NEG_WORD,FA_PRODUCT_B,0,0); }
    W(primary,FA_PRIMARY,960); w=consume(h,FA_ACTION_NORMALISE);
    w.primary=rd_u32(w.record+62); w.selector=rd_u32(w.record+66); w.detail=rd_u32(w.record+70); observe(h,FA_LOAD_MOTION,FA_PRIMARY,w.record+62,0);
    AL(z,FA_Z,w.primary); AL(product_a,FA_PRODUCT_A,w.selector); AL(product_b,FA_PRODUCT_B,w.detail);
    wr_u32(w.record+62,w.z); wr_u32(w.record+66,w.product_a); wr_u32(w.record+70,w.product_b); word(h,w.record+76,0xffec);
#undef AL
#undef MULT
}
void initialise_flight_record_manoeuvre(FlightActionState w,const FlightActionHooks *h) {
    gaddr saved_record,saved_source;
    L(selector,FA_SELECTOR,w.record); w.selector-=0xc46184u; observe(h,FA_SUB_LONG,FA_SELECTOR,0xc46184u,0); word(h,0xc459c2u,(uint16_t)w.selector);
    if(!test_byte(h,0xc45785u)) { W(selector,FA_SELECTOR,rd_u16(0xc459b4u)); w=consume(h,FA_REFRESH_ACTION_VIEW); }
    copy_record(&w,h); bit_change(h,w.record,7,0); byte(h,w.record+98,48); W(primary,FA_PRIMARY,0xe10); W(detail,FA_DETAIL,0x3840); W(y,FA_Y,0x3840); P(coefficients,FA_COEFFICIENTS,w.record+128);
    saved_record=w.record; saved_source=w.source; observe(h,FA_SAVE_RECORD_SOURCES,FA_PRIMARY,0,0); w=consume(h,FA_ACTION_ROTATION);
    wr_u16(w.record+102,(uint16_t)w.y); wr_u16(w.record+104,(uint16_t)w.z); wr_u16(w.record+106,(uint16_t)w.product_a);
    w=consume(h,FA_ACTION_MATRIX); observe(h,FA_RESTORE_RECORD_SOURCES,FA_PRIMARY,0,0); w.record=saved_record; w.source=saved_source;
    W(primary,FA_PRIMARY,40); apply_flight_record_action_motion(w,h);
}
void initialise_flight_record_release(FlightActionState w,const FlightActionHooks *h) {
    L(selector,FA_SELECTOR,w.record); w.selector-=0xc46184u; observe(h,FA_SUB_LONG,FA_SELECTOR,0xc46184u,0); word(h,0xc458c2u,(uint16_t)w.selector);
    copy_record(&w,h); bit_change(h,w.record,7,0); byte(h,w.record+98,49); W(primary,FA_PRIMARY,220); apply_flight_record_action_motion(w,h);
}
void try_flight_record_action(FlightActionState w,const FlightActionHooks *h) {
    uint32_t mask; uint8_t kind;
    if(!test_byte(h,0xc45789u)) goto inactive;
    CBYTE(rd_u8(0xc458a7u),3);
    if(rd_s8(0xc458a7u)>=3) L(selector,FA_SELECTOR,3);
    else { CBYTE(rd_u8(0xc458a7u),2); L(selector,FA_SELECTOR,rd_s8(0xc458a7u)>=2?5:7); }
    if(test_byte(h,0xc458b5u)) { w.selector=word_half(w.selector,(uint16_t)w.selector>>1); observe(h,FA_LSR_WORD,FA_SELECTOR,1,0); }
    W(primary,FA_PRIMARY,rd_u16(0xc458dau)); w.primary=word_half(w.primary,w.primary&w.selector); observe(h,FA_AND_WORD,FA_PRIMARY,w.selector,0); if((uint16_t)w.primary) goto inactive;
    B(selector,FA_SELECTOR,rd_u8(w.source+95)); B(primary,FA_PRIMARY,rd_u8(w.source+99)); w.primary=byte_half(w.primary,w.primary&0xf0); observe(h,FA_AND_BYTE,FA_PRIMARY,0xf0,0); CBYTE(w.primary,48);
    mask=(uint8_t)w.primary==48?15:0xf0; w.selector=byte_half(w.selector,w.selector&mask); observe(h,FA_AND_BYTE,FA_SELECTOR,mask,0); if(!(uint8_t)w.selector) goto inactive;
    L(primary,FA_PRIMARY,w.source); w.primary-=0xc46184u; observe(h,FA_SUB_LONG,FA_PRIMARY,0xc46184u,0); CWORD(w.primary,rd_u16(0xc458deu));
    if((uint16_t)w.primary==rd_u16(0xc458deu)) { byte(h,0xc45797u,8); byte(h,0xc458b0u,0xfb); }
    copy_record(&w,h); byte(h,w.record+5,0); P(stats,FA_STATS,rd_u32(0xc1ab74u)); B(primary,FA_PRIMARY,rd_u8(w.record+99)); w.primary=byte_half(w.primary,w.primary&0xf0); observe(h,FA_AND_BYTE,FA_PRIMARY,0xf0,0); kind=(uint8_t)w.primary; CBYTE(kind,48);
    byte(h,w.record+98,kind==48?1:0); W(primary,FA_PRIMARY,kind==48?20:0); observe(h,FA_COMPARE_ADDRESS,FA_PRIMARY,w.source,0xc46184u);
    if(w.source==0xc46184u) { increment_word(h,w.stats+(kind==48?62:66)); byte(h,0xc457c5u,1); }
    apply_flight_record_action_motion(w,h); return;
inactive:
    bit_change(h,w.record+1,6,0);
}
void normalise_flight_record_direction(gaddr frame,const FlightActionHooks *h) {
    FlightActionState w={0}; uint32_t factor,length,quotient; unsigned shift; uint16_t divisor;
    observe(h,FA_DIRECTION_ARGUMENTS,FA_PRIMARY,frame+8,0); w.primary=rd_u32(frame+8); w.z=rd_u32(frame+12); w.product_a=rd_u32(frame+16); w.product_b=rd_u32(frame+20);
    observe(h,FA_TEST_WORD,FA_PRIMARY,(uint16_t)w.primary,0);
    if(!(uint16_t)w.primary) { for(;;) { word(h,0xc4599eu,21); consume(h,FA_DIRECTION_FAULT); } }
    if((int16_t)w.primary<0) { w.primary=word_half(w.primary,(uint16_t)(0u-w.primary)); observe(h,FA_NEG_WORD,FA_PRIMARY,0,0); }
    W(detail,FA_DETAIL,w.z); if((int16_t)w.detail<0) { w.detail=word_half(w.detail,(uint16_t)(0u-w.detail)); observe(h,FA_NEG_WORD,FA_DETAIL,0,0); }
    W(x,FA_X,w.product_a); if((int16_t)w.x<0) { w.x=word_half(w.x,(uint16_t)(0u-w.x)); observe(h,FA_NEG_WORD,FA_X,0,0); }
    W(y,FA_Y,w.product_b); if((int16_t)w.y<0) { w.y=word_half(w.y,(uint16_t)(0u-w.y)); observe(h,FA_NEG_WORD,FA_Y,0,0); }
    w=consume(h,FA_DIRECTION_LENGTH); L(detail,FA_DETAIL,3); EL(primary,FA_PRIMARY); factor=w.primary; length=w.selector; shift=3;
    for(;;) { CLONG(factor,length); if((int32_t)factor>(int32_t)length) break; factor<<=2; observe(h,FA_ASL_LONG,FA_PRIMARY,2,0); shift=(uint16_t)(shift+2); observe(h,FA_ADD_WORD,FA_DETAIL,2,0); }
    for(;;) {
        factor=asr(factor,2); observe(h,FA_ASR_LONG,FA_PRIMARY,2,0); shift=(uint16_t)(shift-2); observe(h,FA_SUB_WORD,FA_DETAIL,2,0); CWORD(shift,1); if((int16_t)shift<=1) break;
        CLONG(factor,length); if((int32_t)factor<=(int32_t)length) break;
    }
    factor<<=2; observe(h,FA_ASL_LONG,FA_PRIMARY,2,0); shift=(uint16_t)(shift+2); observe(h,FA_ADD_WORD,FA_DETAIL,2,0); factor<<=8; observe(h,FA_ASL_LONG,FA_PRIMARY,8,0); divisor=(uint16_t)length;
    if(!divisor) { if(!h || !h->divide_exception) abort(); w=h->divide_exception(h->context); quotient=w.primary; shift=(uint16_t)w.detail; }
    else { quotient=factor/divisor; quotient=quotient>0xffff?factor:((factor%divisor)<<16)|quotient; observe(h,FA_DIRECTION_DIVIDE,FA_PRIMARY,divisor,0); }
#define PRODUCT(field,id) do { w.field=(uint32_t)((int32_t)(int16_t)w.field*(int32_t)(int16_t)quotient); observe(h,FA_MULTIPLY,id,(uint16_t)quotient,0); } while(0)
    PRODUCT(z,FA_Z); PRODUCT(product_a,FA_PRODUCT_A); PRODUCT(product_b,FA_PRODUCT_B); ASL(z,FA_Z,shift); ASL(product_a,FA_PRODUCT_A,shift); ASL(product_b,FA_PRODUCT_B,shift);
    observe(h,FA_TEST_WORD,FA_PRIMARY,rd_u16(frame+8),0);
    if(rd_s16(frame+8)<0) { w.z=word_half(w.z,(uint16_t)(0u-w.z)); observe(h,FA_NEG_WORD,FA_Z,0,0); w.product_a=word_half(w.product_a,(uint16_t)(0u-w.product_a)); observe(h,FA_NEG_WORD,FA_PRODUCT_A,0,0); w.product_b=word_half(w.product_b,(uint16_t)(0u-w.product_b)); observe(h,FA_NEG_WORD,FA_PRODUCT_B,0,0); }
    wr_u16(0xc45a4cu,(uint16_t)w.z); wr_u16(0xc45a4eu,(uint16_t)w.product_a); wr_u16(0xc45a50u,(uint16_t)w.product_b); observe(h,FA_DIRECTION_PUBLISH,FA_PRIMARY,0,0);
#undef PRODUCT
}
#undef B
#undef W
#undef L
#undef P
#undef AW
#undef AB
#undef EW
#undef EL
#undef ASW
#undef ASL
#undef ALW
#undef CWORD
#undef CBYTE
#undef CLONG
