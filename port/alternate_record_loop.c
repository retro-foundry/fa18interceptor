#include "alternate_record_loop.h"
int fa18_finish_alternate_record_loop(FA18ScenePlacementRecord *record, const FA18AlternateRecordLoopInput *input, FA18AlternateRecordLoopState *state, FA18AlternateRecordLoopRoute *route) {
    if (!record || !input || !state || !route || !input->call) return -1;
    *state = (FA18AlternateRecordLoopState){0};
    if (!(input->header & 0x50u)) {
        state->record_value = (uint16_t)record->per_frame[3];
        if ((int16_t)state->record_value < 0) {
            uint8_t countdown = (uint8_t)record->per_frame[2];
            --countdown;
            record->per_frame[2] = (record->per_frame[2] & 0xff00u) | countdown;
            if ((int8_t)countdown >= 0) { *route = FA18_ALTERNATE_RECORD_LOOP_SKIPPED; return 0; }
            record->per_frame[2] = (record->per_frame[2] & 0xff00u) | input->refresh_byte;
        }
    }
    uint8_t byte = (uint8_t)record->per_frame[2];
    --byte; record->per_frame[2] = (record->per_frame[2] & 0xff00u) | byte;
    state->record_byte = byte;
    state->fixed_value = (int16_t)record->per_frame[1];
    state->kind = (uint16_t)((input->header & 0xff00u) << 1);
    if (state->kind == input->selected_kind && !input->scene_active) state->fixed_value = 0x7fff;
    if (fa18_run_scene_descriptor_stage(record, &input->descriptor, input->call, input->call_context, &state->descriptor) != 0) return -1;
    state->next_offset = (uint16_t)(input->next_offset + FA18_SCENE_PLACEMENT_BYTES);
    *route = FA18_ALTERNATE_RECORD_LOOP_DISPATCHED;
    return 0;
}
