#include "native_context_refresh.h"

#include <limits.h>

static int32_t signed_long(uint32_t value) {
    return value<=INT32_MAX?(int32_t)value:(int32_t)((int64_t)value-INT64_C(0x100000000));
}
static int16_t asr_word(int16_t value,unsigned count) {
    if(value>=0) return (int16_t)(value/(1<<count));
    return (int16_t)-((-(int32_t)value+((1<<count)-1))/(1<<count));
}
static int call(FA18NativeContextRefresh *s,FA18NativeContextRefreshChild child) {
    return s->ops->consume(s->ops->context,s,child);
}
static void flag_records(FA18NativeSceneRecords *records) {
    unsigned slot;
    for(slot=0;slot<FA18_NATIVE_SCENE_RECORDS;++slot) records->aircraft[slot].flags|=0x10;
    for(slot=0;slot<FA18_NATIVE_SCENE_RECORDS;++slot) records->work[slot][1]|=0x10;
}
static FA18NativeSceneRecord *viewed_record(FA18NativeContextRefresh *s) {
    unsigned slot;
    for(slot=0;slot<FA18_NATIVE_SCENE_RECORDS;++slot)
        if(s->view->flight->viewed==s->records->aircraft+slot) return s->records->records+slot;
    return 0;
}
static int templates(FA18NativeContextRefresh *s,unsigned bit) {
    s->view->update_mask&=(uint8_t)~(1u<<bit);
    if(bit==1) *s->alternate=1;
    return call(s,FA18_CONTEXT_REFRESH_TEMPLATES);
}
static int shared(const FA18NativeContextRefresh *s) {
    return s && s->records && s->view && s->view->flight &&
        s->records->input==s->view->flight->commands &&
        s->view->flight->player==s->records->aircraft && s->ops && s->ops->consume &&
        s->position_bias && s->origin && s->cell_timer && s->error_word &&
        s->condition_key_a && s->condition_key_b && s->stage_selector && s->current_colour &&
        s->line_style && s->context_selection && s->prepared && s->alternate && s->cell_checks &&
        s->view_mode && s->fixed_readouts && s->frame_gate;
}
int fa18_refresh_native_context(FA18NativeContextRefresh *s) {
    FA18NativeSceneRecord *record; uint8_t requests,kind; int16_t x,z;
    if(!shared(s)) return 0;
    if(*s->position_bias<signed_long(UINT32_C(0xf8000000))) return 1;
    *s->frame_gate=0; requests=s->view->update_mask;
    if(requests) {
        *s->cell_timer=0x49; flag_records(s->records); kind=(uint8_t)(requests&15u);
        if(kind!=0x0c && kind!=0x0b && kind!=0x0f) *s->error_word=0x27;
        if(*s->context_selection) {
            x=(int16_t)(((uint32_t)s->origin[0]&UINT32_C(0x1fffffff))>>24);
            z=(int16_t)(((uint32_t)s->origin[2]&UINT32_C(0x1fffffff))>>24);
        } else {
            record=viewed_record(s); if(!record) return 0;
            x=asr_word((int16_t)record->word_06,2);
            z=asr_word((int16_t)record->word_08,2);
        }
        *s->condition_key_a=(uint16_t)x; *s->condition_key_b=(uint16_t)z;
        *s->prepared=1; *s->cell_checks=0; *s->alternate=0;
        if((requests&1u) && !templates(s,0)) return 0;
        if((s->view->update_mask&2u) && !templates(s,1)) return 0;
        *s->cell_checks=1;
        if(s->view->update_mask&4u) s->view->update_mask&=(uint8_t)~4u;
        if(s->view->update_mask&8u) {
            if(!templates(s,3)) return 0;
            *s->cell_timer=0x4a;
        }
        *s->frame_gate=1; s->view->update_mask=0;
    }
    if(!call(s,FA18_CONTEXT_REFRESH_SORT)) return 0;
    *s->cell_timer=0x4b; *s->frame_gate=0;
    if(!call(s,FA18_CONTEXT_REFRESH_CACHE)) return 0;
    *s->cell_timer=0x4c;
    if(*s->stage_selector&1u) {
        if(!call(s,FA18_CONTEXT_REFRESH_CONDITION_B)) return 0;
        *s->cell_timer=0x4e;
    } else {
        if(!call(s,FA18_CONTEXT_REFRESH_CONDITION_A)) return 0;
        *s->cell_timer=0x4d;
    }
    if(!*s->view_mode && *s->fixed_readouts) {
        *s->current_colour=*s->prepared?15:6;
        if(*s->prepared) *s->line_style=0x000fffff;
        if(!call(s,FA18_CONTEXT_REFRESH_RENDER)) return 0;
    }
    *s->prepared=0; return 1;
}
