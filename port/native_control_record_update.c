#include "native_control_record_update.h"

static int child(FA18NativeControlRecordUpdate *s,FA18NativeControlRecordChild kind,
                 unsigned slot,int *decision) {
    int ignored=0;
    return s->ops->consume(s->ops->context,s,kind,slot,s->companion_slot,
        decision?decision:&ignored);
}
static int active(const FA18NativeControlRecordUpdate *s,unsigned slot) {
    return (s->records->aircraft[slot].flags&0x40u)!=0;
}
static void prepare(FA18NativeControlRecordUpdate *s,unsigned slot,unsigned companion) {
    *s->current_slot=(uint16_t)slot; *s->current_stride=(uint16_t)(slot*512u);
    s->companion_slot=companion;
}
static int dispatch_pose(FA18NativeControlRecordUpdate *s,unsigned slot) {
    int decision;
    return child(s,FA18_RECORD_UPDATE_DISPATCH,slot,&decision) &&
        (!decision || child(s,FA18_RECORD_UPDATE_POSE,slot,0));
}
static int group_record(FA18NativeControlRecordUpdate *s,unsigned slot,unsigned companion,uint8_t gate,
                        int allow_release,FA18NativeControlRecordChild place) {
    int decision;
    prepare(s,slot,companion);
    if(!active(s,slot)) {
        if(gate) { if(!child(s,place,slot,0)) return 0; }
        else {
            if(!fa18_select_native_record_action(s->selection,allow_release,&decision)) return 0;
            if(!decision) return 1;
        }
    }
    return dispatch_pose(s,slot);
}
static int active_record(FA18NativeControlRecordUpdate *s,unsigned slot,int force) {
    if(!active(s,slot)) return 1;
    if(force) s->records->aircraft[slot].flags|=4;
    prepare(s,slot,s->companion_slot);
    if(slot==4 && !child(s,FA18_RECORD_UPDATE_SECONDARY_CONTROL,slot,0)) return 0;
    return dispatch_pose(s,slot);
}
static int paired_record(FA18NativeControlRecordUpdate *s,unsigned slot,unsigned companion) {
    int decision;
    prepare(s,slot,companion);
    if(!active(s,slot)) {
        if(!fa18_native_paired_record_ready(s->selection,companion,&decision)) return 0;
        if(!decision) return 1;
        if(!child(s,FA18_RECORD_UPDATE_SECONDARY_PLACE,slot,0)) return 0;
    }
    return dispatch_pose(s,slot);
}
static uint16_t work_word(const FA18NativeSceneRecords *records,unsigned slot) {
    return (uint16_t)(((unsigned)records->work[slot][4]<<8)|records->work[slot][5]);
}
static void set_work_word(FA18NativeSceneRecords *records,unsigned slot,uint16_t value) {
    records->work[slot][4]=(uint8_t)(value>>8); records->work[slot][5]=(uint8_t)value;
}
static int record_word(FA18NativeSceneRecord *record,size_t offset,uint16_t *value) {
    uint8_t bytes[2];
    if(!value || !fa18_read_native_scene_record(record,offset,bytes,2)) return 0;
    *value=(uint16_t)(((unsigned)bytes[0]<<8)|bytes[1]); return 1;
}
static int set_record_word(FA18NativeSceneRecord *record,size_t offset,uint16_t value) {
    uint8_t bytes[2]={(uint8_t)(value>>8),(uint8_t)value};
    return fa18_write_native_scene_record(record,offset,bytes,2);
}
int fa18_update_native_control_records(FA18NativeControlRecordUpdate *s) {
    FA18NativeSceneRecord *root; uint16_t countdown; unsigned slot;
    if(!s || !s->records || !s->selection || s->selection->records!=s->records ||
       !s->ops || !s->ops->consume || !s->post_input_event ||
       !s->counter_first || !s->counter_second || !s->primary_gate || !s->secondary_gate ||
       !s->periodic_word || !s->current_slot || !s->current_stride) return 0;
    root=s->records->records;
    if(!*s->post_input_event)
        for(slot=0;slot<FA18_NATIVE_SCENE_RECORDS;++slot)
            set_work_word(s->records,slot,(uint16_t)(work_word(s->records,slot)-1u));
    if((*s->periodic_word&15u)==3u && !child(s,FA18_RECORD_UPDATE_PERIODIC,0,0)) return 0;
    for(slot=0;slot<15;++slot) s->records->aircraft[slot].secondary_flags&=0xfffe;
    if(!*s->post_input_event) {
        if((int8_t)*s->counter_first>0) --*s->counter_first;
        if((int8_t)*s->counter_second>0) --*s->counter_second;
    }
    if(!fa18_release_lost_native_selection(s->selection)) return 0;
    prepare(s,0,4);
    if(!record_word(root,0x4c,&countdown)) return 0;
    if(!*s->post_input_event && !set_record_word(root,0x4c,(uint16_t)(countdown-1u))) return 0;
    root->aircraft->flags&=0xfffd;
    if(!child(s,FA18_RECORD_UPDATE_ROOT_CONTROL,0,0) ||
       !child(s,FA18_RECORD_UPDATE_ROOT_VIEW,0,0) ||
       !child(s,FA18_RECORD_UPDATE_ROOT_MARKER,0,0) ||
       !child(s,FA18_RECORD_UPDATE_POSE,0,0)) return 0;
    for(slot=1;slot<4;++slot)
        if(!group_record(s,slot,0,*s->primary_gate,1,
                         FA18_RECORD_UPDATE_PRIMARY_PLACE)) return 0;
    if(!active_record(s,4,0) ||
       !group_record(s,5,4,*s->secondary_gate,0,
                     FA18_RECORD_UPDATE_SECONDARY_PLACE) ||
       !active_record(s,6,0)) return 0;
    prepare(s,7,6); (void)active(s,7);
    if(!active_record(s,8,0) || !paired_record(s,9,8) || !active_record(s,10,0) ||
       !paired_record(s,11,10) || !active_record(s,12,0) || !paired_record(s,13,12) ||
       !active_record(s,14,1) || !active_record(s,15,1)) return 0;
    return child(s,FA18_RECORD_UPDATE_FINISH,15,0);
}
