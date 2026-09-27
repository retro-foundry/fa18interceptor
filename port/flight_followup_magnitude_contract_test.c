#include "flight_followup_magnitude.h"

#include <assert.h>

static int report(void *context, uint16_t code) {
    unsigned *calls = context;
    assert(code == 0x29);
    ++*calls;
    return 0;
}
static int fail(void *context, uint16_t code) { (void)context; (void)code; return -1; }

int main(void) {
    FA18FlightFollowupMagnitudeRecord record = {0};
    FA18FlightFollowupMagnitudeInput input = {0};
    FA18FlightFollowupMagnitudeResult result;
    unsigned calls = 0;

    assert(fa18_calculate_flight_followup_magnitudes(&record, &input, report, &calls,
                                                      &result) == 0);
    assert(!result.raw_x && !result.raw_z && !result.maximum_half && !result.capped && !calls);
    record.depth = 0x00100000;
    assert(fa18_calculate_flight_followup_magnitudes(&record, &input, report, &calls,
                                                      &result) == 0);
    assert(result.maximum_half == 0xef && result.capped && calls == 1);
    assert(fa18_calculate_flight_followup_magnitudes(&record, &input, fail, &calls,
                                                      &result) == -1);
    assert(fa18_calculate_flight_followup_magnitudes(0, &input, report, &calls, &result) == -1);
    return 0;
}
