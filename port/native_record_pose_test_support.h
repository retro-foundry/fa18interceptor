/* Lower-boundary contracts and bindings for tests only. */
#ifndef FA18_NATIVE_RECORD_POSE_TEST_SUPPORT_H
#define FA18_NATIVE_RECORD_POSE_TEST_SUPPORT_H
#include "native_record_pose.h"
#include "native_record_control.h"
#include <assert.h>
typedef struct {
    FA18NativeRecordMotionHistory history; FA18NativeRecordPoseOps ops;
    uint8_t history_bytes[128]; PortFieldByte fields[128];
    uint16_t selector,matrix_control,shown,grid_x,grid_z,error,collision_slot,damage;
    uint32_t events;
    uint8_t cell_only,activity,bar_redraw,view_decay,collision_enable,collision_inhibit;
    uint8_t cockpit_a,cockpit_b,report,failure,failure_view,request,request_clear,count,index;
    unsigned calls[14]; int complete;
} FA18RecordPoseTestStorage;
static int fa18_test_pose_child(void *context,FA18NativeRecordPose *state,
        FA18NativeRecordPoseChild child,const FA18NativeRecordPoseInput *in,
        FA18NativeRecordPoseResult *out) {
    FA18RecordPoseTestStorage *storage=context;
    assert(state && in && out && child<14 && in->slot<16);
    ++storage->calls[child];
    if(child==FA18_POSE_MOTION_CANDIDATE) out->clear=1;
    return storage->complete;
}
static void fa18_test_bind_record_pose(FA18NativeRecordPose *s,FA18RecordPoseTestStorage *storage,
        FA18NativeRecordControl *control,uint16_t *stride) {
    unsigned i;
    for(i=0;i<128;++i) storage->fields[i]=(PortFieldByte){.byte=storage->history_bytes+i};
    storage->fields[28]=(PortFieldByte){.byte=&storage->count};
    storage->fields[29]=(PortFieldByte){.byte=&storage->index};
    storage->fields[30]=(PortFieldByte){.unsigned_word=&storage->collision_slot,.shift=8};
    storage->fields[31]=(PortFieldByte){.unsigned_word=&storage->collision_slot};
    storage->history=(FA18NativeRecordMotionHistory){storage->fields,128,32};
    storage->ops=(FA18NativeRecordPoseOps){fa18_test_pose_child,storage}; storage->complete=1;
    *s=(FA18NativeRecordPose){.records=control->records,.ops=&storage->ops,.history=&storage->history,
        .current_slot=control->current_slot,.current_stride=stride,.target_slot=control->target_slot,
        .selector_word=&storage->selector,.matrix_control=&storage->matrix_control,
        .shown_message=&storage->shown,.grid_x=&storage->grid_x,.grid_z=&storage->grid_z,
        .error_word=&storage->error,.collision_slot=&storage->collision_slot,.damage_count=&storage->damage,
        .events=&storage->events,.cell_only=&storage->cell_only,.post_input_event=control->post_input_event,
        .origin_enable=control->origin_enable,.activity_count=&storage->activity,.scene_redraw=control->scene_redraw,
        .bar_redraw=&storage->bar_redraw,.view_decay=&storage->view_decay,
        .collision_enable=&storage->collision_enable,.collision_inhibit=&storage->collision_inhibit,
        .cockpit_a=&storage->cockpit_a,.cockpit_b=&storage->cockpit_b,.collision_report=&storage->report,
        .mission_failure=&storage->failure,.failure_view=&storage->failure_view,
        .request_flag=&storage->request,.request_clear=&storage->request_clear,
        .history_count=&storage->count,.history_index=&storage->index};
}
#endif
