/* $C2B05A: directory crossings and placed-polygon region membership. */
#include "record_region_probe.h"
#include "globals.h"

#include <stdlib.h>

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

static void set_record_bit(gaddr record, unsigned bit, int enabled) {
    uint8_t flags = rd_u8(record + 4);
    flags = enabled ? (uint8_t)(flags | (1u << bit)) :
                      (uint8_t)(flags & ~(1u << bit));
    wr_u8(record + 4, flags);
}

/* $C2B0A8-$C2B230: fixed-point segment/ray crossing. Keep the source's
 * adaptive DIVS scaling, endpoint exclusion and strict bound comparisons. */
static RegionSegmentProbe probe_segment(gaddr record, gaddr vertex,
                                        gaddr next, uint32_t point_y) {
    RegionSegmentProbe probe = {REGION_SEGMENT_NORMALIZED, 0, 0, 0, 0, 0, 0};
    uint32_t point_x = rd_u32(record + 0x14) & 0x00ffffffu;
    uint32_t start_x = (uint32_t)(int32_t)rd_s16(vertex) << 12;
    uint32_t start_y = (uint32_t)(int32_t)rd_s16(vertex + 2) << 12;
    uint32_t end_x = (uint32_t)(int32_t)rd_s16(next) << 12;
    uint32_t end_y = (uint32_t)(int32_t)rd_s16(next + 2) << 12;
    uint32_t delta_x = end_x - start_x, delta_y = end_y - start_y;
    uint32_t x_operand = arithmetic_right(delta_x, 8), y_operand;
    uint32_t offset, intersection_y;
    int shift;

    probe.magnitude = x_operand;
    if (!probe.magnitude) return probe;
    probe.magnitude = abs_long(probe.magnitude);
    while ((int32_t)probe.magnitude > 0x7fff) {
        probe.magnitude = arithmetic_right(probe.magnitude, 1);
        if (!probe.magnitude) return probe;
        x_operand = arithmetic_right(x_operand, 1);
        delta_y = arithmetic_right(delta_y, 1);
        delta_x = arithmetic_right(delta_x, 1);
    }
    probe.stage = REGION_SEGMENT_PROJECTED;
    probe.scale = -8;
    probe.delta_y = delta_y;
    y_operand = abs_long(delta_y) << 2;
    x_operand = abs_long(delta_x);
    if ((int32_t)x_operand >= (int32_t)y_operand) shift = 8;
    else {
        y_operand = arithmetic_right(y_operand, 1);
        if ((int32_t)x_operand >= (int32_t)y_operand) shift = 7;
        else {
            y_operand = arithmetic_right(y_operand, 1);
            if ((int32_t)x_operand >= (int32_t)y_operand) shift = 6;
            else {
                for (shift = 5; shift >= 2; --shift) {
                    x_operand <<= 1;
                    if ((int32_t)x_operand >= (int32_t)y_operand) break;
                }
                if (shift < 2) shift = 0;
            }
        }
    }
    y_operand = delta_y << (shift >= 6 ? 6 : shift);
    x_operand = arithmetic_right(delta_x, shift == 8 ? 10 : shift == 7 ? 9 : 8);
    probe.scale = (int16_t)(probe.scale - shift);
    y_operand = divide_signed_word(y_operand, x_operand);
    offset = point_x - start_x;
    x_operand = abs_long(offset);
    while ((int32_t)x_operand > 0x7fff) {
        offset = arithmetic_right(offset, 1);
        x_operand = arithmetic_right(x_operand, 1);
        ++probe.scale;
    }
    offset = signed_product(y_operand, offset);
    if (probe.scale < 0) {
        probe.scale = (int16_t)-probe.scale;
        offset = arithmetic_right(offset, (uint16_t)probe.scale & 63u);
    } else {
        unsigned count = (uint16_t)probe.scale & 63u;
        offset = count >= 32 ? 0 : offset << count;
    }
    intersection_y = start_y + offset;
    if ((int32_t)intersection_y < (int32_t)point_y) return probe;
    probe.stage = REGION_SEGMENT_BOUNDED;
    probe.upper_y = end_y;
    if ((int32_t)abs_long(point_x - start_x) < 0x300 ||
        (int32_t)abs_long(point_x - end_x) < 0x300) {
        probe.endpoint_excluded = 1;
        return probe;
    }
    if (start_y == end_y) {
        if ((int32_t)end_x < (int32_t)start_x) {
            uint32_t tmp = start_x; start_x = end_x; end_x = tmp;
        }
        if ((int32_t)point_x <= (int32_t)start_x ||
            (int32_t)point_x >= (int32_t)end_x) return probe;
    } else {
        if ((int32_t)end_y <= (int32_t)start_y) {
            uint32_t tmp = start_y; start_y = end_y; end_y = tmp;
        }
        probe.upper_y = end_y;
        if ((int32_t)intersection_y <= (int32_t)start_y ||
            (int32_t)intersection_y >= (int32_t)end_y) return probe;
    }
    probe.crossed = 1;
    return probe;
}

static void probe_directory(gaddr record, const RecordRegionProbeHooks *hooks) {
    uint32_t x = rd_u32(record + 0x14), y = rd_u32(record + 0x1c);
    uint16_t index = (uint16_t)(((x >> 24) * 2u) + ((y >> 24) << 6));
    int16_t offset = rd_s16(0xc42e6cu + (gaddr)(int32_t)(int16_t)index);
    gaddr cursor = 0xc42e6cu;
    if (hooks && hooks->directory)
        hooks->directory(hooks->context, record, x, y, index, offset);
    if (offset <= 0) {
        wr_u16(0xc4599eu, 0x41);
        set_record_bit(record, 1, 0);
        return;
    }
    cursor += (gaddr)(int32_t)offset;
    if (rd_s16(cursor) < 0) {
        set_record_bit(record, 1, 0);
        return;
    }
    cursor += 4;
    for (;;) {
        int16_t count = rd_s16(cursor);
        gaddr first;
        uint16_t crossings = 0;
        cursor += 2;
        if (hooks && hooks->remaining) hooks->remaining(hooks->context, count);
        if (count <= 0) {
            if (count == -1) {
                set_record_bit(record, 1, 0);
                return;
            }
            count = rd_s16(cursor);
            cursor += 2;
            if (hooks && hooks->remaining) hooks->remaining(hooks->context, count);
        }
        first = cursor;
        if (hooks && hooks->group) hooks->group(hooks->context, first, crossings);
        while (count > 0) {
            RegionSegmentProbe segment = probe_segment(record, cursor,
                count > 1 ? cursor + 4 : first, y & 0x00ffffffu);
            cursor += 4;
            if (hooks && hooks->segment) hooks->segment(hooks->context, &segment);
            crossings = (uint16_t)(crossings + segment.crossed);
            if (hooks && hooks->group) hooks->group(hooks->context, first, crossings);
            /* $C2B1E4/$C2B1FE branch to $C2B050, abandoning this entire
             * directory walk, rather than continuing with its next edge. */
            if (segment.endpoint_excluded) {
                set_record_bit(record, 1, 0);
                return;
            }
            --count;
            if (hooks && hooks->remaining) hooks->remaining(hooks->context, count);
        }
        if (hooks && hooks->group) hooks->group(hooks->context, first, crossings);
        if (crossings & 1u) {
            set_record_bit(record, 1, 1);
            return;
        }
    }
}

static int polygon_contains_record(gaddr record, gaddr polygon,
                                   const RegionShapePosition *position,
                                   const RecordRegionProbeHooks *hooks) {
    unsigned edge;
    for (edge = 0; edge < 4; ++edge) {
        gaddr start = polygon + 4u * edge;
        gaddr next = polygon + 4u * ((edge + 1u) & 3u);
        uint16_t dx = (uint16_t)(rd_u16(next) - rd_u16(start));
        uint16_t negative_dy = (uint16_t)(rd_u16(start + 2) - rd_u16(next + 2));
        RegionShapeEdge result;
        result.relative_x = arithmetic_right(
            ((uint32_t)(int32_t)rd_s16(start) << 8) + position->origin[0] -
                rd_u32(record + 0x14), 8);
        result.relative_y = arithmetic_right(
            ((uint32_t)(int32_t)rd_s16(start + 2) << 8) + position->origin[1] -
                rd_u32(record + 0x1c), 8);
        result.x_product = signed_product(negative_dy, result.relative_x);
        result.cross_product = signed_product(dx, result.relative_y) + result.x_product;
        if (hooks && hooks->edge) hooks->edge(hooks->context, &result);
        if ((int32_t)result.cross_product < 0) return 0;
    }
    return 1;
}

static void probe_shapes(gaddr record, const RecordRegionProbeHooks *hooks) {
    uint16_t offset = 0;
    for (;;) {
        gaddr entry = 0xc42a96u + (gaddr)(int32_t)(int16_t)offset;
        int16_t fields[5];
        RegionShapePosition position;
        uint32_t magnitude;
        int16_t selector;
        gaddr polygon;
        unsigned i;
        for (i = 0; i < 5; ++i) fields[i] = rd_s16(entry + 2u + 2u * i);
        if (hooks && hooks->shape) hooks->shape(hooks->context, offset, fields);
        if (fields[0] == -1) {
            set_record_bit(record, 2, 0);
            return;
        }
        if (fields[0] < 0) goto next_entry;
        position.adjustment_index = (int16_t)(uint16_t)((uint16_t)fields[2] << 2);
        for (i = 0; i < 2; ++i) {
            position.adjustment[i] = (uint32_t)(int32_t)rd_s16(GRID_ADJUST_WORDS +
                (gaddr)(int32_t)position.adjustment_index + 2u * i) << 10;
            position.offset[i] = (uint32_t)(int32_t)fields[3 + i] << 10;
            /* SWAP after sign extension retains $FFFF in the low word
             * for a negative field; the following ASL.L #8 keeps $FFFF00. */
            position.origin[i] = (((uint32_t)(uint16_t)fields[i] << 24) |
                (fields[i] < 0 ? 0x00ffff00u : 0u)) +
                position.adjustment[i] + position.offset[i];
        }
        if (hooks && hooks->position) hooks->position(hooks->context, &position);
        magnitude = abs_long(position.origin[0] - rd_u32(record + 0x14));
        if (hooks && hooks->distance) hooks->distance(hooks->context, magnitude);
        if ((int32_t)magnitude > 0x180000) goto next_entry;
        magnitude = abs_long(arithmetic_right(position.origin[1] - rd_u32(record + 0x1c), 8));
        if (hooks && hooks->distance) hooks->distance(hooks->context, magnitude);
        if ((int32_t)magnitude > 0x180000) goto next_entry;
        selector = rd_s16(entry);
        polygon = rd_u32(SCENE_POINTERS + (gaddr)(int32_t)selector + 0x10u);
        if (hooks && hooks->stream) hooks->stream(hooks->context, selector, polygon);
        if ((int32_t)polygon <= 0) goto next_entry;
        for (;;) {
            if (hooks && hooks->polygon) hooks->polygon(hooks->context, polygon);
            if (rd_s16(polygon) == -1) break;
            if (polygon_contains_record(record, polygon, &position, hooks)) {
                set_record_bit(record, 2, 1);
                return;
            }
            polygon += 0x10u;
        }
next_entry:
        offset = (uint16_t)(offset + 0x10u);
    }
}

void probe_record_regions(const RecordRegionProbeHooks *hooks) {
    gaddr record = CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(0xc459b6u);
    probe_directory(record, hooks);
    probe_shapes(record, hooks);
}
