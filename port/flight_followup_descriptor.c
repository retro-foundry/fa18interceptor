#include "flight_followup_descriptor.h"

int fa18_dispatch_flight_followup_descriptor(
    FA18FlightFollowupDescriptorState *state,
    FA18FlightFollowupCall fixed_point_stage, void *fixed_point_context,
    FA18FlightFollowupDescriptorLookup lookup, void *lookup_context) {
    FA18FlightFollowupDescriptor descriptor;
    if (!state || !fixed_point_stage || !lookup || fixed_point_stage(fixed_point_context) != 0 ||
        lookup(lookup_context, state->descriptor_kind, &descriptor) != 0 || !descriptor.handler)
        return -1;
    state->control_stream = descriptor.control_stream;
    state->control_aux = descriptor.control_aux;
    if (descriptor.handler(descriptor.handler_context) != 0) return -1;
    state->record_index = (uint16_t)(state->record_index + 2u);
    return 0;
}
