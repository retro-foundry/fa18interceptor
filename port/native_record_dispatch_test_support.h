#ifndef FA18_NATIVE_RECORD_DISPATCH_TEST_SUPPORT_H
#define FA18_NATIVE_RECORD_DISPATCH_TEST_SUPPORT_H
/* Composed contract bindings only. The differential proof supplies the source
 * control rows and live neighboring fields; these fixtures use gated routes. */
#include "native_control_record_update.h"
#include <assert.h>
typedef struct {
    FA18FlightCommandState flight; FA18ViewCommandState view;
    FA18ContextCommandState context; FA18CommandQueue queue;
    FA18NativeContextPublication publication; FA18ViewSpanOffsets spans;
    uint8_t rows[64],track,clear,choices[2]; PortFieldWindow controls;
    uint16_t marker;
} FA18RecordDispatchTestStorage;
static void fa18_test_bind_record_dispatch(FA18NativeRecordDispatch *s,
    FA18RecordDispatchTestStorage *storage,FA18NativeControlRecordUpdate *update,
    FA18ContextCommandState *context,FA18CommandQueue *queue) {
    if(!context) {
        uint8_t neighbors[FA18_COMMAND_QUEUE_NEIGHBORS]={0},keys[128]={0};
        storage->flight.commands=update->records->input;
        storage->flight.player=storage->flight.viewed=update->records->aircraft;
        storage->view.flight=&storage->flight; storage->context.view=&storage->view;
        storage->context.records=update->records->geometry; storage->context.record_count=16;
        assert(fa18_initialize_command_queue(&storage->queue,&storage->context,
            neighbors,sizeof neighbors,keys,sizeof keys));
        context=&storage->context; queue=&storage->queue;
    }
    storage->publication=(FA18NativeContextPublication){update->records,context,queue,
        &storage->spans,&storage->marker,update->control->target_slot};
    storage->controls=(PortFieldWindow){.bytes=storage->rows,.byte_count=sizeof storage->rows};
    update->control->scene_redraw=update->pose->scene_redraw=
        update->placement->scene_redraw=&context->view->update_mask;
    *s=(FA18NativeRecordDispatch){.view=update->view,.view_work=update->view_work,
        .range=update->range,.publication=&storage->publication,.selected_controls=&storage->controls,
        .cell_only=update->pose->cell_only,.track_enable=&storage->track,
        .sequence_phase=update->control->sequence_phase,.sequence_step=update->postflight->sequence_step,
        .detail_clear=&storage->clear,.scene_redraw=update->control->scene_redraw,
        .control_choice={storage->choices,storage->choices+1},.target_slot=update->control->target_slot};
    update->dispatch=s;
}
#endif
