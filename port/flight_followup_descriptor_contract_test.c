#include "flight_followup_descriptor.h"

#include <assert.h>

typedef struct { unsigned fixed, lookup, handler; } Log;
static int fixed(void *context) { ++((Log *)context)->fixed; return 0; }
static int handler(void *context) { ++((Log *)context)->handler; return 0; }
static int lookup(void *context, uint16_t kind, FA18FlightFollowupDescriptor *out) {
    Log *log = context; ++log->lookup; assert(kind == 3);
    *out = (FA18FlightFollowupDescriptor){handler, log, 0, 0x11223344u, 0x55667788u}; return 0;
}
int main(void) {
    Log log = {0};
    FA18FlightFollowupDescriptorState state = {3, 0, 0, 0xfffe};
    assert(fa18_dispatch_flight_followup_descriptor(&state, fixed, &log, lookup, &log) == 0);
    assert(log.fixed == 1 && log.lookup == 1 && log.handler == 1 &&
           state.control_stream == 0x11223344u && state.control_aux == 0x55667788u &&
           state.record_index == 0);
    assert(fa18_dispatch_flight_followup_descriptor(&state, 0, &log, lookup, &log) == -1);
    return 0;
}
