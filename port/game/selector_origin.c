#include "selector_origin.h"
#include "globals.h"
#include <stdlib.h>

static void observe(const SelectorOriginHooks *h, enum SelectorOriginPhase phase,
                    gaddr address, uint32_t value, uint32_t previous, uint32_t parameter) {
    SelectorOriginEvent e={phase,address,value,previous,parameter};
    if(h->observe) h->observe(h->context,&e);
}
static SelectorOriginTriple load_triple(gaddr address) {
    SelectorOriginTriple v; unsigned i;
    for(i=0;i<3;++i) v.component[i]=rd_u32(address+4*i);
    return v;
}
static void store_triple(gaddr address,SelectorOriginTriple v) {
    unsigned i; for(i=0;i<3;++i) wr_u32(address+4*i,v.component[i]);
}
static uint32_t arithmetic_right(uint32_t value,unsigned shift) {
    shift&=63u;
    return shift<32 ? (uint32_t)((int32_t)value>>shift) : (uint32_t)((int32_t)value>>31);
}
static uint32_t left(uint32_t value,unsigned shift) {
    shift&=63u; return shift<32 ? value<<shift : 0;
}
static int byte_test(const SelectorOriginHooks *h,gaddr address) {
    uint8_t value=rd_u8(address); observe(h,ORIGIN_BYTE_TEST,address,value,0,0); return value;
}
static int byte_compare(const SelectorOriginHooks *h,gaddr address,int8_t limit) {
    int8_t value=rd_s8(address);
    observe(h,ORIGIN_BYTE_COMPARE,address,(uint8_t)value,(uint8_t)limit,0);
    return value<limit ? -1 : value>limit;
}
static void publish_byte(const SelectorOriginHooks *h,gaddr address,uint8_t value) {
    wr_u8(address,value); observe(h,ORIGIN_BYTE_PUBLISH,address,value,0,0);
}
static void publish_word(const SelectorOriginHooks *h,gaddr address,uint16_t value) {
    wr_u16(address,value); observe(h,ORIGIN_WORD_PUBLISH,address,value,0,0);
}
static int threshold(const SelectorOriginHooks *h,uint32_t magnitude,uint32_t limit,
                     unsigned shift) {
    observe(h,ORIGIN_THRESHOLD,0,magnitude,limit,shift);
    return (int32_t)magnitude>(int32_t)limit;
}
static void companion(const SelectorOriginHooks *h,SelectorOriginTriple v) {
    unsigned i;
    observe(h,ORIGIN_COMPANION,0,0,0,0);
    for(i=0;i<3;++i) v.component[i]=0u-(v.component[i]&(i==1 ? UINT32_MAX : 0x3fffffu));
    store_triple(ORIGIN_NEGATED_COMPANION,v);
}
static void blend(const SelectorOriginHooks *h) {
    SelectorOriginTriple v=load_triple(SELECTOR_ORIGIN),first,second,record;
    gaddr active=CONTROL_RECORDS+(gaddr)(int32_t)rd_s16(ORIGIN_RECORD_OFFSET);
    unsigned i;
    observe(h,ORIGIN_BLEND_SAVE,0,0,0,0);
    record=load_triple(active+0x14u);
    observe(h,ORIGIN_BLEND_START,active,0,0,0);
    for(i=0;i<3;++i) first.component[i]=arithmetic_right(v.component[i]+record.component[i],1);
    observe(h,ORIGIN_BLEND_FIRST,0,0,0,0);
    for(i=0;i<3;++i) second.component[i]=arithmetic_right(first.component[i]+(i==1?0:v.component[i]),1);
    observe(h,ORIGIN_BLEND_SECOND,0,0,0,0);
    for(i=0;i<3;++i) second.component[i]=arithmetic_right(second.component[i]+first.component[i],1);
    observe(h,ORIGIN_BLEND_THIRD,0,0,0,0);
    for(i=0;i<3;++i) first.component[i]=arithmetic_right(second.component[i]+first.component[i],1);
    observe(h,ORIGIN_BLEND_LAST,0,0,0,0);
    for(i=0;i<3;++i) v.component[i]=arithmetic_right(first.component[i]+second.component[i],1);
    store_triple(ORIGIN_CANDIDATE_TRIPLE,v);
}
void load_origin_candidate_preset(const SelectorOriginHooks *h,gaddr preset) {
    observe(h,ORIGIN_PRESET_RESULT,preset,0,0,0);
    store_triple(ORIGIN_CANDIDATE_TRIPLE,load_triple(preset));
}
static void small_candidate(const SelectorOriginHooks *h,int alternate) {
    SelectorOriginTriple result,input;
    int positive=rd_u8(ORIGIN_VARIANT_SELECTOR)==1 || rd_u8(ORIGIN_VARIANT_SELECTOR)==2;
    observe(h,ORIGIN_SMALL_SAVE,0,0,0,0);
    observe(h,ORIGIN_SMALL_INPUTS,0,rd_u8(ORIGIN_VARIANT_SELECTOR),0,(unsigned)alternate);
    input.component[0]=positive?6:0xfffffffau;
    input.component[1]=alternate?1:4;
    input.component[2]=alternate?6:positive?3:0xfffffff7u;
    result=h->consume(h->context,ORIGIN_MATRIX_B,&input);
    store_triple(ORIGIN_CANDIDATE_TRIPLE,result);
    observe(h,ORIGIN_SMALL_RESTORE,0,0,0,0);
}
static void countdown(const SelectorOriginHooks *h) {
    uint8_t old=rd_u8(ORIGIN_DETAIL_COUNTER),value=(uint8_t)(old-1u);
    wr_u8(ORIGIN_DETAIL_COUNTER,value); observe(h,ORIGIN_COUNTDOWN,0,value,old,0);
    /* Preserve the source's signed byte countdown, including wraparound. */
    if((int8_t)old>1) return;
    wr_u16(ORIGIN_STATUS_WORD,(uint16_t)(rd_u16(ORIGIN_STATUS_WORD)|2u));
    observe(h,ORIGIN_STATUS,0,rd_u16(ORIGIN_STATUS_WORD),0,0);
    publish_byte(h,ORIGIN_DETAIL_MODE,0);
}
static void adjust(const SelectorOriginHooks *h,SelectorOriginTriple delta,
                   uint32_t magnitude,unsigned shift) {
    SelectorOriginTriple normalized,old,origin;
    unsigned i;
    for(;;) {
        observe(h,ORIGIN_LONG_COMPARE,0,magnitude,0x4800u,0);
        if((int32_t)magnitude<0x4800) break;
        observe(h,ORIGIN_REDUCE,0,0,0,0);
        for(i=0;i<3;++i) delta.component[i]=arithmetic_right(delta.component[i],2);
        magnitude=arithmetic_right(magnitude,2);
    }
    observe(h,ORIGIN_NORMALIZE_SAVE,0,0,0,0);
    normalized=h->consume(h->context,ORIGIN_NORMALIZE,&delta);
    observe(h,ORIGIN_NORMALIZE_RESULT,0,0,0,0);
    for(i=0;i<3;++i) delta.component[i]=left((uint32_t)(int32_t)(int16_t)normalized.component[i],shift);
    old=load_triple(ORIGIN_SMOOTHED_DELTA);
    observe(h,ORIGIN_SMOOTH,0,0,0,0);
    if(old.component[0]|old.component[1]|old.component[2])
        for(i=0;i<3;++i) delta.component[i]=old.component[i]+arithmetic_right(delta.component[i]-old.component[i],1);
    store_triple(ORIGIN_SMOOTHED_DELTA,delta);
    origin=load_triple(SELECTOR_ORIGIN);
    observe(h,ORIGIN_ADD,0,0,0,0);
    for(i=0;i<3;++i) origin.component[i]+=delta.component[i];
    store_triple(SELECTOR_ORIGIN,origin); companion(h,origin);
}
static void adjustment_mode(const SelectorOriginHooks *h) {
    SelectorOriginTriple delta=load_triple(ORIGIN_CANDIDATE_TRIPLE),origin=load_triple(SELECTOR_ORIGIN);
    uint32_t absolute[3],magnitude;
    unsigned i,shift=0,mode=rd_u8(ORIGIN_ADJUSTMENT_MODE);
    enum { FINALIZE, BLEND, PRESET, SMALL, ALTERNATE, ADJUST } route=FINALIZE;
    for(i=0;i<3;++i) {
        delta.component[i]-=origin.component[i];
        absolute[i]=(int32_t)delta.component[i]<0 ? 0u-delta.component[i] : delta.component[i];
    }
    magnitude=absolute[0];
    if((int32_t)absolute[1]>(int32_t)magnitude) magnitude=absolute[1];
    if((int32_t)absolute[2]>(int32_t)magnitude) magnitude=absolute[2];
    observe(h,ORIGIN_DELTAS,0,0,0,0); observe(h,ORIGIN_DISPATCH,0,mode,0,0);
    switch(mode) {
    case 0:
        shift=13; if(threshold(h,magnitude,0x1c00000u,shift)) break;
        publish_byte(h,ORIGIN_THRESHOLD_FLAG,1); publish_byte(h,ORIGIN_GATE_MODE,0);
        shift=11; if(threshold(h,magnitude,0xd00000u,shift)) break;
        publish_byte(h,ORIGIN_ADJUSTMENT_MODE,1); route=BLEND; break;
    case 1:
        shift=11; if(threshold(h,magnitude,0x200000u,shift)) break;
        shift=10; if(threshold(h,magnitude,0x60000u,shift)) break;
        publish_byte(h,ORIGIN_ADJUSTMENT_MODE,2); route=BLEND; break;
    case 2:
        shift=9; route=BLEND; if(threshold(h,magnitude,0x60000u,shift)) break;
        shift=7; if(threshold(h,magnitude,0x30000u,shift)) break;
        publish_byte(h,ORIGIN_ADJUSTMENT_MODE,3); route=PRESET; break;
    case 3:
        shift=7; route=PRESET; if(threshold(h,magnitude,0x60000u,shift)) break;
        shift=6; if(threshold(h,magnitude,0x60000u,shift)) break;
        shift=5; if(threshold(h,magnitude,0xd000u,shift)) break;
        shift=3; if(threshold(h,magnitude,0x8000u,shift)) break;
        publish_byte(h,ORIGIN_ADJUSTMENT_MODE,4); route=SMALL; break;
    case 4:
        shift=2; if(threshold(h,magnitude,0x2800u,shift)) break;
        publish_byte(h,ORIGIN_ADJUSTMENT_MODE,5); route=ALTERNATE; break;
    case 5: shift=1; observe(h,ORIGIN_THRESHOLD,0,magnitude,0,shift); break;
    case 6: countdown(h); return;
    case 7:
        if((int8_t)byte_test(h,ORIGIN_AUXILIARY_FLAG)<=0) {
            shift=14; if(threshold(h,magnitude,0x800000u,shift)) { route=ADJUST; break; }
            shift=12;
            observe(h,ORIGIN_THRESHOLD,0,magnitude,0,shift);
            observe(h,ORIGIN_LONG_COMPARE,0,rd_u32(ORIGIN_AUXILIARY_DELTA),0xfff00000u,0);
            if(rd_s32(ORIGIN_AUXILIARY_DELTA)<-0x100000) {
                observe(h,ORIGIN_LONG_COMPARE,0,magnitude,0x100000u,0);
                if((int32_t)magnitude>0x100000) { route=ADJUST; break; }
            }
        }
        publish_byte(h,ORIGIN_DETAIL_MODE,3); publish_byte(h,ORIGIN_ADJUSTMENT_MODE,8);
        store_triple(ORIGIN_CANDIDATE_TRIPLE,h->consume(h->context,ORIGIN_REGENERATE,0)); return;
    case 8:
        shift=14; if(threshold(h,magnitude,0x800000u,shift)) { route=ADJUST; break; }
        shift=12; if(threshold(h,magnitude,0x100000u,shift)) { route=ADJUST; break; }
        publish_byte(h,ORIGIN_DETAIL_MODE,4); return;
    default: abort();
    }
    if(route==BLEND) blend(h);
    else if(route==PRESET) {
        observe(h,ORIGIN_PRESET_SAVE,0,0,0,0);
        load_origin_candidate_preset(h,ORIGIN_ROOT_PRESET);
    } else if(route==SMALL || route==ALTERNATE) small_candidate(h,route==ALTERNATE);
    if(route!=ADJUST) {
        observe(h,ORIGIN_LONG_COMPARE,0,magnitude,0x240u,0);
        if((int32_t)magnitude<=0x240) {
            if(byte_compare(h,ORIGIN_DETAIL_MODE,6)<0) return;
            publish_byte(h,ORIGIN_ADJUSTMENT_MODE,6); publish_byte(h,ORIGIN_DETAIL_COUNTER,5);
            publish_byte(h,ORIGIN_THRESHOLD_FLAG,0); countdown(h); return;
        }
    }
    adjust(h,delta,magnitude,shift);
}
void publish_selector_origin(const SelectorOriginHooks *h) {
    SelectorOriginTriple v;
    gaddr record,table;
    uint8_t type,index; int8_t enable;
    unsigned i;
    if(!h || !h->consume) abort();
    h->consume(h->context,ORIGIN_PREPARE,0);
    if(!byte_test(h,ORIGIN_ENABLE) || !byte_test(h,ORIGIN_GATE_B) || byte_test(h,ORIGIN_GATE_A)) return;
    if(byte_test(h,ORIGIN_GATE_MODE) && !byte_test(h,ORIGIN_DETAIL_MODE)) {
        record=CONTROL_RECORDS+(gaddr)(int32_t)rd_s16(ORIGIN_RECORD_OFFSET);
        v=load_triple(SELECTOR_ORIGIN); v.component[0]=rd_u32(record+0x14u); v.component[2]=rd_u32(record+0x1cu);
        observe(h,ORIGIN_DIRECT,record,0,0,0);
        store_triple(SELECTOR_ORIGIN,v); companion(h,v); return;
    }
    if(byte_compare(h,ORIGIN_DETAIL_MODE,5)!=0 && byte_compare(h,ORIGIN_DETAIL_MODE,2)>=0) { adjustment_mode(h); return; }
    record=CONTROL_RECORDS+(gaddr)(int32_t)rd_s16(ORIGIN_RECORD_OFFSET);
    type=rd_u8(record+0x62u); index=rd_u8(ORIGIN_DETAIL_INDEX);
    table=ORIGIN_MATRIX_CLASS_30;
    if((type&0xf0u)!=0x30u) {
        int16_t angle=rd_s16(record+0x68u);
        int16_t previous=rd_s16(ORIGIN_ANGLE_HISTORY);
        int16_t change=(int16_t)((uint16_t)previous-(uint16_t)angle);
        if(previous<angle) change=(int16_t)(0u-(uint16_t)change);
        observe(h,ORIGIN_ANGLE,record,(uint16_t)angle,rd_u16(ORIGIN_ANGLE_HISTORY),index);
        wr_u16(ORIGIN_ANGLE_HISTORY,(uint16_t)angle);
        if(change>=0x1c20 && change<=0x5460 && index && (int8_t)index<8 && index!=4) {
            index=(uint8_t)(index+4u); if((int8_t)index>=8) index=(uint8_t)(index-8u);
            wr_u8(ORIGIN_DETAIL_INDEX,index);
        }
        table=type==0x11 ? ORIGIN_MATRIX_CLASS_11 : type==0x14 ? ORIGIN_MATRIX_CLASS_14 : ORIGIN_MATRIX_TABLE;
    } else observe(h,ORIGIN_ANGLE,record,0,0,index);
    enable=rd_s8(ORIGIN_ENABLE);
    /* Six-byte rows and original byte/word wrap precede the signed lookup. */
    for(i=0;i<3;++i) {
        uint16_t word=rd_u16(table+(gaddr)(int32_t)(int16_t)((int8_t)index*6)+2*i);
        if(enable>=0) {
            if(enable<=1) word=(uint16_t)(word+(uint16_t)((int16_t)word>>1));
            else { unsigned shift=(unsigned)(enable-1)&63u; word=shift<16 ? (uint16_t)(word<<shift) : 0; }
        }
        v.component[i]=(uint32_t)(int32_t)(int16_t)word;
    }
    observe(h,ORIGIN_MATRIX_INPUTS,table,index,(uint8_t)enable,0);
    observe(h,ORIGIN_MATRIX_SAVE,record,type&0xf0u,index,0);
    v=h->consume(h->context,(type&0xf0u)==0x30u || (index!=0 && index!=4) ? ORIGIN_MATRIX_A : ORIGIN_MATRIX_B,&v);
    store_triple(ORIGIN_CANDIDATE_TRIPLE,v); observe(h,ORIGIN_MATRIX_RESULT,record,0,0,0);
    observe(h,ORIGIN_FLOOR,record,0,0,0);
    {
        int16_t floor=(int16_t)(((rd_u8(record+4u)&0xc0u)?rd_u16(record+0x4eu):0)+7u);
        uint32_t minimum=(uint32_t)(int32_t)floor<<8;
        if((int32_t)v.component[1]<(int32_t)minimum) v.component[1]=minimum;
    }
    store_triple(SELECTOR_ORIGIN,v); companion(h,v);
}
void select_origin_control_record(const SelectorOriginHooks *h) {
    SelectorOriginTriple v;
    gaddr list=rd_u32(ORIGIN_RECORD_LIST),base=CONTROL_RECORDS;
    uint16_t selected=0;
    publish_byte(h,ORIGIN_AUXILIARY_FLAG,1); publish_word(h,ORIGIN_SELECTED_OFFSET,0); publish_byte(h,ORIGIN_ADJUSTMENT_MODE,7);
    observe(h,ORIGIN_SCAN_START,list,0,0,0);
    for(;;) {
        uint16_t offset; gaddr record;
        observe(h,ORIGIN_SCAN_TEST,list,rd_u16(list),0,0);
        if(rd_s16(list)<0) break;
        offset=(uint16_t)(rd_u16(list+4u)<<9); list+=10;
        record=base+(gaddr)(int32_t)(int16_t)offset;
        observe(h,ORIGIN_SCAN_RECORD,list,offset,rd_u8(record+0x62u),0);
        if((rd_u8(record+0x62u)&0xf0u)!=0x10u) continue;
        observe(h,ORIGIN_SCAN_ACTIVE,record,rd_u8(record+1u),0,0);
        if(!(rd_u8(record+1u)&0x40u)) continue;
        selected=offset; wr_u16(ORIGIN_SELECTED_OFFSET,selected);
        observe(h,ORIGIN_SCAN_SELECTED,record,selected,0,0);
        if(!(rd_u8(record+1u)&8u)) goto selected_record;
    }
    observe(h,ORIGIN_SCAN_FINISH,0,selected,0,0);
    if(!selected) {
        publish_word(h,ORIGIN_FALLBACK_WORD,0x3e);
        h->consume(h->context,ORIGIN_FALLBACK,0);
        v=h->consume(h->context,ORIGIN_REGENERATE,0);
        store_triple(ORIGIN_CANDIDATE_TRIPLE,v); return;
    }
selected_record:
    publish_byte(h,ORIGIN_AUXILIARY_FLAG,0xff);
    v.component[0]=rd_u32(base+(gaddr)(int32_t)(int16_t)selected+0x14u);
    v.component[2]=rd_u32(base+(gaddr)(int32_t)(int16_t)selected+0x1cu);
    v.component[1]=rd_u32(SELECTOR_ORIGIN_MIDDLE);
    observe(h,ORIGIN_SCAN_RESULT,0,selected,0,0);
    v.component[1]-=arithmetic_right(v.component[1],2);
    store_triple(ORIGIN_CANDIDATE_TRIPLE,v);
}
