#include "projection_grid.h"

#include <limits.h>
#include <stddef.h>

int fa18_load_projection_grid(const FA18Hunks *hunks, FA18ProjectionGrid *grid) {
    if (!hunks || !grid || FA18_C279_PROJECTION_GRID_HUNK >= hunks->count) return -1;
    const FA18HunkSegment *segment = &hunks->segments[FA18_C279_PROJECTION_GRID_HUNK];
    if (!segment->data ||
        segment->size < FA18_C279_PROJECTION_PAIR_SOURCE_20_OFFSET +
                            FA18_C279_PROJECTION_PAIR_SOURCE_BYTES ||
        segment->size < FA18_C279_PROJECTION_GRID_OFFSET + 4u ||
        FA18_C279_PROJECTION_BOUNDS_OFFSET + FA18_C279_PROJECTION_BOUNDS_BYTES >
            FA18_C279_PROJECTION_GRID_OFFSET)
        return -1;
    const uint8_t *data = segment->data + FA18_C279_PROJECTION_GRID_OFFSET;
    const uint16_t record_count = fa18_be16(data);
    const size_t records_size = (size_t)record_count * FA18_C279_PROJECTION_GRID_RECORD_BYTES;
    if (records_size > segment->size - FA18_C279_PROJECTION_GRID_OFFSET - 4u) return -1;
    grid->record_count = record_count;
    grid->bounds_limit = (int16_t)fa18_be16(data + 2u);
    grid->records = data + 4u;
    grid->bounds_table = segment->data + FA18_C279_PROJECTION_BOUNDS_OFFSET;
    grid->pair_sources[0] = segment->data + FA18_C279_PROJECTION_PAIR_SOURCE_12_OFFSET;
    grid->pair_sources[1] = segment->data + FA18_C279_PROJECTION_PAIR_SOURCE_16_OFFSET;
    grid->pair_sources[2] = segment->data + FA18_C279_PROJECTION_PAIR_SOURCE_20_OFFSET;
    return 0;
}

int fa18_projection_grid_pair_source(const FA18ProjectionGrid *grid, int16_t kind,
                                     FA18ProjectionPairInput pairs[3]) {
    if (!grid || !pairs || kind > -12 || kind < -20 || (kind & 3) != 0) return -1;
    const uint16_t source_index = (uint16_t)((-kind - 12) / 4);
    const uint8_t *source = grid->pair_sources[source_index];
    if (!source) return -1;
    for (uint16_t index = 0; index < 3; ++index) {
        pairs[index].x = (int16_t)fa18_be16(source + (size_t)index * 4u);
        pairs[index].y = (int16_t)fa18_be16(source + (size_t)index * 4u + 2u);
    }
    return 0;
}

int fa18_projection_grid_record(const FA18ProjectionGrid *grid, uint16_t index,
                                FA18ProjectionGridRecord *record) {
    if (!grid || !record || !grid->records || index >= grid->record_count) return -1;
    const uint8_t *data = grid->records + (size_t)index * FA18_C279_PROJECTION_GRID_RECORD_BYTES;
    record->x = (int16_t)fa18_be16(data);
    record->y = (int16_t)fa18_be16(data + 2u);
    record->kind = (int16_t)fa18_be16(data + 4u);
    return 0;
}

static int16_t normalize_grid_component(int16_t component) {
    const uint16_t difference = (uint16_t)(UINT16_C(0x0400) - (uint16_t)component);
    return (int16_t)((difference & UINT16_C(0xf800)) + (uint16_t)component);
}

int fa18_prepare_projection_grid(const FA18ProjectionGrid *grid,
                                 int16_t projection_input,
                                 int16_t component_x, int16_t component_y,
                                 FA18ProjectionGridSetup *setup) {
    if (!grid || !grid->records || !setup) return -1;
    if (projection_input < -128) return 1;
    setup->record_count = grid->record_count;
    setup->bounds_limit = grid->bounds_limit;
    setup->grid_x = normalize_grid_component(component_x);
    setup->grid_y = normalize_grid_component(component_y);
    setup->scaled_input = (int16_t)((uint16_t)projection_input << 3);
    setup->coordinate_shift = 3;
    return 0;
}

int fa18_initialize_projection_grid_packet(
    const FA18ProjectionGrid *grid, uint8_t packet_mode,
    int32_t projection_component, int16_t projection_input,
    int16_t component_x, int16_t component_y,
    FA18ProjectionGridPacketState *state, FA18ProjectionGridSetup *setup,
    FA18ProjectionGridPacketRoute *route) {
    if (!grid || !state || !setup || !route) return -1;
    state->renderer_state_words[0] = 4;
    state->renderer_state_words[1] = 0;
    state->renderer_state_words[2] = 0;
    state->renderer_state_words[3] = -1;
    state->renderer_selector = 3;
    if (packet_mode != 0) {
        *route = FA18_PROJECTION_GRID_PACKET_MODE_CONTINUATION;
        return 0;
    }
    if (projection_component < -2048) {
        *route = FA18_PROJECTION_GRID_PACKET_DEPTH_REJECT;
        return 0;
    }
    state->line_emitter_mode_flag = 1;
    state->negative_kind_flag = 0;
    if (projection_component < -512) {
        *route = FA18_PROJECTION_GRID_PACKET_LOWER_RANGE_CONTINUATION;
        return 0;
    }
    const int result = fa18_prepare_projection_grid(
        grid, projection_input, component_x, component_y, setup);
    if (result < 0) return -1;
    *route = result == 0 ? FA18_PROJECTION_GRID_PACKET_READY :
                           FA18_PROJECTION_GRID_PACKET_ALTERNATE_CONTINUATION;
    return 0;
}

int fa18_complete_projection_grid_packet(void *context) {
    FA18ProjectionGridPacketState *state = context;
    if (!state) return -1;
    state->line_emitter_mode_flag = 0;
    return 0;
}

static int16_t add_word(int16_t left, int16_t right) {
    return (int16_t)((uint16_t)left + (uint16_t)right);
}

static int16_t absolute_word(int16_t value) {
    return value < 0 ? (int16_t)(UINT16_C(0) - (uint16_t)value) : value;
}

static uint32_t multiply_word_bits(int16_t left, int16_t right) {
    return (uint32_t)((int32_t)left * (int32_t)right);
}

static uint32_t arithmetic_shift_right_8(uint32_t value) {
    if ((value & UINT32_C(0x80000000)) != 0)
        return (value >> 8) | UINT32_C(0xff000000);
    return value >> 8;
}

static int16_t matrix_pair_component(int16_t first_coefficient,
                                     int16_t second_coefficient,
                                     int16_t x, int16_t y, int16_t base) {
    const uint32_t sum = multiply_word_bits(first_coefficient, x) +
                         multiply_word_bits(second_coefficient, y);
    return (int16_t)(uint16_t)(arithmetic_shift_right_8(sum) + (uint16_t)base);
}

static int16_t multiply_word_shift_8(int16_t left, int16_t right) {
    return (int16_t)arithmetic_shift_right_8(multiply_word_bits(left, right));
}

int fa18_prepare_projection_grid_record(const FA18ProjectionGrid *grid,
                                        const FA18ProjectionGridSetup *setup,
                                        uint16_t record_index,
                                        int16_t negative_kind_flag,
                                        FA18ProjectionGridPreparedRecord *record) {
    FA18ProjectionGridRecord source;
    if (!grid || !setup || !record || !grid->bounds_table ||
        setup->coordinate_shift != 3 ||
        fa18_projection_grid_record(grid, record_index, &source) != 0)
        return -1;
    const int16_t x = add_word(source.x, setup->grid_x);
    const int16_t y = add_word(source.y, setup->grid_y);
    const int16_t x_bin = (int16_t)(absolute_word(x) >> 8);
    const int16_t y_bin = (int16_t)(absolute_word(y) >> 8);
    if (x_bin < 0 || y_bin < 0 || x_bin >= 32 || y_bin >= 32) return -1;
    const int16_t bound = (int8_t)grid->bounds_table[(uint16_t)y_bin * 32u + (uint16_t)x_bin];
    if (bound > setup->bounds_limit) return 0;

    int16_t kind = source.kind;
    if (kind < 0 && (negative_kind_flag != 0 || bound > 1))
        kind = kind == -12 ? 1 : 2;
    record->shifted_x = (int16_t)((uint16_t)x << setup->coordinate_shift);
    record->shifted_y = (int16_t)((uint16_t)y << setup->coordinate_shift);
    record->kind = kind;
    record->bound = bound;
    return 1;
}

int fa18_transform_projection_pair(const FA18ProjectionPairMatrix *matrix,
                                   const FA18ProjectionPairBase *base,
                                   const FA18ProjectionPairInput *input,
                                   FA18ProjectionPairOutput *output) {
    if (!matrix || !base || !input || !output) return -1;
    output->x = matrix_pair_component(matrix->words[0], matrix->words[2],
                                      input->x, input->y, base->x);
    output->y = matrix_pair_component(matrix->words[3], matrix->words[5],
                                      input->x, input->y, base->y);
    output->depth = matrix_pair_component(matrix->words[6], matrix->words[8],
                                          input->x, input->y, base->depth);
    return 0;
}

int fa18_prepare_projection_pair_base(const FA18ProjectionPairMatrix *matrix,
                                      int16_t scaled_input,
                                      FA18ProjectionPairBase *base) {
    if (!matrix || !base) return -1;
    base->x = multiply_word_shift_8(matrix->words[1], scaled_input);
    base->y = multiply_word_shift_8(matrix->words[4], scaled_input);
    base->depth = multiply_word_shift_8(matrix->words[7], scaled_input);
    return 0;
}

static int16_t negate_word(int16_t value) {
    return (int16_t)(UINT16_C(0) - (uint16_t)value);
}

static int divide_signed_long_by_word(int32_t dividend, int16_t divisor,
                                      int16_t *quotient) {
    if (!quotient || divisor == 0) return -1;
    const int32_t value = dividend / divisor;
    if (value < INT16_MIN || value > INT16_MAX) return -1;
    *quotient = (int16_t)value;
    return 0;
}

int fa18_project_projection_pair(const FA18ProjectionPairOutput *input,
                                 FA18ProjectionPairScreenPoint *point) {
    if (!input || !point) return -1;
    const int16_t depth = input->depth;
    if (depth <= 0 || input->x > depth || negate_word(input->x) > depth ||
        input->y > depth || negate_word(input->y) > depth)
        return 0;

    int16_t projected_x;
    int16_t projected_y;
    if (divide_signed_long_by_word((int32_t)input->x * 160, depth, &projected_x) != 0 ||
        divide_signed_long_by_word((int32_t)input->y * 90, depth, &projected_y) != 0)
        return -1;
    projected_x = add_word(projected_x, 160);
    projected_y = add_word(projected_y, 90);
    if (projected_x < 0 || projected_x >= 320 || projected_y < 0 || projected_y >= 180)
        return 0;
    point->x = negate_word(add_word(projected_x, -319));
    point->y = negate_word(add_word(projected_y, -179));
    return 1;
}

int fa18_project_projection_triangle(const FA18ProjectionPairMatrix *matrix,
                                     const FA18ProjectionPairBase *base,
                                     const FA18ProjectionGridPreparedRecord *translation,
                                     const FA18ProjectionPairInput pairs[3],
                                     FA18ProjectionTriangle *triangle) {
    if (!matrix || !base || !translation || !pairs || !triangle) return -1;
    FA18ProjectionTriangle projected = { 0 };
    for (uint16_t index = 0; index < 3; ++index) {
        FA18ProjectionPairInput translated = {
            add_word(pairs[index].x, translation->shifted_x),
            add_word(pairs[index].y, translation->shifted_y)
        };
        FA18ProjectionPairOutput transformed;
        if (fa18_transform_projection_pair(matrix, base, &translated, &transformed) != 0)
            return -1;
        const int status = fa18_project_projection_pair(&transformed,
                                                         &projected.points[index]);
        if (status != 1) return status;
    }
    *triangle = projected;
    return 1;
}

int fa18_emit_projection_grid_record(const FA18ProjectionGrid *grid,
                                     const FA18ProjectionPairMatrix *matrix,
                                     const FA18ProjectionPairBase *base,
                                     const FA18ProjectionGridPreparedRecord *record,
                                     int16_t direct_pair_mode_limit,
                                     FA18ProjectionGridEmission *emission) {
    if (!grid || !matrix || !base || !record || !emission) return -1;
    if (record->kind < 0) {
        FA18ProjectionPairInput pairs[3];
        if (fa18_projection_grid_pair_source(grid, record->kind, pairs) != 0)
            return -1;
        const int status = fa18_project_projection_triangle(matrix, base, record, pairs,
                                                            &emission->triangle);
        return status == 1 ? FA18_PROJECTION_GRID_TRIANGLE : status;
    }

    FA18ProjectionPairInput pair = { record->shifted_x, record->shifted_y };
    FA18ProjectionPairOutput transformed;
    if (fa18_transform_projection_pair(matrix, base, &pair, &transformed) != 0)
        return -1;
    const int status = fa18_project_projection_pair(&transformed, &emission->direct_pair);
    if (status != 1) return status;
    if (emission->direct_pair.y > direct_pair_mode_limit) return FA18_PROJECTION_GRID_SKIP;
    return record->kind == 2 ? FA18_PROJECTION_GRID_DIRECT_RENDERER_B :
                               FA18_PROJECTION_GRID_DIRECT_RENDERER_A;
}

int fa18_submit_projection_grid_direct_pair(
    int route, FA18ProjectionPairScreenPoint point,
    FA18ProjectionPairAxisEmitter primary_emitter,
    FA18ProjectionPairAxisEmitter adjacent_emitter, void *emitter_context) {
    if (route == FA18_PROJECTION_GRID_DIRECT_RENDERER_A) {
        if (!primary_emitter) return -1;
        return primary_emitter(emitter_context, point.x, point.y);
    }
    if (route == FA18_PROJECTION_GRID_DIRECT_RENDERER_B) {
        if (!adjacent_emitter) return -1;
        return adjacent_emitter(emitter_context, point.x, point.y);
    }
    return -1;
}

int fa18_submit_projection_grid_record(
    const FA18ProjectionGrid *grid, const FA18ProjectionPairMatrix *matrix,
    const FA18ProjectionPairBase *base,
    const FA18ProjectionGridPreparedRecord *record,
    int16_t direct_pair_mode_limit,
    const FA18ProjectionPairSubmission *submission,
    FA18ProjectionGridEmission *emission,
    FA18ProjectionPairBoundsRoute *bounds_route,
    FA18ProjectionPairFinalizationRoute *finalization_route) {
    const int route = fa18_emit_projection_grid_record(
        grid, matrix, base, record, direct_pair_mode_limit, emission);
    if (route != FA18_PROJECTION_GRID_TRIANGLE) return route;
    if (!submission || !bounds_route || !finalization_route ||
        fa18_submit_projection_pair_list(emission->triangle.points, 3,
                                         submission, bounds_route,
                                         finalization_route) != 0)
        return -1;
    return route;
}

int fa18_submit_projection_grid_pass(
    const FA18ProjectionGrid *grid, const FA18ProjectionGridSetup *setup,
    const FA18ProjectionPairMatrix *matrix, int16_t negative_kind_flag,
    int16_t direct_pair_mode_limit,
    const FA18ProjectionGridSubmission *submission,
    uint16_t *submitted_record_count) {
    if (!grid || !setup || !matrix || !submission || !submitted_record_count ||
        !submission->completion_emitter ||
        setup->record_count != grid->record_count)
        return -1;

    FA18ProjectionPairBase base;
    if (fa18_prepare_projection_pair_base(matrix, setup->scaled_input, &base) != 0)
        return -1;

    uint16_t submitted = 0;
    for (uint16_t record_index = 0; record_index < setup->record_count; ++record_index) {
        FA18ProjectionGridPreparedRecord record;
        const int prepared = fa18_prepare_projection_grid_record(
            grid, setup, record_index, negative_kind_flag, &record);
        if (prepared < 0) return -1;
        if (prepared == 0) continue;

        FA18ProjectionGridEmission emission;
        FA18ProjectionPairBoundsRoute bounds_route;
        FA18ProjectionPairFinalizationRoute finalization_route;
        const int route = fa18_submit_projection_grid_record(
            grid, matrix, &base, &record, direct_pair_mode_limit,
            submission->triangle_submission, &emission, &bounds_route,
            &finalization_route);
        if (route < 0) return -1;
        if (route == FA18_PROJECTION_GRID_TRIANGLE) {
            ++submitted;
            continue;
        }
        if (route == FA18_PROJECTION_GRID_DIRECT_RENDERER_A ||
            route == FA18_PROJECTION_GRID_DIRECT_RENDERER_B) {
            if (fa18_submit_projection_grid_direct_pair(
                    route, emission.direct_pair, submission->primary_emitter,
                    submission->adjacent_emitter, submission->emitter_context) != 0)
                return -1;
            ++submitted;
        }
    }
    if (submission->completion_emitter(submission->completion_context) != 0)
        return -1;
    *submitted_record_count = submitted;
    return 0;
}

int fa18_reduce_projection_pair_bounds(const FA18ProjectionPairScreenPoint *pairs,
                                       uint16_t count, FA18ProjectionPairBounds *bounds) {
    if (!pairs || !bounds || count < 3) return -1;
    FA18ProjectionPairBounds reduced = {
        pairs[0].x, pairs[0].x, pairs[0].y, pairs[0].y
    };
    for (uint16_t index = 1; index < count; ++index) {
        if (pairs[index].x < reduced.min_x) reduced.min_x = pairs[index].x;
        if (pairs[index].x > reduced.max_x) reduced.max_x = pairs[index].x;
        if (pairs[index].y < reduced.min_y) reduced.min_y = pairs[index].y;
        if (pairs[index].y > reduced.max_y) reduced.max_y = pairs[index].y;
    }
    *bounds = reduced;
    return 0;
}

static int16_t subtract_word(int16_t left, int16_t right) {
    return (int16_t)((uint16_t)left - (uint16_t)right);
}

static int16_t absolute_word_extent(int16_t extent) {
    return extent < 0 ? (int16_t)(UINT16_C(0) - (uint16_t)extent) : extent;
}

int fa18_select_projection_pair_bounds_route(const FA18ProjectionPairBounds *bounds,
                                             int16_t display_bound_y,
                                             FA18ProjectionPairBoundsRoute *route) {
    if (!bounds || !route) return -1;

    /* `$C3025A`: a minimum row beyond `$C45984` returns success directly. */
    if (bounds->min_y > display_bound_y) {
        *route = FA18_PROJECTION_PAIR_BOUNDS_RETURN;
        return 0;
    }

    const int16_t vertical_extent = absolute_word_extent(
        subtract_word(bounds->max_y, bounds->min_y));
    if (vertical_extent > 2) {
        *route = FA18_PROJECTION_PAIR_BOUNDS_C302DE_CONTINUATION;
        return 0;
    }

    const int16_t horizontal_extent = absolute_word_extent(
        subtract_word(bounds->max_x, bounds->min_x));
    if (vertical_extent > 1) {
        if (horizontal_extent > 2) {
            *route = FA18_PROJECTION_PAIR_BOUNDS_C302EC_CONTINUATION;
            return 0;
        }
        if ((int16_t)((uint16_t)bounds->min_y + 1u) > display_bound_y) {
            *route = FA18_PROJECTION_PAIR_BOUNDS_RETURN;
            return 0;
        }
        *route = FA18_PROJECTION_PAIR_BOUNDS_C2F66E_HELPER;
        return 0;
    }

    if (horizontal_extent <= 1) {
        *route = FA18_PROJECTION_PAIR_BOUNDS_C302C4_AXIS_STEP;
        return 0;
    }
    *route = FA18_PROJECTION_PAIR_BOUNDS_BLITTER_LINE_C2FA7E;
    return 0;
}

int fa18_submit_projection_pair_bounds(const FA18ProjectionPairBounds *bounds,
                                       int16_t display_bound_y,
                                       FA18ProjectionPairLineEmitter line_emitter,
                                       void *line_context,
                                       FA18ProjectionPairBoundsRoute *route) {
    if (fa18_select_projection_pair_bounds_route(bounds, display_bound_y, route) != 0)
        return -1;
    if (*route != FA18_PROJECTION_PAIR_BOUNDS_BLITTER_LINE_C2FA7E) return 0;
    if (!line_emitter) return -1;
    /* `$C302B6` is the only native renderer call admitted by this branch
     * selector. The caller adapter owns the source-proved line-plane state. */
    return line_emitter(line_context, bounds->min_x, bounds->min_y,
                        bounds->max_x, bounds->max_y, display_bound_y);
}

int fa18_submit_projection_pair_tuple_list(
    const FA18ProjectionPairScreenPoint *pairs, uint16_t count,
    int16_t display_bound_y, FA18ProjectionPairDmaEmitter dma_emitter,
    void *dma_context, FA18ProjectionPairLineEmitter line_emitter,
    void *line_context, FA18ProjectionPairBoundsRoute *route) {
    FA18ProjectionPairBounds bounds;

    if (!dma_emitter || !route) return -1;
    /* `$C2FF4A`: `move.w #$8400,$DFF096.l`, before the list is inspected. */
    if (dma_emitter(dma_context, UINT16_C(0x8400)) != 0) return -1;
    if (fa18_reduce_projection_pair_bounds(pairs, count, &bounds) != 0)
        return -1;
    return fa18_submit_projection_pair_bounds(&bounds, display_bound_y,
                                              line_emitter, line_context, route);
}

int fa18_submit_projection_pair_protected_line(int16_t d0, int16_t d1,
                                                int16_t d2, int16_t d3,
                                                uint8_t mode_flag,
                                                uint32_t saved_line_scratch,
                                                FA18ProjectionPairProtectedLineEmitter emitter,
                                                void *emitter_context) {
    if (!emitter) return -1;
    const uint32_t line_scratch = mode_flag ? saved_line_scratch : 0x000fffffu;
    /* The original restores `$C456E6` after C2FA7E returns; no caller state
     * is mutated across this opaque boundary. */
    return emitter(emitter_context, d0, d1, d2, d3, line_scratch);
}

int fa18_submit_projection_pair_axis_step(int16_t value_x, int16_t endpoint_x,
                                          int16_t y, int16_t remaining_distance,
                                          int16_t display_bound_y,
                                          FA18ProjectionPairAxisEmitter first_emitter,
                                          FA18ProjectionPairAxisEmitter second_emitter,
                                          void *emitter_context,
                                          int16_t *advanced_y,
                                          FA18ProjectionPairAxisRoute *route) {
    if (!advanced_y || !route) return -1;

    const int16_t next_y = (int16_t)((uint16_t)y + 1u);
    *advanced_y = next_y;
    if (next_y > display_bound_y) {
        *route = FA18_PROJECTION_PAIR_AXIS_RETURN;
        return 0;
    }

    if (subtract_word(remaining_distance, 1) < 0) {
        *route = FA18_PROJECTION_PAIR_AXIS_C2F5F4;
        if (!first_emitter) return -1;
        return first_emitter(emitter_context, value_x, next_y);
    }

    *route = FA18_PROJECTION_PAIR_AXIS_C2F60A;
    if (!second_emitter) return -1;
    return second_emitter(emitter_context, endpoint_x, next_y);
}

int fa18_prepare_projection_pair_range(int16_t d0, int16_t d1,
                                       int16_t d2, int16_t d3,
                                       int16_t display_bound_y,
                                       FA18ProjectionPairRangeState *state,
                                       FA18ProjectionPairRangeRoute *route) {
    if (!state || !route) return -1;
    if (d1 == d3) {
        *route = FA18_PROJECTION_PAIR_RANGE_EQUAL_RETURN;
        return 0;
    }
    /* `bls` after `cmp.w d1,d3` is an unsigned `d3 <= d1` test. */
    if ((uint16_t)d3 <= (uint16_t)d1) {
        *route = FA18_PROJECTION_PAIR_RANGE_C305D6_CONTINUATION;
        return 0;
    }

    const int16_t next_d1 = (int16_t)((uint16_t)d1 + 1u);
    if (next_d1 > display_bound_y) {
        *route = FA18_PROJECTION_PAIR_RANGE_EQUAL_RETURN;
        return 0;
    }

    const int16_t scaled_d1 = (int16_t)((uint16_t)next_d1 << 3);
    const int16_t scaled_d7 = (int16_t)((uint16_t)scaled_d1 * 5u);
    *state = (FA18ProjectionPairRangeState){
        (int16_t)((uint16_t)d0 >> 3),
        scaled_d1,
        d3,
        subtract_word(d2, d0),
        subtract_word(subtract_word(d3, d1), 1),
        d0,
        (int16_t)((uint16_t)scaled_d7 + ((uint16_t)d0 >> 3)),
        next_d1
    };
    *route = FA18_PROJECTION_PAIR_RANGE_C305F8_CONTINUATION;
    return 0;
}

static int16_t shift_left_word(int16_t value, unsigned count) {
    return (int16_t)((uint16_t)value << count);
}

static int16_t arithmetic_shift_right_word(int16_t value, unsigned count) {
    uint16_t shifted = (uint16_t)value >> count;
    if (value < 0) shifted = (uint16_t)(shifted | (uint16_t)(0xffffu << (16u - count)));
    return (int16_t)shifted;
}

static int16_t divs_word_result(int32_t dividend, int16_t divisor,
                                int16_t unchanged_low_word, int *fault) {
    if (!divisor) {
        *fault = 1;
        return unchanged_low_word;
    }
    const int64_t quotient = dividend / divisor;
    /* A 68000 DIVS overflow leaves the destination operand unchanged. */
    if (quotient < INT16_MIN || quotient > INT16_MAX) return unchanged_low_word;
    return (int16_t)quotient;
}

static int16_t bounded_pair_limit(int16_t d4, int16_t d5, int16_t a4,
                                  int16_t a1, int16_t d3, int *fault) {
    a4 = subtract_word(a4, a1);
    if (d5 <= a4) return d4;
    const int32_t dividend = (int32_t)(uint32_t)((int64_t)d4 * a4 * 2);
    return arithmetic_shift_right_word(divs_word_result(dividend, d5, d3, fault), 1);
}

int fa18_prepare_projection_pair_blitter_core(
                                         const FA18ProjectionPairBlitterCoreInput *input,
                                         FA18ProjectionPairBlitterWrites *writes,
                                         FA18ProjectionPairBlitterRoute *route) {
    if (!input || !writes || !route) return -1;

    int16_t d1 = 3;
    int16_t d2;
    int16_t d3 = input->d3;
    int16_t d4 = input->d4;
    int16_t d5 = input->d5;
    int16_t a4 = input->display_bound_y;
    const int16_t a1 = input->a1;
    const uint32_t d7 = (uint32_t)((int32_t)input->d7) + input->renderer_base_long;
    int16_t d6 = (int16_t)((uint16_t)input->d6 & 0x000fu);
    d6 = (int16_t)(((uint16_t)d6 >> 4) | ((uint16_t)d6 << 12));
    d6 = (int16_t)((uint16_t)d6 + 0x0b4au);

    int fault = 0;
    if (d4 < 0) {
        d4 = (int16_t)(UINT16_C(0) - (uint16_t)d4);
        if ((uint16_t)d4 < (uint16_t)d5) {
            d1 = (int16_t)((uint16_t)d1 + 8u);
            const int16_t swap = d4; d4 = d5; d5 = swap;
            a4 = subtract_word(a4, a1);
            if (d4 <= a4) a4 = d4;
        } else {
            d1 = (int16_t)((uint16_t)d1 + 0x14u);
            a4 = bounded_pair_limit(d4, d5, a4, a1, d3, &fault);
        }
    } else if ((uint16_t)d4 < (uint16_t)d5) {
        const int16_t swap = d4; d4 = d5; d5 = swap;
        a4 = subtract_word(a4, a1);
        if (d4 <= a4) a4 = d4;
    } else {
        d1 = (int16_t)((uint16_t)d1 + 0x10u);
        a4 = bounded_pair_limit(d4, d5, a4, a1, d3, &fault);
    }
    if (fault) return -1;

    d5 = shift_left_word(d5, 2);
    d4 = shift_left_word(d4, 1);
    d2 = subtract_word(d5, d4);
    if (d2 < 0) d1 = (int16_t)((uint16_t)d1 | 0x0040u);
    d3 = d5;
    d4 = shift_left_word(d4, 1);
    d5 = subtract_word(d5, d4);
    d4 = (int16_t)((uint16_t)shift_left_word(a4, 6) + 0x0042u);

    *writes = (FA18ProjectionPairBlitterWrites){
        (uint16_t)d6, (uint16_t)d1, 0xffffu, 0x8000u, 0xffffu,
        (uint16_t)d5, (uint16_t)d3, 0x0028u, 0x0028u, (uint16_t)d2,
        d7, d7, (uint16_t)d4
    };
    *route = FA18_PROJECTION_PAIR_BLITTER_C30668_SUBMIT;
    return 0;
}

int fa18_prepare_projection_pair_blitter(const FA18ProjectionPairBlitterInput *input,
                                         FA18ProjectionPairBlitterWrites *writes,
                                         FA18ProjectionPairBlitterRoute *route) {
    if (!input || !writes || !route) return -1;

    const int16_t next_d3 = (int16_t)((uint16_t)input->d3 + 1u);
    if (next_d3 > input->display_bound_y) {
        *route = FA18_PROJECTION_PAIR_BLITTER_RETURN;
        return 0;
    }
    const int16_t scaled_d3 = shift_left_word(next_d3, 3);
    const int16_t d7 = (int16_t)((uint16_t)((uint16_t)scaled_d3 * 5u) +
                                 ((uint16_t)input->d2 >> 3));
    const FA18ProjectionPairBlitterCoreInput core = {
        scaled_d3,
        subtract_word(input->d0, input->d2),
        subtract_word(subtract_word(input->d1, input->d3), 1),
        input->d2,
        d7,
        next_d3,
        input->display_bound_y,
        input->renderer_base_long
    };
    return fa18_prepare_projection_pair_blitter_core(&core, writes, route);
}

int fa18_submit_projection_pair_range_blitter(
    const FA18ProjectionPairRangeState *state, int16_t display_bound_y,
    uint32_t renderer_base_long, FA18ProjectionPairBlitterWrites *writes,
    FA18ProjectionPairBlitterRoute *route) {
    if (!state || !writes || !route) return -1;
    const FA18ProjectionPairBlitterCoreInput core = {
        state->d3, state->d4, state->d5, state->d6, state->d7, state->a1,
        display_bound_y, renderer_base_long
    };
    return fa18_prepare_projection_pair_blitter_core(&core, writes, route);
}

int fa18_submit_projection_pair_range(int16_t d0, int16_t d1,
                                      int16_t d2, int16_t d3,
                                      int16_t display_bound_y,
                                      uint32_t renderer_base_long,
                                      FA18ProjectionPairBlitterWrites *writes,
                                      FA18ProjectionPairRangeRoute *range_route,
                                      FA18ProjectionPairBlitterRoute *blitter_route) {
    if (!writes || !range_route || !blitter_route) return -1;
    FA18ProjectionPairRangeState state;
    if (fa18_prepare_projection_pair_range(d0, d1, d2, d3, display_bound_y,
                                           &state, range_route) != 0)
        return -1;
    if (*range_route == FA18_PROJECTION_PAIR_RANGE_EQUAL_RETURN) {
        *blitter_route = FA18_PROJECTION_PAIR_BLITTER_RETURN;
        return 0;
    }
    if (*range_route == FA18_PROJECTION_PAIR_RANGE_C305D6_CONTINUATION) {
        const FA18ProjectionPairBlitterInput input = {
            d0, d1, d2, d3, display_bound_y, renderer_base_long
        };
        return fa18_prepare_projection_pair_blitter(&input, writes, blitter_route);
    }
    return fa18_submit_projection_pair_range_blitter(&state, display_bound_y,
                                                      renderer_base_long, writes,
                                                      blitter_route);
}

static uint32_t replace_low_word(uint32_t value, int16_t word) {
    return (value & 0xffff0000u) | (uint16_t)word;
}

int fa18_finalize_projection_pair_blit(const FA18ProjectionPairFinalInput *input,
                                       FA18ProjectionPairFinalState *state) {
    if (!input || !state) return -1;

    /* `movem.w $C4597C,d1/d3` consumes the first two words in memory order. */
    int16_t d3 = arithmetic_shift_right_word(input->saved_second_word, 4);
    int16_t d2 = d3;
    int16_t d1_word = arithmetic_shift_right_word(input->saved_first_word, 4);
    d3 = subtract_word(d3, d1_word);
    int16_t d6 = d3;
    d3 = subtract_word(0x0027, shift_left_word(d3, 1));

    const int16_t vertical = input->vertical_value;
    const int16_t scaled_vertical = shift_left_word(vertical, 3);
    const int16_t forty_vertical = (int16_t)((uint16_t)scaled_vertical +
                                             (uint16_t)shift_left_word(scaled_vertical, 2));
    uint32_t d1 = replace_low_word((uint32_t)(int32_t)input->saved_first_word,
                                   forty_vertical);
    d2 = shift_left_word(d2, 1);
    d1 = d1 + (uint32_t)(int32_t)d2;
    const uint32_t base_offset = d1;

    int16_t d0 = 0;
    uint32_t d7 = 0;
    if (vertical > input->display_bound_y) {
        const int16_t delta = subtract_word(vertical, input->display_bound_y);
        d0 = delta;
        const int16_t scaled_delta = shift_left_word(delta, 3);
        const int16_t forty_delta = (int16_t)((uint16_t)scaled_delta +
                                              (uint16_t)shift_left_word(scaled_delta, 2));
        d7 = (uint32_t)(int32_t)forty_delta;
        d1 = d1 - d7;
    }

    int16_t d5 = subtract_word(vertical, input->horizontal_value);
    d5 = (int16_t)((uint16_t)d5 + 1u);
    d6 = (int16_t)((uint16_t)d6 + 1u);
    int16_t blit_size = subtract_word(d5, d0);
    blit_size = shift_left_word(blit_size, 6);
    blit_size = (int16_t)((uint16_t)blit_size + (uint16_t)d6);

    const uint32_t lane = input->renderer_base_long + base_offset - d7;
    *state = (FA18ProjectionPairFinalState){
        d1, lane, lane, (uint16_t)blit_size,
        0x09f0u, 0x000au, lane, 0xffffffffu, lane,
        (uint16_t)d3, (uint16_t)d3, (uint16_t)d3
    };
    return 0;
}

int fa18_finalize_projection_pair_list(
    const FA18ProjectionPairFinalizationInput *input,
    FA18ProjectionPairRangeEmitter range_emitter, void *emitter_context,
    FA18ProjectionPairFinalState *state,
    FA18ProjectionPairFinalizationRoute *route) {
    if (!input || !state || !route || !input->pairs || input->pair_count < 2)
        return -1;
    if (input->d6 <= 1) {
        *route = FA18_PROJECTION_PAIR_FINALIZATION_C3029E_FALLBACK;
        return 0;
    }
    if (!range_emitter) return -1;

    int16_t saved_first = subtract_word(input->d0, 1);
    if (saved_first < 0) saved_first = 0;
    /* `$C302F2` exchanges D1/D2 before the four-word workspace store. */
    const int16_t saved_second = input->d2;

    for (uint16_t index = 0; index + 1 < input->pair_count; ++index) {
        const FA18ProjectionPairScreenPoint first = input->pairs[index];
        const FA18ProjectionPairScreenPoint second = input->pairs[index + 1];
        if (range_emitter(emitter_context, first.x, first.y, second.x, second.y,
                          input->display_bound_y) != 0)
            return -1;
    }
    const FA18ProjectionPairScreenPoint first = input->pairs[input->pair_count - 1];
    const FA18ProjectionPairScreenPoint second = input->pairs[0];
    if (range_emitter(emitter_context, first.x, first.y, second.x, second.y,
                      input->display_bound_y) != 0)
        return -1;

    const FA18ProjectionPairFinalInput final_input = {
        saved_first, saved_second, input->vertical_value, input->horizontal_value,
        input->display_bound_y, input->renderer_base_long
    };
    if (fa18_finalize_projection_pair_blit(&final_input, state) != 0) return -1;
    *route = FA18_PROJECTION_PAIR_FINALIZATION_SUBMITTED;
    return 0;
}

typedef struct {
    int16_t display_bound_y;
    uint32_t renderer_base_long;
    FA18ProjectionPairBlitterEmitter emitter;
    void *emitter_context;
} ProjectionPairFarRangeContext;

static int submit_projection_pair_far_range(void *context,
                                            int16_t d0, int16_t d1,
                                            int16_t d2, int16_t d3,
                                            int16_t display_bound_y) {
    ProjectionPairFarRangeContext *far = context;
    FA18ProjectionPairBlitterWrites writes;
    FA18ProjectionPairRangeRoute range_route;
    FA18ProjectionPairBlitterRoute blitter_route;

    if (!far || display_bound_y != far->display_bound_y) return -1;
    if (fa18_submit_projection_pair_range(d0, d1, d2, d3, display_bound_y,
                                          far->renderer_base_long, &writes,
                                          &range_route, &blitter_route) != 0)
        return -1;
    if (blitter_route != FA18_PROJECTION_PAIR_BLITTER_C30668_SUBMIT) return 0;
    return far->emitter(far->emitter_context, &writes);
}

int fa18_submit_projection_pair_far_list(
    const FA18ProjectionPairScreenPoint *pairs, uint16_t count,
    int16_t display_bound_y, int16_t vertical_value, int16_t horizontal_value,
    uint32_t renderer_base_long, uint8_t mode_flag,
    uint32_t saved_line_scratch,
    FA18ProjectionPairProtectedLineEmitter protected_line_emitter,
    void *protected_line_context,
    FA18ProjectionPairBlitterEmitter blitter_emitter,
    FA18ProjectionPairFinalEmitter final_emitter, void *emitter_context,
    FA18ProjectionPairBoundsRoute *bounds_route,
    FA18ProjectionPairFinalizationRoute *finalization_route) {
    FA18ProjectionPairBounds bounds;
    FA18ProjectionPairFinalizationInput finalization_input;
    FA18ProjectionPairFinalState final_state;
    ProjectionPairFarRangeContext range_context;
    int16_t d6;

    if (!blitter_emitter || !final_emitter || !bounds_route || !finalization_route)
        return -1;
    if (fa18_reduce_projection_pair_bounds(pairs, count, &bounds) != 0 ||
        fa18_select_projection_pair_bounds_route(&bounds, display_bound_y,
                                                 bounds_route) != 0)
        return -1;
    if (*bounds_route != FA18_PROJECTION_PAIR_BOUNDS_C302DE_CONTINUATION &&
        *bounds_route != FA18_PROJECTION_PAIR_BOUNDS_C302EC_CONTINUATION)
        return 0;

    /* `$C302DE` overwrites D6 with D2-D0; `$C302EC` retains the preceding
     * nonnegative horizontal extent. Both subsequently enter `$C302E6`. */
    d6 = subtract_word(bounds.max_x, bounds.min_x);
    if (*bounds_route == FA18_PROJECTION_PAIR_BOUNDS_C302EC_CONTINUATION && d6 < 0)
        d6 = (int16_t)(UINT16_C(0) - (uint16_t)d6);

    range_context = (ProjectionPairFarRangeContext){
        display_bound_y, renderer_base_long, blitter_emitter, emitter_context
    };
    finalization_input = (FA18ProjectionPairFinalizationInput){
        bounds.min_x, bounds.min_y, bounds.max_x, bounds.max_y, d6,
        pairs, count, display_bound_y, vertical_value, horizontal_value,
        renderer_base_long
    };
    if (fa18_finalize_projection_pair_list(&finalization_input,
                                           submit_projection_pair_far_range,
                                           &range_context, &final_state,
                                           finalization_route) != 0)
        return -1;
    if (*finalization_route != FA18_PROJECTION_PAIR_FINALIZATION_SUBMITTED) {
        if (!protected_line_emitter) return -1;
        /* `$C302E6` branches to `$C3029E` before `$C302EC` decrements D0. */
        return fa18_submit_projection_pair_protected_line(
            bounds.min_x, bounds.min_y, bounds.max_x, bounds.max_y,
            mode_flag, saved_line_scratch, protected_line_emitter,
            protected_line_context);
    }
    return final_emitter(emitter_context, &final_state);
}

int fa18_submit_projection_pair_list(
    const FA18ProjectionPairScreenPoint *pairs, uint16_t count,
    const FA18ProjectionPairSubmission *submission,
    FA18ProjectionPairBoundsRoute *bounds_route,
    FA18ProjectionPairFinalizationRoute *finalization_route) {
    FA18ProjectionPairBounds bounds;

    if (!submission || !submission->dma_emitter || !bounds_route ||
        !finalization_route)
        return -1;
    /* `$C2FF4A`: this write precedes the `$C301F6` entry on every route. */
    if (submission->dma_emitter(submission->dma_context, UINT16_C(0x8400)) != 0)
        return -1;
    if (fa18_reduce_projection_pair_bounds(pairs, count, &bounds) != 0 ||
        fa18_select_projection_pair_bounds_route(&bounds,
                                                 submission->display_bound_y,
                                                 bounds_route) != 0)
        return -1;

    if (*bounds_route == FA18_PROJECTION_PAIR_BOUNDS_BLITTER_LINE_C2FA7E) {
        *finalization_route = FA18_PROJECTION_PAIR_FINALIZATION_C3029E_FALLBACK;
        return fa18_submit_projection_pair_bounds(
            &bounds, submission->display_bound_y, submission->line_emitter,
            submission->line_context, bounds_route);
    }
    if (*bounds_route != FA18_PROJECTION_PAIR_BOUNDS_C302DE_CONTINUATION &&
        *bounds_route != FA18_PROJECTION_PAIR_BOUNDS_C302EC_CONTINUATION) {
        *finalization_route = FA18_PROJECTION_PAIR_FINALIZATION_C3029E_FALLBACK;
        return 0;
    }
    return fa18_submit_projection_pair_far_list(
        pairs, count, submission->display_bound_y, submission->vertical_value,
        submission->horizontal_value, submission->renderer_base_long,
        submission->mode_flag, submission->saved_line_scratch,
        submission->protected_line_emitter, submission->protected_line_context,
        submission->blitter_emitter,
        submission->final_emitter, submission->blitter_context, bounds_route,
        finalization_route);
}
