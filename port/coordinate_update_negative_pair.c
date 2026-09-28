#include "coordinate_update_negative_pair.h"

#include "magnitude_refinement.h"
#include "rounded_signed_divide.h"

#include <limits.h>

enum { COORDINATE_TABLE_HUNK = 63 };

static int32_t negate_long(int32_t value) {
    return (int32_t)(UINT32_C(0) - (uint32_t)value);
}

static int32_t asr_long(int32_t value, unsigned count) {
    if (value >= 0) return value >> count;
    return (int32_t)-((-(int64_t)value + ((INT64_C(1) << count) - 1)) >> count);
}

static int16_t asr_word(int16_t value, unsigned count) {
    if (value >= 0) return (int16_t)(value >> count);
    return (int16_t)-((-(int32_t)value + ((1 << count) - 1)) >> count);
}

static int lookup_word(const FA18CoordinateAngleTable *table, int16_t index,
                       int16_t *value) {
    size_t offset;
    if (!table || !table->bytes || !value || index < 0) return -1;
    offset = (size_t)(uint16_t)index * 2u;
    if (offset > table->byte_count || table->byte_count - offset < 2) return -1;
    *value = (int16_t)fa18_be16(table->bytes + offset);
    return 0;
}

int fa18_load_coordinate_angle_table(const FA18Hunks *hunks,
                                     FA18CoordinateAngleTable *table) {
    const FA18HunkSegment *segment;
    if (!hunks || !table || !hunks->segments || hunks->count <= COORDINATE_TABLE_HUNK)
        return -1;
    segment = &hunks->segments[COORDINATE_TABLE_HUNK];
    if (!segment->data || !segment->size) return -1;
    table->bytes = segment->data;
    table->byte_count = segment->size;
    return 0;
}

int fa18_update_coordinate_negative_pair(
    const FA18CoordinateAngleTable *table,
    const FA18CoordinateNegativePairInput *input,
    FA18CoordinateNegativePairOutput *output) {
    int32_t first, second;
    int16_t scaled_first, scaled_second, scaled_third;
    int16_t first_ratio, second_ratio, table_word;
    uint16_t magnitude;
    int32_t square_sum;

    if (!table || !input || !output || input->first_component >= 0 ||
        input->second_component >= 0 || input->third_component < 0 ||
        input->terminal_component >= 0)
        return -1;
    first = negate_long(input->first_component);
    second = negate_long(input->second_component);
    if (first <= second || first <= input->third_component ||
        first > INT32_C(0x200000))
        return -1;

    scaled_first = (int16_t)asr_long(first, 8);
    scaled_third = (int16_t)asr_long(input->third_component, 8);
    if (scaled_first <= scaled_third ||
        /* `$C12570` reloads the unscaled `$18(a6)` component before ASL #6. */
        fa18_round_signed_divide(input->third_component << 6, scaled_first,
                                 &first_ratio) != 0 ||
        lookup_word(table, asr_word(first_ratio, 6), &table_word) != 0)
        return -1;
    output->output_z = (int16_t)(uint16_t)((int32_t)(0x0384 - table_word) * 8);

    square_sum = (int32_t)scaled_third * scaled_third;
    square_sum = (int32_t)((uint32_t)square_sum +
                           (uint32_t)((int32_t)scaled_first * scaled_first));
    if (square_sum < 0 || fa18_refine_component_magnitude((uint32_t)square_sum,
                                                           &magnitude) != 0)
        return -1;
    scaled_second = (int16_t)asr_long(second, 8);
    if (scaled_second > (int16_t)magnitude ||
        fa18_round_signed_divide((int32_t)scaled_second << 8, (int16_t)magnitude,
                                 &second_ratio) != 0 ||
        lookup_word(table, second_ratio, &table_word) != 0)
        return -1;
    output->output_x = (int16_t)(uint16_t)((int32_t)table_word * 8);
    output->status_flag = 1;
    return 0;
}
