#ifndef FA18_FLIGHT_FOLLOWUP_MAGNITUDE_H
#define FA18_FLIGHT_FOLLOWUP_MAGNITUDE_H

#include <stdint.h>

typedef struct {
    uint16_t coordinate_x;
    uint16_t coordinate_z;
    int32_t depth;
    int32_t offset_18;
    uint32_t origin_x;
    uint32_t origin_z;
} FA18FlightFollowupMagnitudeRecord;

typedef struct {
    int16_t selector_x;
    int16_t selector_z;
    int16_t prepared_x;
    int16_t prepared_z;
    int32_t prepared_depth;
} FA18FlightFollowupMagnitudeInput;

typedef int (*FA18FlightFollowupMagnitudeError)(void *context, uint16_t code);

typedef struct {
    int32_t raw_x;
    int32_t raw_z;
    uint16_t maximum_half;
    uint8_t capped;
} FA18FlightFollowupMagnitudeResult;

/* `$C1CD0E-$C1CDB1`: prepare and cap the follow-up magnitude maximum. */
int fa18_calculate_flight_followup_magnitudes(
    const FA18FlightFollowupMagnitudeRecord *record,
    const FA18FlightFollowupMagnitudeInput *input,
    FA18FlightFollowupMagnitudeError error_callback, void *context,
    FA18FlightFollowupMagnitudeResult *result);

#endif
