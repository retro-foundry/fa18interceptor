#include "native_record_control_test_support.h"
#include "native_record_pose_test_support.h"
#include <string.h>
typedef struct {
    FA18CommandInput input; FA18NativeSceneRecords records;
    FA18NativeRecordControl control; FA18RecordControlTestStorage control_storage;
    FA18NativeRecordPose pose; FA18RecordPoseTestStorage storage;
    FA18NativeRecordViewWork work; FA18NativeRecordPoseOps ops;
    uint8_t source[8192],work_bytes[512],event,mode;
    uint16_t slot,stride; int inject_rates;
} Fixture;
static int child(void *context,FA18NativeRecordPose *s,FA18NativeRecordPoseChild kind,
        const FA18NativeRecordPoseInput *in,FA18NativeRecordPoseResult *out) {
    Fixture *f=context; assert(s==&f->pose && in->slot==f->slot);
    if(kind==FA18_POSE_ROOT_FLIGHT && f->inject_rates) {
        f->records.records[0].long_3e=(uint32_t)-10;
        f->records.records[0].long_42=0; f->records.records[0].long_46=3;
    }
    if(kind==FA18_POSE_MOTION_CANDIDATE && f->inject_rates) {
        assert(in->point[0]==5 && in->point[1]==0 && in->point[2]==0);
    }
    return fa18_test_pose_child(&f->storage,s,kind,in,out);
}
static void initialize(Fixture *f) {
    memset(f,0,sizeof *f);
    assert(fa18_import_native_scene_records(&f->records,&f->input,
        f->source,sizeof f->source,f->work_bytes,sizeof f->work_bytes));
    fa18_test_bind_record_control(&f->control,&f->control_storage,&f->records,
        &f->work,&f->slot,&f->event,&f->mode);
    fa18_test_bind_record_pose(&f->pose,&f->storage,&f->control,&f->stride);
    f->ops=(FA18NativeRecordPoseOps){child,f}; f->pose.ops=&f->ops;
}
static uint8_t record_byte(Fixture *f,unsigned slot,size_t at) {
    uint8_t b; assert(fa18_read_native_scene_record(f->records.records+slot,at,&b,1)); return b;
}
static uint8_t history_byte(Fixture *f,size_t at) {
    uint8_t b; assert(port_read_field_byte(f->storage.fields+at,&b)); return b;
}
int main(void) {
    static Fixture f;
    initialize(&f); f.pose.current_stride=NULL;
    assert(!fa18_update_native_record_pose(&f.pose,0));
    initialize(&f); f.event=1; f.pose.ops=NULL;
    assert(fa18_update_native_record_pose(&f.pose,0));

    /* The non-root cell route precedes the event return and retains its word
     * packing, including the original signed extension and rotation. */
    initialize(&f); f.slot=1; f.stride=512; f.storage.cell_only=1; f.event=1;
    f.records.aircraft[1].flags=0x40;
    f.records.records[1].word_0c=0x3f00; f.records.records[1].word_0e=0x200;
    assert(fa18_update_native_record_pose(&f.pose,1));
    assert(f.records.records[1].byte_0a==12 && f.records.geometry[1].inverse[0][0]==0x4000);
    f.pose.ops=NULL;
    assert(fa18_update_native_record_pose(&f.pose,1));
    f.pose.trig=NULL;
    assert(!fa18_update_native_record_pose(&f.pose,1) && f.records.records[1].byte_0a==12);

    initialize(&f); f.inject_rates=1; f.storage.matrix_control=0x40;
    f.records.aircraft[0].flags=0x1000;
    f.records.geometry[0].position[0]=5; f.records.geometry[0].position[2]=100;
    assert(fa18_update_native_record_pose(&f.pose,0));
    assert(f.records.geometry[0].position[0]==0xfffffffbu && f.records.geometry[0].position[2]==3);
    assert(f.records.records[0].word_06==0xffff && f.control_storage.redraw==0xff);
    assert(f.storage.calls[FA18_POSE_ROOT_FLIGHT]==1 && f.storage.calls[FA18_POSE_MOTION_CANDIDATE]==1);

    initialize(&f); f.storage.complete=0;
    assert(!fa18_update_native_record_pose(&f.pose,0));
    assert(f.storage.calls[FA18_POSE_RECORD_MATRIX]==1 && !f.storage.count);
    initialize(&f); f.records.records[0].byte_7c=0x11; f.storage.view_decay=0x84;
    assert(fa18_update_native_record_pose(&f.pose,0));
    assert(!f.records.records[0].byte_7c && f.storage.bar_redraw==3 && f.storage.view_decay==0x85);
    f.records.records[0].byte_7c=0xe1;
    assert(fa18_update_native_record_pose(&f.pose,0));
    assert(f.records.records[0].byte_7c==0xe2 && f.storage.view_decay==0x85);

    /* Actual history duplicates the tuple below count five, then wraps the
     * six-row cursor. The duplicate row remains an explicit seventh tuple. */
    initialize(&f); f.storage.index=5; f.storage.count=4;
    f.records.records[0].word_6e=1; f.records.aircraft[0].secondary_flags=0x1000;
    f.records.geometry[0].position[0]=0x01020304;
    f.records.geometry[0].position[1]=0x05060708; f.records.geometry[0].position[2]=0x090a0b0c;
    assert(fa18_update_native_record_motion_history(&f.pose));
    assert(f.storage.index==0 && f.storage.count==5 && record_byte(&f,0,0x3d)==4);
    assert(history_byte(&f,32+60)==1 && history_byte(&f,32+72)==1 && history_byte(&f,32+83)==12);

    /* A negative row crosses the count/index/slot owners. Re-read the live
     * count after that write; a frozen packed mirror would duplicate wrongly. */
    initialize(&f); f.storage.index=0xff;
    f.records.records[0].word_6e=1; f.records.aircraft[0].secondary_flags=0x1000;
    f.records.geometry[0].position[2]=0x06ff0000;
    assert(fa18_update_native_record_motion_history(&f.pose));
    assert(f.storage.count==6 && !f.storage.index && !f.storage.collision_slot);
    assert(record_byte(&f,0,0x3d)==4 && history_byte(&f,28)==6 && !history_byte(&f,29));

    initialize(&f); f.storage.history.byte_count=44;
    f.records.geometry[0].position[0]=0x11223344;
    assert(!fa18_update_native_record_motion_history(&f.pose));
    assert(history_byte(&f,32)==0x11 && !f.storage.index && !f.storage.count);
    initialize(&f); f.stride=1; f.storage.collision_slot=1;
    assert(!fa18_update_native_record_motion_history(&f.pose) && !f.storage.count);
    return 0;
}
