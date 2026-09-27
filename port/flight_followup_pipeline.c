#include "flight_followup_pipeline.h"

int fa18_run_flight_followup_pipeline(const FA18FlightFollowupPipelineInput *input,
                                      FA18FlightFollowupPipelineResult *result,
                                      FA18FlightFollowupPipelineRoute *route) {
    FA18FlightFollowupRecordRoute record_route;
    FA18FlightFollowupMagnitudeRecord record;
    uint16_t offset;
    if (!input || !result || !route ||
        fa18_prepare_flight_followup_record(input->selector_table, input->selector_table_size,
                                            &result->record_state, &record_route, &offset) != 0)
        return -1;
    if (record_route == FA18_FLIGHT_FOLLOWUP_EMPTY_CONTINUATION) {
        FA18SceneAlternateRecordRoute alternate_route;
        if (!input->alternate_table || !input->alternate_handler_lookup ||
            (size_t)input->alternate_offset > input->alternate_table_size ||
            input->alternate_table_size - (size_t)input->alternate_offset <
                FA18_SCENE_PLACEMENT_BYTES ||
            fa18_prepare_scene_alternate_record(
                input->alternate_table + input->alternate_offset,
                input->alternate_guard_coordinate, input->alternate_handler_lookup,
                input->alternate_handler_context, &result->alternate_record,
                &alternate_route) != 0)
            return -1;
        switch (alternate_route) {
        case FA18_SCENE_ALTERNATE_RECORD_PREPARED:
            *route = FA18_FLIGHT_FOLLOWUP_PIPELINE_ALTERNATE_RECORD_PREPARED;
            break;
        case FA18_SCENE_ALTERNATE_RECORD_SKIPPED:
            *route = FA18_FLIGHT_FOLLOWUP_PIPELINE_ALTERNATE_RECORD_SKIPPED;
            break;
        case FA18_SCENE_ALTERNATE_RECORD_TERMINATOR:
            *route = FA18_FLIGHT_FOLLOWUP_PIPELINE_ALTERNATE_RECORD_TERMINATOR;
            break;
        default:
            return -1;
        }
        return 0;
    }
    if (!input->record_lookup ||
        input->record_lookup(input->record_context, offset, &record) != 0 ||
        fa18_calculate_flight_followup_magnitudes(&record, &input->magnitude_input,
                                                  input->magnitude_error, input->magnitude_context,
                                                  &result->magnitude) != 0)
        return -1;
    const FA18FlightFollowupShiftInput shift_input = {
        result->magnitude.raw_x, result->magnitude.raw_z,
        result->magnitude.maximum_half, record.offset_18,
        input->magnitude_input.prepared_depth
    };
    if (fa18_shift_flight_followup_components(&shift_input, input->shift_lookup,
                                               input->shift_context, &result->shift) != 0)
        return -1;
    result->descriptor = (FA18FlightFollowupDescriptorState){
        (uint16_t)result->record_state.indexed_word, 0, 0, result->record_state.record_index
    };
    if (fa18_dispatch_flight_followup_descriptor(&result->descriptor,
                                                  input->fixed_point_stage,
                                                  input->fixed_point_context,
                                                  input->descriptor_lookup,
                                                  input->descriptor_context) != 0)
        return -1;
    *route = FA18_FLIGHT_FOLLOWUP_PIPELINE_DESCRIPTOR_DISPATCHED;
    return 0;
}
