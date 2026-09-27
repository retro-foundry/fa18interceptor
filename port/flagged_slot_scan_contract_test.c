#include "flagged_slot_scan.h"
#include <assert.h>
typedef struct { unsigned calls; uint16_t slot; } Fixture;
static int evaluate(void *context, uint16_t slot, int32_t *result) {
    Fixture *fixture = context; ++fixture->calls; fixture->slot = slot; *result = 1; return 0;
}
int main(void) {
    FA18FlaggedSlot slots[FA18_FLAGGED_SLOT_SCAN_SLOTS] = {{0}};
    Fixture fixture = {0}; int32_t result = -1;
    assert(fa18_scan_flagged_flight_slots(slots, evaluate, &fixture, &result) == 0);
    assert(!fixture.calls && !result);
    slots[2].bytes[0x27] = 1; slots[17].bytes[0x27] = 1;
    assert(fa18_scan_flagged_flight_slots(slots, evaluate, &fixture, &result) == 0);
    assert(fixture.calls == 1 && fixture.slot == 17 && result == 1);
    assert(fa18_scan_flagged_flight_slots(slots, 0, &fixture, &result) == -1);
    return 0;
}
