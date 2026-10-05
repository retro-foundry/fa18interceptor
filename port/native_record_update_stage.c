#include "native_record_update_stage.h"

#include <limits.h>

static int16_t signed_word(uint16_t value) {
    return value<=INT16_MAX?(int16_t)value:(int16_t)((int32_t)value-0x10000);
}
static int32_t signed_long(uint32_t value) {
    return value<=INT32_MAX?(int32_t)value:(int32_t)((int64_t)value-INT64_C(0x100000000));
}
static int16_t asr_word(int16_t value,unsigned count) {
    if(value>=0) return (int16_t)(value/(1<<count));
    return (int16_t)-((-(int32_t)value+((1<<count)-1))/(1<<count));
}
static int16_t magnitude(int16_t value) {
    return value<0?signed_word((uint16_t)(0u-(uint16_t)value)):value;
}
static uint8_t reverse_cell(uint16_t x,uint16_t z) {
    return (uint8_t)((3u-(x&3u))+4u*(3u-(z&3u)));
}
static FA18NativeSceneRecord *viewed_record(FA18NativeRecordUpdateStage *s) {
    unsigned i;
    if(!s || !s->view || !s->view->flight) return 0;
    for(i=0;i<FA18_NATIVE_SCENE_RECORDS;++i)
        if(s->view->flight->viewed==s->records->aircraft+i) return s->records->records+i;
    return 0;
}
static int classify_rate(FA18NativeSceneRecord *record,uint8_t *rate) {
    uint8_t bytes[0x6e]; int16_t a,b,c,largest,secondary;
    if(!rate || !fa18_read_native_scene_record(record,0,bytes,sizeof bytes)) return 0;
    a=magnitude(signed_word((uint16_t)(((unsigned)bytes[0x56]<<8)|bytes[0x57])));
    b=magnitude(signed_word((uint16_t)(((unsigned)bytes[0x58]<<8)|bytes[0x59])));
    c=asr_word(magnitude(signed_word((uint16_t)(((unsigned)bytes[0x5a]<<8)|bytes[0x5b]))),2);
    largest=b>a?(b>c?b:c):(c>a?c:a);
    if(largest<=0x60) {
        secondary=magnitude(signed_word((uint16_t)(((unsigned)bytes[0x6c]<<8)|bytes[0x6d])));
        *rate=secondary>0x1000?3:5;
    } else *rate=largest<=0xc0?3:1;
    return 1;
}
static int shared(const FA18NativeRecordUpdateStage *s) {
    return s && s->records && s->view && s->view->flight && s->view->flight->commands &&
        s->records->input==s->view->flight->commands &&
        s->view->flight->player==s->records->aircraft && s->control_records &&
        s->control_records->records==s->records && s->origin_update &&
        s->origin_update->records==s->records && s->origin_update->origin==s->origin &&
        s->input_byte && s->input_byte_mirror && s->change_inhibit && s->context_selection &&
        s->origin_detail_mode && s->selector_byte_coarse && s->selector_byte_fine && s->record_rate &&
        s->position_bias && s->long_mirror && s->projection_depth && s->origin && s->scaled_word &&
        s->selector_word_x && s->selector_word_z;
}
int fa18_update_native_scene_records(FA18NativeRecordUpdateStage *s) {
    FA18NativeSceneRecord *record; uint8_t requests=0,first,second;
    uint32_t negated,x,z; int32_t previous; uint16_t coarse;
    int16_t coarse_x,coarse_z;
    if(!shared(s)) return 0;
    if(*s->input_byte!=*s->input_byte_mirror) {
        *s->input_byte_mirror=*s->input_byte;
        if(!*s->change_inhibit) requests|=0x0b;
    }
    negated=0u-(uint32_t)*s->position_bias; previous=*s->long_mirror;
    if((signed_long(negated)>=0xa000)!=(previous>=0xa000)) requests|=0x0b;
    *s->long_mirror=signed_long(negated);
    /* CLR.W, SWAP, ASR.L #5: the original high word becomes a zero-extended key. */
    coarse=(uint16_t)((negated>>16)>>5);
    if(coarse!=*s->scaled_word) {
        if(*s->origin_detail_mode!=2) requests|=0x0b;
        *s->scaled_word=coarse;
    }
    if(!fa18_update_native_control_records(s->control_records)) return 0;

    if(!*s->context_selection) {
        record=viewed_record(s);
        if(!record || !classify_rate(record,s->record_rate)) return 0;
        *s->selector_word_x=record->word_06; *s->selector_word_z=record->word_08;
        *s->selector_byte_fine=record->byte_0a;
        *s->selector_byte_coarse=reverse_cell(record->word_06,record->word_08);
    } else {
        if(!fa18_update_native_selector_origin(s->origin_update)) return 0;
        record=s->records->records;
        if(!classify_rate(record,s->record_rate)) return 0;
        x=(uint32_t)s->origin[0]&UINT32_C(0x1fffffff);
        z=(uint32_t)s->origin[2]&UINT32_C(0x1fffffff);
        coarse_x=asr_word((int16_t)(x>>16),4); coarse_z=asr_word((int16_t)(z>>16),4);
        first=reverse_cell((uint16_t)coarse_x,(uint16_t)coarse_z);
        coarse_x=asr_word(coarse_x,2); coarse_z=asr_word(coarse_z,2);
        second=reverse_cell((uint16_t)coarse_x,(uint16_t)coarse_z);
        if(first!=*s->selector_byte_fine) {
            if(*s->origin_detail_mode!=2 || *s->projection_depth>-0x1000) requests=0xff;
            *s->selector_byte_fine=first;
        }
        if(second!=*s->selector_byte_coarse) {
            if(*s->origin_detail_mode!=2 || *s->projection_depth>-0x1000) requests=0xff;
            *s->selector_byte_coarse=second;
        }
        if((uint16_t)coarse_x!=*s->selector_word_x) { requests=0xff; *s->selector_word_x=(uint16_t)coarse_x; }
        if((uint16_t)coarse_z!=*s->selector_word_z) { requests=0xff; *s->selector_word_z=(uint16_t)coarse_z; }
    }
    s->view->update_mask|=requests;
    return 1;
}
