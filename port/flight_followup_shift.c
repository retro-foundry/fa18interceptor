#include "flight_followup_shift.h"

static int32_t add_long(int32_t left, int32_t right) {
    return (int32_t)((uint32_t)left + (uint32_t)right);
}
static int32_t asr_long(int32_t value, unsigned count) {
    count &= 63u;
    if (!count) return value;
    if (value >= 0) return value >> count;
    return (int32_t)-((-(int64_t)value + (((int64_t)1 << count) - 1)) >> count);
}

int fa18_shift_flight_followup_components(
    const FA18FlightFollowupShiftInput *input,
    FA18FlightFollowupShiftLookup lookup, void *context,
    FA18FlightFollowupShiftState *state) {
    int8_t shift;
    int32_t components[3];
    if (!input || !lookup || !state ||
        lookup(context, (int16_t)input->maximum_half, &shift) != 0)
        return -1;
    const unsigned count = (unsigned)(uint8_t)shift & 63u;
    components[0] = asr_long(input->raw_x, count);
    components[1] = asr_long(input->record_offset_18, count);
    components[2] = asr_long(input->raw_z, count);
    *state = (FA18FlightFollowupShiftState){
        shift,
        asr_long(add_long(input->record_offset_18, input->depth_component), count),
        {components[0], components[1], components[2]},
        {(int16_t)asr_long(components[0], 8),
         (int16_t)asr_long(components[1], 8),
         (int16_t)asr_long(components[2], 8)},
        1
    };
    return 0;
}
