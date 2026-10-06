/* Original stream-fed Custom stores, three numeric fields and marker siblings. */
#include "hud_stream.h"
#include <stdlib.h>
static void observe(const HudStreamHooks *h,enum StreamPhase p,enum StreamField f,uint32_t v,uint32_t o) {
    if(h && h->observe) h->observe(h->context,p,f,v,o);
}
static HudStreamState consume(const HudStreamHooks *h,enum StreamChild c,HudStreamState w) {
    if(h && h->consume_values) return h->consume_values(h->context,c,w);
    if(h && h->consume) return h->consume(h->context,c); abort();
}
static uint32_t low_word(uint32_t old,uint16_t v) { return (old&0xffff0000u)|v; }
#define W(f,id,v) do { w.f=low_word(w.f,(uint16_t)(v)); observe(h,HS_WORD,id,w.f,0); } while(0)
#define L(f,id,v) do { w.f=(uint32_t)(v); observe(h,HS_LONG,id,w.f,0); } while(0)
#define P(f,id,v) do { w.f=(gaddr)(v); observe(h,HS_POINTER,id,w.f,0); } while(0)
#define AW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f+n_)); observe(h,HS_ADD_WORD,id,n_,0); } while(0)
#define AL(f,id,v) do { uint32_t n_=(uint32_t)(v); w.f+=n_; observe(h,HS_ADD_LONG,id,n_,0); } while(0)
#define EL(f,id) do { w.f=(uint32_t)(int32_t)(int16_t)w.f; observe(h,HS_EXT_LONG,id,0,0); } while(0)
static void word(const HudStreamHooks *h,gaddr address,uint16_t v) { wr_u16(address,v); observe(h,HS_STORE_WORD,HS_PRIMARY,v,0); }
static void longword(const HudStreamHooks *h,gaddr address,uint32_t v) { wr_u32(address,v); observe(h,HS_STORE_LONG,HS_PRIMARY,v,0); }

/* A0 is the original caller's register base, not a hardcoded Custom address. */
void store_stream_cd(HudStreamState w,const HudStreamHooks *h) {
    word(h,w.registers+0x40,(uint16_t)w.control); longword(h,w.registers+0x48,w.destination);
    longword(h,w.registers+0x54,w.destination); word(h,w.registers+0x58,(uint16_t)w.size);
}
void store_stream_ad(HudStreamState w,const HudStreamHooks *h) {
    word(h,w.registers+0x40,(uint16_t)w.control); longword(h,w.registers+0x50,w.primary);
    longword(h,w.registers+0x54,w.destination); word(h,w.registers+0x58,(uint16_t)w.size);
}
void submit_next_stream_cd(HudStreamState w,const HudStreamHooks *h) {
    L(destination,HS_DESTINATION,rd_u32(w.stream)); P(stream,HS_STREAM,w.stream+4); AL(destination,HS_DESTINATION,w.base);
    w=consume(h,HS_WAIT_NEXT,w); store_stream_cd(w,h);
}
void submit_table_stream_ad(HudStreamState w,const HudStreamHooks *h) {
    L(destination,HS_DESTINATION,rd_u32(w.stream)); P(stream,HS_STREAM,w.stream+4); AL(destination,HS_DESTINATION,w.base);
    P(record,HS_RECORD,rd_u32(w.pointers)); P(pointers,HS_POINTERS,w.pointers+4);
    L(primary,HS_PRIMARY,rd_u32(w.record)); AL(primary,HS_PRIMARY,w.offset);
    w=consume(h,HS_WAIT_TABLE,w); store_stream_ad(w,h);
}
void submit_other_stream_acd(HudStreamState w,const HudStreamHooks *h) {
    L(destination,HS_DESTINATION,rd_u32(w.stream)); P(stream,HS_STREAM,w.stream+4); AL(destination,HS_DESTINATION,w.base);
    P(record,HS_RECORD,rd_u32(w.pointers)); P(pointers,HS_POINTERS,w.pointers+4);
    L(secondary,HS_SECONDARY,rd_u32(w.record)); AL(secondary,HS_SECONDARY,w.offset);
    w=consume(h,HS_WAIT_OTHER,w);
    word(h,w.registers+0x40,(uint16_t)w.control); longword(h,w.registers+0x50,w.primary);
    longword(h,w.registers+0x4c,w.secondary); longword(h,w.registers+0x48,w.destination);
    longword(h,w.registers+0x54,w.destination); word(h,w.registers+0x58,(uint16_t)w.size);
}
static HudStreamState numeric_field(HudStreamState w,unsigned field,const HudStreamHooks *h) {
    EL(primary,HS_PRIMARY); longword(h,0xc45b1eu,w.primary);
    w=consume(h,(enum StreamChild)(HS_BCD_FIRST+2*field),w);
    L(primary,HS_PRIMARY,3); P(pointers,HS_POINTERS,0xc31a4cu); P(stream,HS_STREAM,0xc457fbu);
    P(registers,HS_REGISTERS,w.stream+4); P(screen,HS_SCREEN,120+4*field); P(planes,HS_PLANES,4*field);
    return consume(h,(enum StreamChild)(HS_DIGITS_FIRST+2*field),w);
}
void draw_stream_numeric_fields(HudStreamState w,const HudStreamHooks *h) {
    unsigned field;
    word(h,0xc45954u,9); W(primary,HS_PRIMARY,rd_u16(0xc45ae6u)); w=numeric_field(w,0,h);
    observe(h,HS_TEST_BYTE,HS_PRIMARY,rd_u8(0xc457b3u),0); if(!rd_u8(0xc457b3u)) return;
    for(field=1;field<3;++field) {
        W(primary,HS_PRIMARY,rd_u16(0xc45774u+2*field));
        if((int16_t)w.primary<0) {
            w.primary=low_word(w.primary,(uint16_t)(0u-w.primary)); observe(h,HS_NEG_WORD,HS_PRIMARY,0,0);
        }
        w=numeric_field(w,field,h);
    }
}
void draw_bounded_stream_marker(HudStreamState w,int second,const HudStreamHooks *h) {
    AW(primary,HS_PRIMARY,rd_u16(0xc45988u));
    observe(h,HS_COMPARE_WORD,HS_PRIMARY,(uint16_t)w.primary,315); if((int16_t)w.primary>=315) return;
    W(base,HS_BASE,90); W(control,HS_CONTROL,w.primary); AW(control,HS_CONTROL,4);
    observe(h,HS_COMPARE_WORD,HS_PRIMARY,(uint16_t)w.control,4); if((int16_t)w.control<=4) return;
    W(secondary,HS_SECONDARY,w.base); AW(base,HS_BASE,rd_u16(0xc458d8u)); AW(secondary,HS_SECONDARY,rd_u16(0xc458d8u));
    (void)consume(h,second?HS_LINE_SECOND:HS_LINE_FIRST,w);
}
