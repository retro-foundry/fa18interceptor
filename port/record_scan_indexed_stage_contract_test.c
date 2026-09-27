#include "record_scan_indexed_stage.h"

#include <assert.h>

typedef struct { unsigned calls; uint16_t index; } Fixture;

static uint16_t be16(const uint8_t *bytes) {
    return (uint16_t)((uint16_t)bytes[0] << 8 | bytes[1]);
}

static int32_t be32(const uint8_t *bytes) {
    return (int32_t)((uint32_t)bytes[0] << 24 | (uint32_t)bytes[1] << 16 |
                     (uint32_t)bytes[2] << 8 | bytes[3]);
}

static void put16(uint8_t *bytes, uint16_t value) {
    bytes[0] = (uint8_t)(value >> 8);
    bytes[1] = (uint8_t)value;
}

static int tail(void *context, uint16_t index) {
    Fixture *fixture = context;
    ++fixture->calls;
    fixture->index = index;
    return 0;
}

int main(void) {
    FA18FlaggedSlot slot = {{0}};
    const FA18RecordScanIndexedStageInput input = {
        {64, 0, 0, 0, 64, 0, 0, 0, 64}, 0, 3, 0x1234, 0x5678, 0, 0, 0};
    FA18RecordScanIndexedStageResult result;
    Fixture fixture = {0};
    assert(fa18_run_record_scan_indexed_stage(&slot, 7, &input, tail, &fixture,
                                               &result) == 0);
    assert(result.primary_x == 6 && result.primary_y == 1 && result.primary_z == 24);
    assert(result.secondary_x == 0 && result.secondary_y == 256 &&
           result.secondary_z == 4096);
    assert(be32(slot.bytes) == 6 && be32(slot.bytes + 4) == 1 &&
           be32(slot.bytes + 8) == 24 && be16(slot.bytes + 0x28) == 0x14 &&
           be16(slot.bytes + 0x30) == 0x1234 && be16(slot.bytes + 0x32) == 0x5678);
    assert(fixture.calls == 1 && fixture.index == 7);

    slot = (FA18FlaggedSlot){{0}};
    put16(slot.bytes + 0x26, 0x1000);
    Fixture next = {0};
    assert(fa18_run_record_scan_indexed_stage(&slot, 2, &input, tail, &next,
                                               &result) == 0);
    assert(result.primary_x == -8 && result.primary_y == 0 && result.primary_z == 0 &&
           result.secondary_y == -256 && be16(slot.bytes + 0x28) == 0x1e);

    slot = (FA18FlaggedSlot){{0}};
    FA18RecordScanIndexedStageInput mode = input;
    mode.mode = 0x11;
    assert(fa18_run_record_scan_indexed_stage(&slot, 0, &mode, tail, &next,
                                               &result) == 0);
    assert(result.primary_z == 56 && be16(slot.bytes + 0x28) == 0x14);

    slot = (FA18FlaggedSlot){{0}};
    mode = input;
    mode.turn_word = 0;
    assert(fa18_run_record_scan_indexed_stage(&slot, 0, &mode, tail, &next,
                                               &result) == 0);
    assert(result.secondary_y == 268);
    return 0;
}
