#include "native_selector_origin.h"

#include <limits.h>

static int32_t signed_long(uint32_t value) {
    return value<=INT32_MAX?(int32_t)value:(int32_t)((int64_t)value-INT64_C(0x100000000));
}
static int16_t signed_word(uint16_t value) {
    return value<=INT16_MAX?(int16_t)value:(int16_t)((int32_t)value-0x10000);
}
static int32_t add_long(int32_t a,int32_t b) {
    return signed_long((uint32_t)a+(uint32_t)b);
}
static int32_t sub_long(int32_t a,int32_t b) {
    return signed_long((uint32_t)a-(uint32_t)b);
}
static int32_t asr_long(int32_t value,unsigned count) {
    int64_t divisor;
    if(count>=32) return value<0?-1:0;
    divisor=(int64_t)(UINT64_C(1)<<count);
    if(value>=0) return (int32_t)((int64_t)value/divisor);
    return (int32_t)-((-(int64_t)value+(divisor-1))/divisor);
}
static int16_t asr_word(int16_t value,unsigned count) {
    if(count>=16) return value<0?-1:0;
    if(value>=0) return (int16_t)(value/(1<<count));
    return (int16_t)-((-(int32_t)value+((1<<count)-1))/(1<<count));
}
static int32_t shift_long(int32_t value,unsigned count) {
    count&=63u; return count<32?signed_long((uint32_t)value<<count):0;
}
static int16_t shift_word(int16_t value,unsigned count) {
    count&=63u; return count<16?signed_word((uint16_t)((uint16_t)value<<count)):0;
}
static uint32_t magnitude(int32_t value) {
    return value<0?0u-(uint32_t)value:(uint32_t)value;
}
static int record_word(const FA18NativeSceneRecord *record,size_t offset,uint16_t *value) {
    uint8_t bytes[2];
    if(!value || !fa18_read_native_scene_record(record,offset,bytes,2)) return 0;
    *value=(uint16_t)(((unsigned)bytes[0]<<8)|bytes[1]); return 1;
}
static int shared(const FA18NativeSelectorOrigin *s) {
    return s && s->records && s->active_record && *s->active_record && s->ops && s->ops->consume &&
        s->tables && s->root_preset && s->origin && s->candidate && s->smoothed_delta &&
        s->negated_companion && s->auxiliary_delta && s->angle_history && s->status_word &&
        s->enable && s->gate_b && s->gate_a && s->gate_mode && s->detail_mode && s->detail_index &&
        s->adjustment_mode && s->threshold_flag && s->auxiliary_flag && s->variant_selector &&
        s->detail_counter;
}
static int child(FA18NativeSelectorOrigin *s,FA18NativeSelectorOriginChild which,
                 const int32_t input[3],int32_t output[3]) {
    return s->ops->consume(s->ops->context,s,which,input,output);
}
static int owns_record(const FA18NativeSelectorOrigin *s,const FA18NativeSceneRecord *record) {
    unsigned slot;
    for(slot=0;slot<FA18_NATIVE_SCENE_RECORDS;++slot)
        if(record==s->records->records+slot) return 1;
    return 0;
}
static void companion(FA18NativeSelectorOrigin *s) {
    unsigned i;
    for(i=0;i<3;++i) {
        uint32_t value=(uint32_t)s->origin[i];
        if(i!=1) value&=UINT32_C(0x003fffff);
        s->negated_companion[i]=signed_long(0u-value);
    }
}
static void blend(FA18NativeSelectorOrigin *s) {
    int32_t first[3],second[3]; unsigned i;
    for(i=0;i<3;++i) first[i]=asr_long(add_long(s->origin[i],
        signed_long((*s->active_record)->geometry->position[i])),1);
    for(i=0;i<3;++i) second[i]=asr_long(add_long(first[i],i==1?0:s->origin[i]),1);
    for(i=0;i<3;++i) second[i]=asr_long(add_long(second[i],first[i]),1);
    for(i=0;i<3;++i) first[i]=asr_long(add_long(second[i],first[i]),1);
    for(i=0;i<3;++i) s->candidate[i]=asr_long(add_long(first[i],second[i]),1);
}
static int small_candidate(FA18NativeSelectorOrigin *s,int alternate) {
    int32_t input[3],result[3];
    int positive=*s->variant_selector==1 || *s->variant_selector==2;
    input[0]=positive?6:-6; input[1]=alternate?1:4;
    input[2]=alternate?6:positive?3:-9;
    if(!child(s,FA18_SELECTOR_ORIGIN_MATRIX_B,input,result)) return 0;
    s->candidate[0]=result[0]; s->candidate[1]=result[1]; s->candidate[2]=result[2]; return 1;
}
static void countdown(FA18NativeSelectorOrigin *s) {
    uint8_t old=*s->detail_counter;
    *s->detail_counter=(uint8_t)(old-1u);
    if((int8_t)old>1) return;
    *s->status_word=(uint16_t)(*s->status_word|2u); *s->detail_mode=0;
}
static int adjust(FA18NativeSelectorOrigin *s,int32_t delta[3],uint32_t amount,unsigned shift) {
    int32_t normalized[3]; unsigned i;
    while((int32_t)amount>=0x4800) {
        for(i=0;i<3;++i) delta[i]=asr_long(delta[i],2);
        amount=(uint32_t)asr_long((int32_t)amount,2);
    }
    if(!child(s,FA18_SELECTOR_ORIGIN_NORMALIZE,delta,normalized)) return 0;
    for(i=0;i<3;++i) delta[i]=shift_long(signed_word((uint16_t)normalized[i]),shift);
    if(s->smoothed_delta[0] || s->smoothed_delta[1] || s->smoothed_delta[2])
        for(i=0;i<3;++i) delta[i]=add_long(s->smoothed_delta[i],
            asr_long(sub_long(delta[i],s->smoothed_delta[i]),1));
    for(i=0;i<3;++i) {
        s->smoothed_delta[i]=delta[i]; s->origin[i]=add_long(s->origin[i],delta[i]);
    }
    companion(s); return 1;
}
static int adjustment_mode(FA18NativeSelectorOrigin *s) {
    enum { FINALIZE, BLEND, PRESET, SMALL, ALTERNATE, ADJUST } route=FINALIZE;
    int32_t delta[3],regenerated[3]; uint32_t amount,absolute[3]; unsigned i,shift=0;
    for(i=0;i<3;++i) { delta[i]=sub_long(s->candidate[i],s->origin[i]); absolute[i]=magnitude(delta[i]); }
    amount=absolute[0];
    if((int32_t)absolute[1]>(int32_t)amount) amount=absolute[1];
    if((int32_t)absolute[2]>(int32_t)amount) amount=absolute[2];
    switch(*s->adjustment_mode) {
    case 0:
        shift=13; if((int32_t)amount>0x1c00000) break;
        *s->threshold_flag=1; *s->gate_mode=0;
        shift=11; if((int32_t)amount>0xd00000) break;
        *s->adjustment_mode=1; route=BLEND; break;
    case 1:
        shift=11; if((int32_t)amount>0x200000) break;
        shift=10; if((int32_t)amount>0x60000) break;
        *s->adjustment_mode=2; route=BLEND; break;
    case 2:
        shift=9; route=BLEND; if((int32_t)amount>0x60000) break;
        shift=7; if((int32_t)amount>0x30000) break;
        *s->adjustment_mode=3; route=PRESET; break;
    case 3:
        shift=7; route=PRESET; if((int32_t)amount>0x60000) break;
        shift=6; if((int32_t)amount>0x60000) break;
        shift=5; if((int32_t)amount>0xd000) break;
        shift=3; if((int32_t)amount>0x8000) break;
        *s->adjustment_mode=4; route=SMALL; break;
    case 4:
        shift=2; if((int32_t)amount>0x2800) break;
        *s->adjustment_mode=5; route=ALTERNATE; break;
    case 5: shift=1; break;
    case 6: countdown(s); return 1;
    case 7:
        if((int8_t)*s->auxiliary_flag<=0) {
            shift=14; if((int32_t)amount>0x800000) { route=ADJUST; break; }
            shift=12;
            if(*s->auxiliary_delta<-0x100000 && (int32_t)amount>0x100000) { route=ADJUST; break; }
        }
        *s->detail_mode=3; *s->adjustment_mode=8;
        if(!child(s,FA18_SELECTOR_ORIGIN_REGENERATE,0,regenerated)) return 0;
        for(i=0;i<3;++i) s->candidate[i]=regenerated[i];
        return 1;
    case 8:
        shift=14; if((int32_t)amount>0x800000) { route=ADJUST; break; }
        shift=12; if((int32_t)amount>0x100000) { route=ADJUST; break; }
        *s->detail_mode=4; return 1;
    default: return 0;
    }
    if(route==BLEND) blend(s);
    else if(route==PRESET) for(i=0;i<3;++i) s->candidate[i]=s->root_preset[i];
    else if((route==SMALL || route==ALTERNATE) && !small_candidate(s,route==ALTERNATE)) return 0;
    if(route!=ADJUST && (int32_t)amount<=0x240) {
        if((int8_t)*s->detail_mode<6) return 1;
        *s->adjustment_mode=6; *s->detail_counter=5; *s->threshold_flag=0; countdown(s); return 1;
    }
    return adjust(s,delta,amount,shift);
}
static const PortFieldWindow *matrix_table(const FA18NativeSelectorOrigin *s,uint8_t type) {
    if((type&0xf0u)==0x30u) return &s->tables->class_30;
    if(type==0x11) return &s->tables->class_11;
    if(type==0x14) return &s->tables->class_14;
    return &s->tables->general;
}
int fa18_update_native_selector_origin(FA18NativeSelectorOrigin *s) {
    FA18NativeSceneRecord *record; const PortFieldWindow *table;
    int32_t input[3],output[3]; int16_t word,angle,previous,change,floor;
    uint16_t floor_word; uint8_t type,index; int8_t enable; unsigned i;
    if(!shared(s) || !child(s,FA18_SELECTOR_ORIGIN_PREPARE,0,0) ||
       !(record=*s->active_record) || !owns_record(s,record)) return 0;
    if(!*s->enable || !*s->gate_b || *s->gate_a) return 1;
    if(*s->gate_mode && !*s->detail_mode) {
        s->origin[0]=signed_long(record->geometry->position[0]);
        s->origin[2]=signed_long(record->geometry->position[2]); companion(s); return 1;
    }
    if((int8_t)*s->detail_mode!=5 && (int8_t)*s->detail_mode>=2) return adjustment_mode(s);
    type=record->aircraft->equipment_kind; index=*s->detail_index; table=matrix_table(s,type);
    if((type&0xf0u)!=0x30u) {
        angle=signed_word(record->geometry->angle); previous=signed_word(*s->angle_history);
        change=signed_word((uint16_t)previous-(uint16_t)angle);
        if(previous<angle) change=signed_word((uint16_t)(0u-(uint16_t)change));
        *s->angle_history=(uint16_t)angle;
        if(change>=0x1c20 && change<=0x5460 && index && (int8_t)index<8 && index!=4) {
            index=(uint8_t)(index+4u); if((int8_t)index>=8) index=(uint8_t)(index-8u);
            *s->detail_index=index;
        }
        table=matrix_table(s,type);
    }
    enable=(int8_t)*s->enable;
    for(i=0;i<3;++i) {
        if(!port_field_window_s16(table,(int32_t)(int8_t)index*6+(int32_t)(2*i),&word)) return 0;
        if(enable>=0) {
            if(enable<=1) word=signed_word((uint16_t)word+(uint16_t)asr_word(word,1));
            else word=shift_word(word,(unsigned)(enable-1));
        }
        input[i]=word;
    }
    if(!child(s,(type&0xf0u)==0x30u || (index!=0 && index!=4)?
              FA18_SELECTOR_ORIGIN_MATRIX_A:FA18_SELECTOR_ORIGIN_MATRIX_B,input,output)) return 0;
    for(i=0;i<3;++i) s->candidate[i]=output[i];
    if(!record_word(record,0x4e,&floor_word)) return 0;
    floor=signed_word((uint16_t)(((record->byte_04&0xc0u)?floor_word:0)+7u));
    input[1]=shift_long(floor,8);
    if(output[1]<input[1]) output[1]=input[1];
    for(i=0;i<3;++i) s->origin[i]=output[i];
    companion(s); return 1;
}
