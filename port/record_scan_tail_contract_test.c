#include "record_scan_tail.h"

#include <assert.h>

typedef struct { int16_t selector, x, y, z; } Fixture;

static int32_t be32(const uint8_t *bytes) {
    return (int32_t)((uint32_t)bytes[0] << 24 | (uint32_t)bytes[1] << 16 |
                     (uint32_t)bytes[2] << 8 | bytes[3]);
}

static void put16(uint8_t *bytes, uint16_t value) {
    bytes[0] = (uint8_t)(value >> 8);
    bytes[1] = (uint8_t)value;
}

static int scalar(void *context, int16_t selector, int16_t x, int16_t y,
                  int16_t z, int16_t result[3]) {
    Fixture *fixture = context;
    fixture->selector = selector;
    fixture->x = x;
    fixture->y = y;
    fixture->z = z;
    result[0] = 1;
    result[1] = -2;
    result[2] = 3;
    return 0;
}

int main(void) {
    FA18FlaggedSlot slot = {{0}};
    FA18FlaggedLinkedRecord linked = {{0}};
    put16(linked.bytes + 0x6c, 0x0080);
    const FA18RecordScanTailInput input = {
        &linked, 1, 0, 0, 0,
        0, 0x1000, 0, 8, 16, 24};
    Fixture fixture = {0};
    FA18RecordScanTailResult result;
    assert(fa18_run_record_scan_tail(&slot, &input, scalar, &fixture, &result) == 0);
    assert(result.selector == 0xd8 && fixture.selector == 0xd8 &&
           fixture.x == 0 && fixture.y == 0x10 && fixture.z == 0);
    assert(be32(slot.bytes + 0x0c) == 8 && be32(slot.bytes + 0x10) == -16 &&
           be32(slot.bytes + 0x14) == 24 && be32(slot.bytes + 0x18) == 1 &&
           be32(slot.bytes + 0x1c) == -2 && be32(slot.bytes + 0x20) == 3);

    slot = (FA18FlaggedSlot){{0}};
    put16(slot.bytes + 0x26, 0x1000);
    assert(fa18_run_record_scan_tail(&slot, &input, scalar, &fixture, &result) == 0);
    assert(result.selector == 13 && be32(slot.bytes + 0x0c) == 16 &&
           be32(slot.bytes + 0x10) == 0 && be32(slot.bytes + 0x14) == 48 &&
           be32(slot.bytes + 0x18) == 2 && be32(slot.bytes + 0x1c) == 0 &&
           be32(slot.bytes + 0x20) == 6);
    return 0;
}
