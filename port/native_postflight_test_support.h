/* Mutable bindings for composed tests only; no runtime replacement children. */
#ifndef FA18_NATIVE_POSTFLIGHT_TEST_SUPPORT_H
#define FA18_NATIVE_POSTFLIGHT_TEST_SUPPORT_H
#include "native_control_record_update.h"
typedef struct {
    uint16_t phase_word,target,gate,command_word,heading; PortFieldByte phase_fields[2];
    uint8_t report,blocked,phase,flags_f,step,context_gate,saved_view,saved_context,
        smooth,started,clear,refresh,aux;
} FA18PostflightTestStorage;
static void fa18_test_bind_postflight(FA18NativePostflight *s,FA18PostflightTestStorage *storage,
                                       FA18NativeControlRecordUpdate *update) {
    storage->phase_fields[0]=(PortFieldByte){.unsigned_word=&storage->phase_word,.shift=8};
    storage->phase_fields[1]=(PortFieldByte){.unsigned_word=&storage->phase_word};
    *s=(FA18NativePostflight){.records=update->records,.selection=update->selection,
        .parameters=&update->view->assets->parameters,.view_work=update->view_work,
        .phase_fields=storage->phase_fields,.current_slot=update->current_slot,
        .target_record=&storage->target,.dispatch_gate=&storage->gate,
        .command_word=&storage->command_word,.view_heading=&storage->heading,
        .space_latch=update->placement->space_latch,.report_latch=&storage->report,
        .status=&update->records->input->indexed.cockpit_low_byte,.blocked=&storage->blocked,
        .mode=update->view->mode,.player_phase=&storage->phase,.player_flags_f=&storage->flags_f,
        .sequence_phase=update->control->sequence_phase,.sequence_step=&storage->step,
        .context_gate=&storage->context_gate,.post_input_event=update->post_input_event,
        .saved_view=&storage->saved_view,.view_side=update->control->view_selector,
        .saved_context=&storage->saved_context,.context_select=update->control->origin_enable,
        .context_smooth=&storage->smooth,.context_started=&storage->started,
        .context_clear=&storage->clear,.refresh=&storage->refresh,.limit=update->view->limit,
        .admitted=update->view->admitted,.aux=&storage->aux,
        .ready_mode=&update->records->input->indexed.pose_entry};
    update->postflight=s;
}
#endif
