#ifndef FA18_FLIGHT_FOLLOWUP_DESCRIPTOR_H
#define FA18_FLIGHT_FOLLOWUP_DESCRIPTOR_H

#include <stdint.h>

typedef int (*FA18FlightFollowupCall)(void *context);

typedef struct {
    FA18FlightFollowupCall handler;
    void *handler_context;
    const void *secondary_pointer;
    uint32_t control_stream;
    uint32_t control_aux;
} FA18FlightFollowupDescriptor;

typedef int (*FA18FlightFollowupDescriptorLookup)(void *context, uint16_t kind,
                                                   FA18FlightFollowupDescriptor *descriptor);

typedef struct {
    uint16_t descriptor_kind;
    uint32_t control_stream;
    uint32_t control_aux;
    uint16_t record_index;
} FA18FlightFollowupDescriptorState;

/* `$C1CDFC-$C1CE37`: call fixed point, dispatch descriptor, and advance +2. */
int fa18_dispatch_flight_followup_descriptor(
    FA18FlightFollowupDescriptorState *state,
    FA18FlightFollowupCall fixed_point_stage, void *fixed_point_context,
    FA18FlightFollowupDescriptorLookup lookup, void *lookup_context);

#endif
