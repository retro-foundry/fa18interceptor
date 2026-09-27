#include "flagged_slot_evaluator.h"

#include <assert.h>

typedef struct {
    const FA18FlaggedLinkedRecord *record;
    int16_t offset;
    unsigned calls;
} Fixture;

static void put16(uint8_t *bytes, uint16_t value) {
    bytes[0] = (uint8_t)(value >> 8);
    bytes[1] = (uint8_t)value;
}

static void put32(uint8_t *bytes, uint32_t value) {
    bytes[0] = (uint8_t)(value >> 24);
    bytes[1] = (uint8_t)(value >> 16);
    bytes[2] = (uint8_t)(value >> 8);
    bytes[3] = (uint8_t)value;
}

static int resolve(void *context, int16_t offset,
                   const FA18FlaggedLinkedRecord **record) {
    Fixture *fixture = context;
    ++fixture->calls;
    if (offset != fixture->offset) return -1;
    *record = fixture->record;
    return 0;
}

int main(void) {
    uint16_t words[258];
    FA18FlaggedSlot slots[FA18_FLAGGED_SLOT_SCAN_SLOTS] = {{0}};
    FA18FlaggedLinkedRecord linked = {{0}};
    for (unsigned index = 0; index != 258; ++index) words[index] = 0x4000;
    const FA18SceneMagnitudeTable table = {words, 258};
    Fixture fixture = {&linked, 0x200, 0};
    FA18FlaggedSlotEvaluator evaluator = {
        slots, &table, resolve, &fixture, 0, 0, 0, 0};
    int32_t result = -1;

    assert(fa18_evaluate_flagged_flight_slot(&evaluator, 1, &result) == 0);
    assert(result == 0 && fixture.calls == 0);

    slots[1].bytes[0x26] = 0x20;
    put16(slots[1].bytes + 0x2e, 1);
    put32(linked.bytes + 0x14, 0x00000100);
    put32(slots[1].bytes, 0x00000200);
    assert(fa18_evaluate_flagged_flight_slot(&evaluator, 1, &result) == 0);
    assert(result == 1 && fixture.calls == 1);

    put32(slots[1].bytes, 0x00000000);
    assert(fa18_evaluate_flagged_flight_slot(&evaluator, 1, &result) == 0);
    assert(result == 0 && fixture.calls == 2);

    slots[1].bytes[0x26] = 0;
    evaluator.context_selection = 1;
    assert(fa18_evaluate_flagged_flight_slot(&evaluator, 1, &result) == 0);
    assert(fixture.calls == 3);
    assert(fa18_evaluate_flagged_flight_slot(&evaluator, 20, &result) == -1);
    return 0;
}
