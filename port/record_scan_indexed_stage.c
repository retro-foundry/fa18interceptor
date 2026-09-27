#include "record_scan_indexed_stage.h"

static uint16_t be16(const uint8_t *bytes) {
    return (uint16_t)((uint16_t)bytes[0] << 8 | bytes[1]);
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

static int32_t asr_long(int32_t value, unsigned count) {
    if (value >= 0) return value >> count;
    return (int32_t)-((-(int64_t)value + (((int64_t)1 << count) - 1)) >> count);
}

static int32_t transform_row(const int16_t *row, int16_t a, int16_t b,
                             int16_t c, unsigned shift) {
    int32_t value = (int32_t)row[0] * a;
    value = add_long(value, (int32_t)row[1] * b);
    value = add_long(value, (int32_t)row[2] * c);
    return asr_long(value, shift);
}

static int32_t transform_pair(int16_t left, int16_t right, int16_t a,
                              int16_t b, unsigned shift) {
    return asr_long(add_long((int32_t)left * a, (int32_t)right * b), shift);
}

int fa18_run_record_scan_indexed_stage(
    FA18FlaggedSlot *slot, uint16_t index,
    const FA18RecordScanIndexedStageInput *input,
    FA18RecordScanIndexedTail tail, void *context,
    FA18RecordScanIndexedStageResult *result) {
    int16_t a, b, c;
    uint16_t countdown;
    uint16_t flags;
    if (!slot || !input || !tail || !result) return -1;
    flags = be16(slot->bytes + 0x26);
    if (flags & 0x1000u) {
        a = -8;
        b = 0;
        c = 0;
        countdown = 0x001e;
    } else if (flags & 0x2000u) {
        a = -8;
        b = -56;
        c = 0;
        countdown = 0x003c;
    } else if (input->mode == 0x11u) {
        a = 0;
        b = 0;
        c = 56;
        countdown = 0x0014;
    } else {
        a = 6;
        b = 1;
        c = 24;
        countdown = 0x0014;
    }
    put_be16(slot->bytes + 0x28, countdown);
    result->primary_x = add_long(transform_row(input->matrix, a, b, c, 6),
                                 (int32_t)((uint32_t)input->origin_x & 0x003fffffu));
    result->primary_y = add_long(transform_row(input->matrix + 3, a, b, c, 6),
                                 input->origin_y);
    result->primary_z = add_long(transform_row(input->matrix + 6, a, b, c, 6),
                                 (int32_t)((uint32_t)input->origin_z & 0x003fffffu));
    put_be16(slot->bytes + 0x30, input->slot_word_30);
    put_be16(slot->bytes + 0x32, input->slot_word_32);
    put_be32(slot->bytes, result->primary_x);
    put_be32(slot->bytes + 4, result->primary_y);
    put_be32(slot->bytes + 8, result->primary_z);

    if (flags & 0x3000u) {
        a = -64;
        c = 0;
    } else {
        a = 64;
        c = 0x400;
        switch (input->turn_word & 3u) {
        case 0: a = 67; break;
        case 1: a = 61; break;
        case 2: a = 69; break;
        default: break;
        }
    }
    result->secondary_x = transform_pair(input->matrix[1], input->matrix[2], a, c, 4);
    result->secondary_y = transform_pair(input->matrix[4], input->matrix[5], a, c, 4);
    result->secondary_z = transform_pair(input->matrix[7], input->matrix[8], a, c, 4);
    if (tail(context, index) != 0) return -1;
    return 0;
}
