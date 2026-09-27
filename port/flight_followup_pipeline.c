#include "flight_followup_pipeline.h"

int fa18_run_flight_followup_pipeline(const FA18FlightFollowupPipelineInput *input,
                                      FA18FlightFollowupPipelineResult *result,
                                      FA18FlightFollowupPipelineRoute *route) {
    FA18FlightFollowupRecordRoute record_route;
    FA18FlightFollowupMagnitudeRecord record;
    uint16_t offset;
    if (!input || !result || !route || !input->record_lookup ||
        fa18_prepare_flight_followup_record(input->selector_table, input->selector_table_size,
                                            &result->record_state, &record_route, &offset) != 0)
        return -1;
    if (record_route == FA18_FLIGHT_FOLLOWUP_EMPTY_CONTINUATION) {
        *route = FA18_FLIGHT_FOLLOWUP_PIPELINE_EMPTY_CONTINUATION;
        return 0;
    }
    if (input->record_lookup(input->record_context, offset, &record) != 0 ||
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
