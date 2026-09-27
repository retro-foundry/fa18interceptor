#ifndef FA18_FLIGHT_FOLLOWUP_PIPELINE_H
#define FA18_FLIGHT_FOLLOWUP_PIPELINE_H

#include "flight_followup_descriptor.h"
#include "flight_followup_magnitude.h"
#include "flight_followup_record.h"
#include "flight_followup_shift.h"

typedef int (*FA18FlightFollowupRecordLookup)(void *context, uint16_t offset,
                                               FA18FlightFollowupMagnitudeRecord *record);

typedef struct {
    const uint8_t *selector_table;
    size_t selector_table_size;
    FA18FlightFollowupRecordLookup record_lookup;
    void *record_context;
    FA18FlightFollowupMagnitudeInput magnitude_input;
    FA18FlightFollowupMagnitudeError magnitude_error;
    void *magnitude_context;
    FA18FlightFollowupShiftLookup shift_lookup;
    void *shift_context;
    FA18FlightFollowupCall fixed_point_stage;
    void *fixed_point_context;
    FA18FlightFollowupDescriptorLookup descriptor_lookup;
    void *descriptor_context;
} FA18FlightFollowupPipelineInput;

typedef enum {
    FA18_FLIGHT_FOLLOWUP_PIPELINE_EMPTY_CONTINUATION,
    FA18_FLIGHT_FOLLOWUP_PIPELINE_DESCRIPTOR_DISPATCHED
} FA18FlightFollowupPipelineRoute;

typedef struct {
    FA18FlightFollowupRecordState record_state;
    FA18FlightFollowupMagnitudeResult magnitude;
    FA18FlightFollowupShiftState shift;
    FA18FlightFollowupDescriptorState descriptor;
} FA18FlightFollowupPipelineResult;

/* Compose the bounded positive `$C1CCBC-$C1CE37` sequence. */
int fa18_run_flight_followup_pipeline(const FA18FlightFollowupPipelineInput *input,
                                      FA18FlightFollowupPipelineResult *result,
                                      FA18FlightFollowupPipelineRoute *route);

#endif
