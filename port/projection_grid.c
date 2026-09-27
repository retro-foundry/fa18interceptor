#include "projection_grid.h"

#include <stddef.h>

int fa18_load_projection_grid(const FA18Hunks *hunks, FA18ProjectionGrid *grid) {
    if (!hunks || !grid || FA18_C279_PROJECTION_GRID_HUNK >= hunks->count) return -1;
    const FA18HunkSegment *segment = &hunks->segments[FA18_C279_PROJECTION_GRID_HUNK];
    if (!segment->data || segment->size < FA18_C279_PROJECTION_GRID_OFFSET + 4u ||
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
