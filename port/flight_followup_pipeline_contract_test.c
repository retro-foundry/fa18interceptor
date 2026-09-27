#include "flight_followup_pipeline.h"

#include <assert.h>

typedef struct { unsigned record, error, shift, fixed, descriptor, handler; } Log;
static int record_lookup(void *context, uint16_t offset, FA18FlightFollowupMagnitudeRecord *out) {
    Log *log = context; assert(offset == 512); ++log->record; *out = (FA18FlightFollowupMagnitudeRecord){0}; return 0;
}
static int error(void *context, uint16_t code) { (void)code; ++((Log *)context)->error; return 0; }
static int shift(void *context, int16_t index, int8_t *out) { Log *log = context; assert(index == 0); ++log->shift; *out = 0; return 0; }
static int fixed(void *context) { ++((Log *)context)->fixed; return 0; }
static int handler(void *context) { ++((Log *)context)->handler; return 0; }
static int descriptor(void *context, uint16_t kind, FA18FlightFollowupDescriptor *out) {
    Log *log = context; assert(kind == 1); ++log->descriptor;
    *out = (FA18FlightFollowupDescriptor){handler, log, 0, 0x11, 0x22}; return 0;
}
int main(void) {
    uint8_t table[] = {0, 1};
    Log log = {0};
    FA18FlightFollowupPipelineInput input = {
        table, sizeof table, record_lookup, &log, {0}, error, &log,
        shift, &log, fixed, &log, descriptor, &log
    };
    FA18FlightFollowupPipelineResult result;
    FA18FlightFollowupPipelineRoute route;
    assert(fa18_run_flight_followup_pipeline(&input, &result, &route) == 0);
    assert(route == FA18_FLIGHT_FOLLOWUP_PIPELINE_DESCRIPTOR_DISPATCHED &&
           log.record == 1 && !log.error && log.shift == 1 && log.fixed == 1 &&
           log.descriptor == 1 && log.handler == 1 && result.descriptor.record_index == 2);
    table[1] = 0;
    assert(fa18_run_flight_followup_pipeline(&input, &result, &route) == 0 &&
           route == FA18_FLIGHT_FOLLOWUP_PIPELINE_EMPTY_CONTINUATION && log.record == 1);
    assert(fa18_run_flight_followup_pipeline(0, &result, &route) == -1);
    return 0;
}
