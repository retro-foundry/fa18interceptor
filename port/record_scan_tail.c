#include "record_scan_tail.h"

static uint16_t be16(const uint8_t *bytes) {
    return (uint16_t)((uint16_t)bytes[0] << 8 | bytes[1]);
}

static int32_t be32(const uint8_t *bytes) {
    return (int32_t)((uint32_t)bytes[0] << 24 | (uint32_t)bytes[1] << 16 |
                     (uint32_t)bytes[2] << 8 | bytes[3]);
}

static void put_be16(uint8_t *bytes, uint16_t value) {
    bytes[0] = (uint8_t)(value >> 8);
    bytes[1] = (uint8_t)value;
}

static void put_be32(uint8_t *bytes, int32_t value) {
    const uint32_t bits = (uint32_t)value;
    bytes[0] = (uint8_t)(bits >> 24);
    bytes[1] = (uint8_t)(bits >> 16);
    bytes[2] = (uint8_t)(bits >> 8);
    bytes[3] = (uint8_t)bits;
}

static int32_t add_long(int32_t left, int32_t right) {
    return (int32_t)((uint32_t)left + (uint32_t)right);
}

static int32_t subtract_long(int32_t left, int32_t right) {
    return (int32_t)((uint32_t)left - (uint32_t)right);
}

static int32_t asr_long(int32_t value, unsigned count) {
    if (value >= 0) return value >> count;
    return (int32_t)-((-(int64_t)value + (((int64_t)1 << count) - 1)) >> count);
}

static int32_t scale_scalar(int16_t value) {
    return (int32_t)((uint32_t)(int32_t)value << 3);
}

int fa18_run_record_scan_tail(FA18FlaggedSlot *slot,
                              const FA18RecordScanTailInput *input,
                              FA18RecordScanTailScalar scalar, void *context,
                              FA18RecordScanTailResult *result) {
    int16_t scalar_result[3];
    int16_t turn_offset;
    uint16_t flags;
    if (!slot || !input || !input->linked_record || !scalar || !result) return -1;
    switch (input->turn_word & 3u) {
    case 1: turn_offset = 0x10; break;
    case 3: turn_offset = -0x10; break;
    default: turn_offset = 0; break;
    }
    flags = be16(slot->bytes + 0x26);
    if (flags & 0x1000u)
        result->selector = 13;
    else if (flags & 0x2000u)
        result->selector = 5;
    else
        result->selector = (int16_t)((uint16_t)turn_offset + 0x00c6u +
                                     (uint16_t)asr_long(
                                         (int16_t)be16(input->linked_record->bytes + 0x6c),
                                         6));
    result->shifted_x = (int16_t)asr_long(
        subtract_long(add_long((int32_t)((uint32_t)input->origin_x & 0x003fffffu),
                               input->primary_x), be32(slot->bytes)), 8);
    result->shifted_y = (int16_t)asr_long(
        subtract_long(add_long(input->origin_y, input->primary_y),
                      be32(slot->bytes + 4)), 8);
    result->shifted_z = (int16_t)asr_long(
        subtract_long(add_long((int32_t)((uint32_t)input->origin_z & 0x003fffffu),
                               input->primary_z), be32(slot->bytes + 8)), 8);
    if (scalar(context, result->selector, result->shifted_x, result->shifted_y,
               result->shifted_z, scalar_result) != 0)
        return -1;
    put_be32(slot->bytes + 0x0c, scale_scalar(scalar_result[0]));
    put_be32(slot->bytes + 0x10, scale_scalar(scalar_result[1]));
    put_be32(slot->bytes + 0x14, scale_scalar(scalar_result[2]));
    put_be32(slot->bytes + 0x18, scalar_result[0]);
    put_be32(slot->bytes + 0x1c, scalar_result[1]);
    put_be32(slot->bytes + 0x20, scalar_result[2]);
    put_be16(slot->bytes + 0x24, 0);
    if (flags & 0x3000u) {
        put_be32(slot->bytes + 0x0c,
                 add_long(be32(slot->bytes + 0x0c), input->motion_x));
        put_be32(slot->bytes + 0x10,
                 add_long(be32(slot->bytes + 0x10), input->motion_y));
        put_be32(slot->bytes + 0x14,
                 add_long(be32(slot->bytes + 0x14), input->motion_z));
        put_be32(slot->bytes + 0x18,
                 add_long(be32(slot->bytes + 0x18), asr_long(input->motion_x, 3)));
        put_be32(slot->bytes + 0x1c,
                 add_long(be32(slot->bytes + 0x1c), asr_long(input->motion_y, 3)));
        put_be32(slot->bytes + 0x20,
                 add_long(be32(slot->bytes + 0x20), asr_long(input->motion_z, 3)));
    }
    return 0;
}
