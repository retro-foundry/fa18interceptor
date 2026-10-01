/* $C2B05A: two source-backed record walks which update bits 1 and 2 of the
 * selected mutable record's flag byte. No renderer or chipset calls. */
#include "record_region_probe.h"

#include "memory.h"

#include <stdlib.h>

static void set_word(uint32_t *reg, uint16_t word) {
    *reg = (*reg & 0xffff0000u) | word;
}

static uint32_t swap(uint32_t value) {
    return (value << 16) | (value >> 16);
}

static uint32_t arithmetic_right(uint32_t value, unsigned count) {
    if (!count) return value;
    if (count >= 32) return (value & 0x80000000u) ? 0xffffffffu : 0u;
    return (value >> count) | ((value & 0x80000000u) ?
                               (0xffffffffu << (32 - count)) : 0u);
}

static uint32_t abs_long(uint32_t value) {
    return (int32_t)value < 0 ? 0u - value : value;
}

static uint32_t signed_product(uint32_t left, uint32_t right) {
    return (uint32_t)((int32_t)(int16_t)left * (int32_t)(int16_t)right);
}

static uint32_t divide_signed_word(uint32_t dividend, uint32_t divisor) {
    int16_t word = (int16_t)divisor;
    int32_t quotient;
    if (!word) abort(); /* The original DIVS faults on zero. */
    if (dividend == 0x80000000u && word == -1) return 0;
    quotient = (int32_t)dividend / word;
    if (quotient < -32768 || quotient > 32767) return dividend;
    return ((uint32_t)((int32_t)dividend % word) << 16) | (uint16_t)quotient;
}

static void set_record_bit(uint32_t record, unsigned bit, int enabled) {
    uint8_t flags = rd_u8(record + 4);
    flags = enabled ? (uint8_t)(flags | (1u << bit)) :
                      (uint8_t)(flags & ~(1u << bit));
    wr_u8(record + 4, flags);
}

/* $C2B0A8-$C2B230: count crossings for one table segment. */
static void count_segment(RecordRegionProbeRegisters *r, uint32_t record) {
    uint32_t *d = r->data, *a = r->address;
    uint32_t saved_d2, saved_d3, saved_d4, saved_d5;
    uint32_t delta_y;
    int shift;

    d[0] = rd_u32(record + 0x14) & 0x00ffffffu;
    d[2] = (uint32_t)(int32_t)rd_s16(a[3]);
    d[3] = (uint32_t)(int32_t)rd_s16(a[3] + 2);
    d[4] = (uint32_t)(int32_t)rd_s16(a[3] + 4);
    d[5] = (uint32_t)(int32_t)rd_s16(a[3] + 6);
    a[3] += 4;
    d[2] <<= 12;
    d[3] <<= 12;
    if ((int16_t)d[7] <= 1) {
        d[4] = (uint32_t)(int32_t)rd_s16(a[4]);
        d[5] = (uint32_t)(int32_t)rd_s16(a[4] + 2);
    }
    d[4] <<= 12;
    d[5] <<= 12;
    /* The source pushes D2-D5, then deliberately reloads those four words
     * into D2/D4-D6. */
    saved_d2 = d[2]; saved_d3 = d[3]; saved_d4 = d[4]; saved_d5 = d[5];
    d[4] -= d[2];
    d[5] -= d[3];
    a[0] = d[4];
    d[4] = arithmetic_right(d[4], 8);
    d[6] = d[4];
    if (!d[6]) return;
    d[6] = abs_long(d[6]);
    while ((int32_t)d[6] > 0x7fff) {
        d[6] = arithmetic_right(d[6], 1);
        if (!d[6]) return;
        d[4] = arithmetic_right(d[4], 1);
        d[5] = arithmetic_right(d[5], 1);
        a[0] = arithmetic_right(a[0], 1);
    }
    d[6] = (uint32_t)-8;
    delta_y = d[5];
    d[5] = a[2];
    a[2] = delta_y;
    d[5] = abs_long(delta_y);
    d[4] = abs_long(a[0]);

    /* Source compares the absolute slope against powers of two before DIVS.
     * D4 and D5 retain the selected scaled operands. */
    d[5] <<= 2;
    if ((int32_t)d[4] >= (int32_t)d[5]) shift = 8;
    else {
        d[5] = arithmetic_right(d[5], 1);
        if ((int32_t)d[4] >= (int32_t)d[5]) shift = 7;
        else {
            d[5] = arithmetic_right(d[5], 1);
            if ((int32_t)d[4] >= (int32_t)d[5]) shift = 6;
            else {
                d[4] <<= 1;
                if ((int32_t)d[4] >= (int32_t)d[5]) shift = 5;
                else {
                    d[4] <<= 1;
                    if ((int32_t)d[4] >= (int32_t)d[5]) shift = 4;
                    else {
                        d[4] <<= 1;
                        if ((int32_t)d[4] >= (int32_t)d[5]) shift = 3;
                        else {
                            d[4] <<= 1;
                            shift = (int32_t)d[4] >= (int32_t)d[5] ? 2 : 0;
                        }
                    }
                }
            }
        }
    }
    d[5] = a[2] << (shift == 0 ? 0 : shift == 2 ? 2 : shift == 3 ? 3 :
                     shift == 4 ? 4 : shift == 5 ? 5 : 6);
    d[4] = arithmetic_right(a[0], shift == 8 ? 10 : shift == 7 ? 9 : 8);
    if (shift) set_word(&d[6], (uint16_t)((uint16_t)d[6] - (uint16_t)shift));
    d[5] = divide_signed_word(d[5], d[4]);
    d[0] -= d[2];
    d[2] = abs_long(d[0]);
    while ((int32_t)d[2] > 0x7fff) {
        d[0] = arithmetic_right(d[0], 1);
        d[2] = arithmetic_right(d[2], 1);
        set_word(&d[6], (uint16_t)((uint16_t)d[6] + 1));
    }
    d[0] = signed_product(d[5], d[0]);
    if ((int16_t)d[6] < 0) {
        set_word(&d[6], (uint16_t)(0u - (uint16_t)d[6]));
        d[0] = arithmetic_right(d[0], (uint16_t)d[6] & 63u);
    } else {
        unsigned count = (uint16_t)d[6] & 63u;
        d[0] = count >= 32 ? 0 : d[0] << count;
    }
    d[3] += d[0];
    if ((int32_t)d[3] < (int32_t)d[1]) return;

    d[2] = saved_d2; d[4] = saved_d3; d[5] = saved_d4; d[6] = saved_d5;
    d[0] = abs_long((rd_u32(record + 0x14) & 0x00ffffffu) - d[2]);
    if ((int32_t)d[0] < 0x300) return;
    d[0] = abs_long((rd_u32(record + 0x14) & 0x00ffffffu) - d[5]);
    if ((int32_t)d[0] < 0x300) return;
    if (d[4] == d[6]) {
        if ((int32_t)d[5] < (int32_t)d[2]) {
            uint32_t tmp = d[2]; d[2] = d[5]; d[5] = tmp;
        }
        d[3] = rd_u32(record + 0x14) & 0x00ffffffu;
        if ((int32_t)d[3] <= (int32_t)d[2] ||
            (int32_t)d[3] >= (int32_t)d[5])
            return;
    } else {
        if ((int32_t)d[6] <= (int32_t)d[4]) {
            uint32_t tmp = d[4]; d[4] = d[6]; d[6] = tmp;
        }
        if ((int32_t)d[3] <= (int32_t)d[4] ||
            (int32_t)d[3] >= (int32_t)d[6])
            return;
    }
    set_word(&a[5], (uint16_t)((uint16_t)a[5] + 1));
}

/* $C2B05A-$C2B24A: directory lookup and crossing parity. */
static void probe_directory(RecordRegionProbeRegisters *r) {
    uint32_t *d = r->data, *a = r->address;
    uint32_t record = 0xc46184u + (uint32_t)(int32_t)rd_s16(0xc459b6u);

    a[1] = record;
    d[6] = rd_u32(record + 0x14);
    d[7] = rd_u32(record + 0x1c);
    d[1] = d[7] & 0x00ffffffu;
    d[6] = swap(d[6]); d[7] = swap(d[7]);
    set_word(&d[6], (uint16_t)d[6] >> 8);
    set_word(&d[7], (uint16_t)d[7] >> 8);
    a[3] = 0xc42e6cu;
    set_word(&d[6], (uint16_t)((uint16_t)d[6] * 2u));
    set_word(&d[7], (uint16_t)d[7] << 6);
    set_word(&d[6], (uint16_t)((uint16_t)d[6] + (uint16_t)d[7]));
    set_word(&d[7], rd_u16(a[3] + (uint32_t)(int32_t)(int16_t)d[6]));
    if ((int16_t)d[7] <= 0) {
        wr_u16(0xc4599eu, 0x41);
        set_record_bit(record, 1, 0);
        return;
    }
    a[3] += (uint32_t)(int32_t)(int16_t)d[7];
    if (rd_s16(a[3]) < 0) {
        set_record_bit(record, 1, 0);
        return;
    }
    a[3] += 4;
    for (;;) {
        set_word(&d[7], rd_u16(a[3]));
        a[3] += 2;
        if ((int16_t)d[7] <= 0) {
            if ((int16_t)d[7] == -1) {
                set_record_bit(record, 1, 0);
                return;
            }
            set_word(&d[7], rd_u16(a[3]));
            a[3] += 2;
        }
        a[4] = a[3];
        a[5] = 0;
        while ((int16_t)d[7] > 0) {
            count_segment(r, record);
            set_word(&d[7], (uint16_t)((uint16_t)d[7] - 1));
        }
        set_word(&d[0], (uint16_t)a[5]);
        if ((d[0] & 1u) != 0) {
            set_record_bit(record, 1, 1);
            return;
        }
    }
}

/* $C2B2D8-$C2B39C: four signed edge products of a 16-byte polygon record. */
static int polygon_contains_record(RecordRegionProbeRegisters *r,
                                   uint32_t record) {
    uint32_t *d = r->data, *a = r->address;
    unsigned edge;
    for (edge = 0; edge < 4; ++edge) {
        uint32_t start = a[4] + 4u * edge;
        uint32_t next = a[4] + 4u * ((edge + 1u) & 3u);
        d[5] = (uint32_t)(int32_t)rd_s16(next);
        d[6] = (uint32_t)(int32_t)rd_s16(next + 2);
        set_word(&d[5], (uint16_t)((uint16_t)d[5] - rd_u16(start)));
        set_word(&d[6], (uint16_t)((uint16_t)d[6] - rd_u16(start + 2)));
        set_word(&d[6], (uint16_t)(0u - (uint16_t)d[6]));
        d[3] = (uint32_t)(int32_t)rd_s16(start);
        d[4] = (uint32_t)(int32_t)rd_s16(start + 2);
        d[3] <<= 8; d[4] <<= 8;
        d[3] += d[1]; d[4] += d[2];
        d[3] -= rd_u32(record + 0x14);
        d[4] -= rd_u32(record + 0x1c);
        d[3] = arithmetic_right(d[3], 8);
        d[4] = arithmetic_right(d[4], 8);
        d[6] = signed_product(d[6], d[3]);
        d[5] = signed_product(d[5], d[4]);
        d[5] += d[6];
        if ((int32_t)d[5] < 0) return 0;
    }
    return 1;
}

/* $C2B24A-$C2B3B2: 16-byte table stride, then polygon edge tests. */
static void probe_shapes(RecordRegionProbeRegisters *r) {
    uint32_t *d = r->data, *a = r->address;
    uint32_t record = a[1];
    a[0] = 0xc42a96u;
    a[3] = 0xc22188u;
    d[0] = 0;
    for (;;) {
        uint32_t entry = a[0] + (uint32_t)(int32_t)(int16_t)d[0];
        d[1] = (uint32_t)(int32_t)rd_s16(entry + 2);
        d[2] = (uint32_t)(int32_t)rd_s16(entry + 4);
        d[3] = (uint32_t)(int32_t)rd_s16(entry + 6);
        d[4] = (uint32_t)(int32_t)rd_s16(entry + 8);
        d[5] = (uint32_t)(int32_t)rd_s16(entry + 10);
        if ((int16_t)d[1] == -1) {
            set_record_bit(record, 2, 0);
            return;
        }
        if ((int16_t)d[1] < 0) goto next_entry;
        d[1] = swap(d[1]) << 8;
        d[2] = swap(d[2]) << 8;
        set_word(&d[3], (uint16_t)d[3] << 2);
        a[4] = 0xc1d7e2u;
        d[6] = (uint32_t)(int32_t)rd_s16(a[4] + (uint32_t)(int32_t)(int16_t)d[3]);
        d[7] = (uint32_t)(int32_t)rd_s16(a[4] + (uint32_t)(int32_t)(int16_t)d[3] + 2);
        d[6] <<= 10; d[7] <<= 10;
        d[1] += d[6]; d[2] += d[7];
        d[4] <<= 10; d[5] <<= 10;
        d[1] += d[4]; d[2] += d[5];
        d[3] = abs_long(d[1] - rd_u32(record + 0x14));
        if ((int32_t)d[3] > 0x180000) goto next_entry;
        d[3] = arithmetic_right(d[2] - rd_u32(record + 0x1c), 8);
        d[3] = abs_long(d[3]);
        if ((int32_t)d[3] > 0x180000) goto next_entry;
        set_word(&d[5], rd_u16(entry));
        d[6] = rd_u32(a[3] + (uint32_t)(int32_t)(int16_t)d[5] + 0x10u);
        if ((int32_t)d[6] <= 0) goto next_entry;
        a[4] = d[6];
        while (rd_s16(a[4]) != -1) {
            if (polygon_contains_record(r, record)) {
                set_record_bit(record, 2, 1);
                return;
            }
            a[4] += 0x10u;
        }
next_entry:
        set_word(&d[0], (uint16_t)((uint16_t)d[0] + 0x10u));
    }
}

void probe_record_regions(RecordRegionProbeRegisters *registers) {
    probe_directory(registers);
    probe_shapes(registers);
}
