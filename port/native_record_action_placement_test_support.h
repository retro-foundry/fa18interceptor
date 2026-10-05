/* Explicit field/data bindings for composed contracts only. */
#ifndef FA18_NATIVE_RECORD_ACTION_PLACEMENT_TEST_SUPPORT_H
#define FA18_NATIVE_RECORD_ACTION_PLACEMENT_TEST_SUPPORT_H
#include "native_record_action_placement.h"
#include "native_record_pose.h"
#include "native_record_control.h"
typedef struct {
    FA18NativeRecordActionPlacementAssets assets; FA18NativeScenePointerGroup groups[16];
    uint8_t offsets[1024],space,pending,enable,divisor,alert,changed,redraw_a,redraw_b;
    uint16_t viewed,primary,alternate; uint32_t warnings;
    PortFieldByte viewed_fields[2];
} FA18RecordActionPlacementTestStorage;
static void fa18_test_bind_record_action_placement(FA18NativeRecordActionPlacement *s,
        FA18RecordActionPlacementTestStorage *storage,FA18NativeRecordControl *control,
        FA18NativeRecordPose *pose,uint8_t *limit) {
    storage->assets.offsets_10=(PortFieldWindow){.bytes=storage->offsets,.byte_count=1024,.origin=512};
    storage->assets.offsets_other=storage->assets.offsets_10;
    storage->assets.primary.procedure=FA18_SCENE_PROCEDURE_RECORD_STREAM;
    storage->assets.alternate.procedure=FA18_SCENE_PROCEDURE_RECORD_STREAM;
    storage->viewed_fields[0]=(PortFieldByte){.unsigned_word=&storage->viewed,.shift=8};
    storage->viewed_fields[1]=(PortFieldByte){.unsigned_word=&storage->viewed};
    *s=(FA18NativeRecordActionPlacement){.records=control->records,.assets=&storage->assets,
        .pointer_groups=storage->groups,.pointer_group_count=16,.view_work=control->view_work,
        .current_slot=control->current_slot,.viewed_record=storage->viewed_fields,.selector_word=pose->selector_word,
        .primary_count=&storage->primary,.alternate_count=&storage->alternate,
        .warning_causes=&storage->warnings,.events=pose->events,.space_latch=&storage->space,
        .fire_pending=&storage->pending,.secondary_enable=&storage->enable,.limit=limit,
        .divisor=&storage->divisor,.alert_countdown=&storage->alert,.fire_state=control->stream_view_state,
        .mode_changed=&storage->changed,.stores_redraw_a=&storage->redraw_a,
        .stores_redraw_b=&storage->redraw_b,.scene_redraw=control->scene_redraw};
}
#endif
