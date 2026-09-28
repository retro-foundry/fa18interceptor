#include "template_bitmask_buffers.h"

#include <assert.h>
#include <string.h>

static void put16(uint8_t *bytes, uint16_t value) {
    bytes[0] = (uint8_t)(value >> 8);
    bytes[1] = (uint8_t)value;
}

static void initialize_stream(uint8_t *bytes, size_t size) {
    assert(size >= 0x108);
    for (size_t row = 0; row != FA18_TEMPLATE_BITMASK_ROWS; ++row)
        put16(bytes + row * 2u, 0x100);
    put16(bytes + 0x100, 0xffff);
}

int main(void) {
    uint8_t first[0x108] = {0};
    uint8_t second[0x108] = {0};
    uint8_t third[0x108] = {0};
    uint8_t hunk[0x308] = {0};
    FA18TemplateBitmaskBuffers buffers;
    FA18HunkSegment segments[67] = {{0}};
    FA18Hunks hunks = {segments, 67};

    initialize_stream(first, sizeof first);
    initialize_stream(second, sizeof second);
    initialize_stream(third, sizeof third);
    put16(first, 0x100); put16(first + 0x100, 4);
    put16(first + 0x102, 0); put16(first + 0x104, 63);
    put16(second + 2, 0x100); put16(second + 0x100, 2);
    put16(second + 0x102, 32);
    assert(fa18_build_template_bitmask_buffers(first, sizeof first,
                                                second, sizeof second,
                                                third, sizeof third, &buffers) == 0);
    assert(buffers.first[3] == 0x01 && buffers.first[4] == 0x80 &&
           buffers.second[23] == 0x01 && !buffers.third[0]);
    first[0] = 0;
    assert(fa18_build_template_bitmask_buffers(first, sizeof first,
                                                second, sizeof second,
                                                third, sizeof third, &buffers) == 0x43);
    memcpy(hunk, first, sizeof first);
    memcpy(hunk + 0x100, second, sizeof second);
    memcpy(hunk + 0x200, third, sizeof third);
    segments[66] = (FA18HunkSegment){FA18_HUNK_CODE, hunk, sizeof hunk, 0, 0};
    assert(fa18_initialize_template_bitmask_buffers(&hunks, &buffers) == 0x43);
    return 0;
}
