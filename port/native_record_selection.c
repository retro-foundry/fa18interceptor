#include "native_record_selection.h"

static int byte_at(const FA18NativeSceneRecord *record,size_t offset,uint8_t *value) {
    return value && fa18_read_native_scene_record(record,offset,value,1);
}
static int shared(const FA18NativeRecordSelection *s) {
    return s && s->records && s->selected_record && s->selection_marker &&
        s->action_pending && s->selection_active && s->origin_enable &&
        s->action_first && s->action_second && s->action_third && s->pair_override;
}
static FA18NativeSceneRecord *selected(FA18NativeRecordSelection *s,uint16_t offset) {
    unsigned slot;
    if(offset%FA18_NATIVE_SCENE_RECORD_BYTES) return 0;
    slot=offset/FA18_NATIVE_SCENE_RECORD_BYTES;
    return slot<FA18_NATIVE_SCENE_RECORDS?s->records->records+slot:0;
}
static int action_child(FA18NativeRecordSelection *s,FA18NativeRecordActionChild child) {
    return s->ops && s->ops->consume && s->ops->consume(s->ops->context,s,child);
}
int fa18_release_lost_native_selection(FA18NativeRecordSelection *s) {
    FA18NativeSceneRecord *record; uint16_t offset;
    if(!shared(s)) return 0;
    offset=*s->selected_record;
    if((int16_t)offset<0) return 1;
    record=selected(s,offset); if(!record) return 0;
    if((record->aircraft->flags&0x40u) && !(record->byte_20&2u)) return 1;
    *s->selected_record=0xffff; *s->selection_active=0; *s->selection_marker=0xffff;
    return 1;
}
int fa18_select_native_record_action(FA18NativeRecordSelection *s,int allow_release,int *decision) {
    FA18NativeSceneRecord *root; uint8_t raw,code;
    if(!shared(s) || !decision) return 0;
    *decision=0;
    if(*s->action_pending) return 1;
    root=s->records->records; raw=root->byte_7c; code=(uint8_t)(raw&15u);
    if(allow_release && (int8_t)code>=14) {
        root->byte_7c=(uint8_t)(raw&0xf0u);
        if(!action_child(s,FA18_RECORD_ACTION_RELEASE)) return 0;
        *decision=1; return 1;
    }
    if(code==9 && !*s->origin_enable) {
        return action_child(s,FA18_RECORD_ACTION_SOUND);
    }
    if(code==1) {
        *s->action_first=0x3f; *s->action_second=0x78; *s->action_third=0xff;
        return 1;
    }
    if(code==3) {
        if(!action_child(s,FA18_RECORD_ACTION_MANOEUVRE)) return 0;
        *decision=1;
    }
    return 1;
}
int fa18_native_paired_record_ready(FA18NativeRecordSelection *s,
                                    unsigned companion_slot,int *decision) {
    FA18NativeSceneRecord *record,*other; uint8_t partner,state_byte;
    unsigned partner_slot;
    if(!shared(s) || !decision || companion_slot>=FA18_NATIVE_SCENE_RECORDS) return 0;
    *decision=0; record=s->records->records+companion_slot;
    if(record->aircraft->flags&0x8700u) return 1;
    if(record->byte_20&2u) return 1;
    if(*s->pair_override) goto ready;
    if(!(record->aircraft->flags&0x40u)) return 1;
    if(!byte_at(record,0x38,&partner)) return 0;
    if((int8_t)partner<0) {
        partner_slot=partner&0x7fu;
        if(partner_slot>=FA18_NATIVE_SCENE_RECORDS) return 0;
        other=s->records->records+partner_slot;
        if(!(other->aircraft->flags&0x40u) || (other->byte_20&2u) ||
           (other->aircraft->flags&1u)) return 1;
    }
    if(!byte_at(record,0x64,&state_byte)) return 0;
    if((state_byte&0x60u)!=0x60u || !(record->aircraft->flags&1u)) return 1;
    state_byte=(uint8_t)(record->aircraft->weapon_radar&0xf0u);
    if(state_byte!=0x20u && state_byte!=0x30u) return 1;
ready:
    *s->pair_override=0; *decision=1; return 1;
}
