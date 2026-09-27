#ifndef FA18_FLIGHT_FOLLOWUP_SHIFT_H
#define FA18_FLIGHT_FOLLOWUP_SHIFT_H

#include <stdint.h>

typedef int (*FA18FlightFollowupShiftLookup)(void *context, int16_t index,
                                             int8_t *shift);

typedef struct {
    int32_t raw_x;
    int32_t raw_z;
    uint16_t maximum_half;
    int32_t record_offset_18;
    int32_t depth_component;
} FA18FlightFollowupShiftInput;

typedef struct {
    int16_t shift_count;
    int32_t shifted_depth;
    int32_t shifted_components[3];
    int16_t component_words[3];
    uint8_t ready_flag;
} FA18FlightFollowupShiftState;

/* `$C1CDB2-$C1CDFB`: table-select an ASR count and publish shifted terms. */
int fa18_shift_flight_followup_components(
    const FA18FlightFollowupShiftInput *input,
    FA18FlightFollowupShiftLookup lookup, void *context,
    FA18FlightFollowupShiftState *state);

#endif
