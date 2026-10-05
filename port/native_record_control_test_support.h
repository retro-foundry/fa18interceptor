/* Shared bindings for composed contracts only. No runtime assets or children. */
#ifndef FA18_NATIVE_RECORD_CONTROL_TEST_SUPPORT_H
#define FA18_NATIVE_RECORD_CONTROL_TEST_SUPPORT_H
#include "native_record_control.h"

typedef struct {
    FA18NativeRecordControlAssets assets;
    FA18NativeControlStream stream;
    uint8_t messages[64],metadata[128];
    uint16_t bias,position,target;
    uint8_t alert,index,pending,gate,enable,origin,phase,selector,stream_view,redraw;
} FA18RecordControlTestStorage;

static void fa18_test_bind_record_control(FA18NativeRecordControl *s,
        FA18RecordControlTestStorage *storage,FA18NativeSceneRecords *records,
        FA18NativeRecordViewWork *work,uint16_t *slot,uint8_t *event,uint8_t *mode) {
    storage->assets=(FA18NativeRecordControlAssets){.streams=&storage->stream,.count=1,
        .messages={.bytes=storage->messages,.byte_count=sizeof storage->messages},
        .metadata={.bytes=storage->metadata,.byte_count=sizeof storage->metadata}};
    *s=(FA18NativeRecordControl){.records=records,.assets=&storage->assets,.view_work=work,
        .magnitude_bias=&storage->bias,.stream_position=&storage->position,.current_slot=slot,
        .target_slot=&storage->target,.magnitude_alert=&storage->alert,.mode=mode,
        .stream_index=&storage->index,.stream_pending=&storage->pending,.stream_gate=&storage->gate,
        .post_input_event=event,.playback_enable=&storage->enable,.origin_enable=&storage->origin,
        .sequence_phase=&storage->phase,.view_selector=&storage->selector,
        .stream_view_state=&storage->stream_view,.scene_redraw=&storage->redraw};
}
#endif
