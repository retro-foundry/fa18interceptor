/* Complete $C279D0-$C27D23. Table identity and record kinds retain their
 * source meaning; the original chooses every coordinate, cutoff and helper. */
#include "grid_projection_packet.h"
#include "globals.h"
#include <stdlib.h>

static int16_t add_word(int16_t left, int16_t right) {
    return (int16_t)((uint16_t)left + (uint16_t)right);
}
static int16_t negate_word(int16_t value) { return (int16_t)(0u - (uint16_t)value); }
static uint32_t low_word(uint32_t value, int16_t word) {
    return (value & 0xffff0000u) | (uint16_t)word;
}
static uint32_t signed_product(int16_t left, int16_t right) {
    return (uint32_t)((int32_t)left * right);
}
static uint32_t right_eight(uint32_t value) { return (uint32_t)((int32_t)value >> 8); }
static int16_t align_component(int16_t component) {
    return add_word(component, (int16_t)((uint16_t)(0x400u - (uint16_t)component) & 0xf800u));
}
static uint32_t project_component(int16_t value, int16_t scale, int16_t depth,
                                  int16_t maximum) {
    uint32_t product = signed_product(value, scale), divided = product;
    int32_t quotient = (int32_t)product / depth;
    int16_t screen;
    if (quotient >= -32768 && quotient <= 32767)
        divided = ((uint32_t)(uint16_t)((int32_t)product % depth) << 16) | (uint16_t)quotient;
    screen = add_word((int16_t)divided, scale);
    if (screen < 0) screen = 0;
    else if (screen >= maximum + 1) screen = maximum;
    return low_word(divided, add_word(maximum, negate_word(screen)));
}

static GridProjectionPoint transform_point(const GridProjectionSetup *setup,
                                           int16_t x, int16_t y) {
    GridProjectionPoint point = {0};
    uint32_t horizontal_first = signed_product(rd_s16(VIEW_ANGLE_MATRIX), x);
    uint32_t horizontal_second = signed_product(rd_s16(VIEW_ANGLE_MATRIX + 4), y);
    uint32_t vertical_second = signed_product(rd_s16(VIEW_ANGLE_MATRIX + 10), y);
    uint32_t depth_second = signed_product(rd_s16(VIEW_ANGLE_MATRIX + 16), y);
    int16_t depth, vertical;
    int32_t depth_sum;
    point.horizontal_component = add_word((int16_t)right_eight(horizontal_first + horizontal_second), setup->base[0]);
    point.vertical_first_product = signed_product(rd_s16(VIEW_ANGLE_MATRIX + 6), x);
    point.vertical = right_eight(point.vertical_first_product + vertical_second);
    point.vertical = low_word(point.vertical, add_word((int16_t)point.vertical, setup->base[1]));
    point.depth_first_product = signed_product(rd_s16(VIEW_ANGLE_MATRIX + 12), x);
    point.depth = right_eight(point.depth_first_product + depth_second);
    depth_sum = (int32_t)(int16_t)point.depth + setup->base[2];
    point.depth = low_word(point.depth, (int16_t)depth_sum);
    depth = (int16_t)point.depth; vertical = (int16_t)point.vertical;
    /* BLE follows ADD.W, so its overflow flag tests the signed sum before
     * word wrapping. Later CMP.W instructions compare the wrapped depth. */
    if (depth_sum <= 0 || depth == 0) return point;
    point.comparisons = 1;
    if (point.horizontal_component > depth) return point;
    point.comparisons = 2;
    if (negate_word(point.horizontal_component) > depth) return point;
    point.comparisons = 3;
    if (vertical > depth) return point;
    point.comparisons = 4;
    if (negate_word(vertical) > depth) return point;
    point.comparisons = 5;
    point.screen_x = project_component(point.horizontal_component, 160, depth, 319);
    point.screen_y = project_component(vertical, 90, depth, 179);
    point.accepted = 1;
    return point;
}

void draw_grid_projection_packet(gaddr frame, const GridProjectionHooks *hooks) {
    GridProjectionSetup setup = {0};
    uint8_t mode = rd_u8(ATTITUDE_BAND);
    int32_t threshold = mode == 0 ? -2048 : mode == 1 ? -3456 : -4608;
    int32_t depth = rd_s32(PROJECTION_Y);
    int16_t input = (int16_t)depth;
    gaddr table, pair_table;
    int16_t negative_kind_flag = depth < -512;
    unsigned i;
    int more_records;
    if (!hooks || !hooks->triangle || !hooks->pixel) abort();
    wr_u16(LINE_STYLE, 4); wr_u16(LINE_STYLE + 2, 0);
    wr_u16(LINE_STYLE + 4, 0); wr_u16(LINE_STYLE + 6, 0xffff);
    wr_u16(CURRENT_COLOUR, 3);
    if (hooks->gate) hooks->gate(hooks->context, mode, threshold, depth);
    if (depth < threshold) goto complete;
    wr_u8(KEEP_LINE_STYLE, 1);
    wr_s16(frame - 0x22, negative_kind_flag);
    wr_s16(frame - 6, rd_s16(PROJECTION_WORDS)); wr_s16(frame - 4, input);
    wr_s16(frame - 2, rd_s16(PROJECTION_WORDS + 4));
    setup.shift = input >= -128 ? 3 : 0;
    table = input >= -128 ? 0xc28124u : input >= -224 ? 0xc28368u : 0xc2854cu;
    setup.count = rd_s16(table); setup.bounds_limit = rd_s16(table + 2);
    setup.records = table + 4;
    wr_s16(frame - 0x16, setup.shift);
    wr_s16(frame - 8, setup.count); wr_s16(frame - 0x1a, setup.bounds_limit);
    setup.component_x = align_component(rd_s16(PROJECTION_WORDS));
    setup.component_y = align_component(rd_s16(PROJECTION_WORDS + 4));
    setup.scaled_input = (int16_t)((uint16_t)input << setup.shift);
    pair_table = setup.shift ? 0xc286dcu : 0xc286d0u;
    wr_s16(frame - 6, setup.component_x); wr_s16(frame - 2, setup.component_y);
    if (setup.shift) wr_s16(frame - 4, setup.scaled_input);
    wr_u32(frame - 0xc, pair_table); wr_u32(frame - 0x10, pair_table + 24);
    wr_u32(frame - 0x14, pair_table + 48);
    for (i = 0; i < 3; ++i) {
        setup.base_products[i] = right_eight(signed_product(rd_s16(VIEW_ANGLE_MATRIX + 2 + 6u * i), setup.scaled_input));
        setup.base[i] = (int16_t)setup.base_products[i];
        wr_s16(frame - 0x20 + 2u * i, setup.base[i]);
    }
    wr_u16(POLY_VERTICES, 3);
    if (hooks->setup) hooks->setup(hooks->context, &setup);
    /* The source visits a record before testing the decremented count. */
    do {
        GridProjectionRecord record = {0};
        int16_t absolute_x, absolute_y;
        gaddr cursor = setup.records;
        record.next_record = cursor + 6;
        record.source_x = rd_s16(cursor); record.source_y = rd_s16(cursor + 2);
        record.source_kind = rd_s16(cursor + 4); record.kind = record.source_kind;
        wr_s16(frame - 0x18, record.kind);
        record.x = add_word(record.source_x, setup.component_x);
        record.y = add_word(record.source_y, setup.component_y);
        absolute_x = record.x < 0 ? negate_word(record.x) : record.x;
        absolute_y = record.y < 0 ? negate_word(record.y) : record.y;
        record.bounds_index = add_word((int16_t)((uint16_t)(absolute_y >> 8) << 5), (int16_t)(absolute_x >> 8));
        record.bound = rd_s8(0xc27d24u + (gaddr)(int32_t)record.bounds_index);
        record.admitted = record.bound <= setup.bounds_limit;
        if (record.admitted && record.kind < 0 && (negative_kind_flag || record.bound > 1))
            record.kind = record.kind == -12 ? 1 : 2;
        if (record.kind != record.source_kind) wr_s16(frame - 0x18, record.kind);
        record.shifted_x = (int16_t)((uint16_t)record.x << setup.shift);
        record.shifted_y = (int16_t)((uint16_t)record.y << setup.shift);
        if (hooks->record) hooks->record(hooks->context, &setup, &record);
        if (record.admitted) {
            if (record.kind < 0) {
                gaddr pairs = rd_u32(frame + (gaddr)(int32_t)record.kind);
                gaddr output = POLY_VERTICES + 2;
                for (i = 0; i < 3; ++i) {
                    GridProjectionPoint point = transform_point(&setup,
                        add_word(rd_s16(pairs), record.shifted_x),
                        add_word(rd_s16(pairs + 2), record.shifted_y));
                    pairs += 4; point.next_pair = pairs; point.output = output;
                    point.triangle = 1;
                    if (point.accepted) {
                        wr_u16(output, (uint16_t)point.screen_x);
                        wr_u16(output + 2, (uint16_t)point.screen_y);
                    }
                    if (hooks->point) hooks->point(hooks->context, &setup, &record, &point);
                    if (!point.accepted) break;
                    output += 4;
                }
                if (i == 3) hooks->triangle(hooks->context);
            } else {
                GridProjectionPoint point = transform_point(&setup, record.shifted_x, record.shifted_y);
                if (hooks->point) hooks->point(hooks->context, &setup, &record, &point);
                if (point.accepted && (int16_t)point.screen_y <= rd_s16(LINE_LAST_ROW))
                    hooks->pixel(hooks->context, (int16_t)point.screen_x, (int16_t)point.screen_y, record.kind == 2);
            }
        }
        setup.records = record.next_record;
        /* BGT follows SUBQ.W: a negative count that wraps positive still
         * exits because of overflow, after retaining the wrapped count. */
        more_records = setup.count > 1;
        setup.count = add_word(setup.count, -1);
        wr_s16(frame - 8, setup.count);
    } while (more_records);
complete:
    wr_u8(KEEP_LINE_STYLE, 0);
}
