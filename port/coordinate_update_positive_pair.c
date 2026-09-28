#include "coordinate_update_positive_pair.h"

#include "magnitude_refinement.h"
#include "rounded_signed_divide.h"

#include <limits.h>

static int32_t asr_long(int32_t value, unsigned count) {
    if (value >= 0) return value >> count;
    return (int32_t)-((-(int64_t)value + ((INT64_C(1) << count) - 1)) >> count);
}

static int lookup_word(const FA18CoordinateAngleTable *table, int16_t index,
                       int16_t *value) {
    size_t offset;
    if (!table || !table->bytes || !value || index < 0) return -1;
    offset = (size_t)(uint16_t)index * 2u;
    if (offset > table->byte_count || table->byte_count - offset < 2u) return -1;
    *value = (int16_t)fa18_be16(table->bytes + offset);
    return 0;
}

int fa18_update_coordinate_positive_pair(
    const FA18CoordinateAngleTable *table,
    const FA18SceneCoordinateUpdateInput *input, int16_t output[2]) {
    int16_t scaled_first, scaled_third, first_ratio, second_ratio;
    int16_t table_word;
    uint16_t magnitude;
    int32_t square_sum;
    int32_t first_output, second_output;

    if (!table || !input || !output || input->d0 || input->d1 || input->d3 ||
        input->d2 <= 0 || input->d4 <= 0 || input->d5 >= 0 ||
        input->d4 <= INT32_C(0x02000000))
        return -1;
    /* `$C124B2-$C12516`: largest positive component selects shift 14 and
     * the first scaled component is no larger than the third. */
    scaled_first = (int16_t)asr_long(input->d2, 14);
    scaled_third = (int16_t)asr_long(input->d4, 14);
    if (scaled_first > scaled_third ||
        fa18_round_signed_divide(input->d2, scaled_third, &first_ratio) != 0 ||
        lookup_word(table, (int16_t)(first_ratio >> 6), &table_word) != 0)
        return -1;
    first_output = (int32_t)table_word * 8;
    /* `$C125EE-$C125F8`: local mode zero complements the first table output. */
    first_output = INT32_C(0x7080) - first_output;

    /* `$C125FC-$C12680`: source-width sum of scaled squares, C2564E
     * magnitude refinement, and the second zero-component table ratio. */
    square_sum = (int32_t)scaled_first * scaled_first;
    square_sum = (int32_t)((uint32_t)square_sum +
                           (uint32_t)((int32_t)scaled_third * scaled_third));
    if (square_sum < 0 || fa18_refine_component_magnitude((uint32_t)square_sum,
                                                           &magnitude) != 0 ||
        !magnitude ||
        fa18_round_signed_divide(0, (int16_t)magnitude, &second_ratio) != 0 ||
        lookup_word(table, second_ratio, &table_word) != 0)
        return -1;
    second_output = INT32_C(0x7080) - (int32_t)table_word * 8;
    if (second_output >= INT32_C(0x7080)) second_output = 0;
    if (first_output < INT16_MIN || first_output > INT16_MAX ||
        second_output < INT16_MIN || second_output > INT16_MAX)
        return -1;
    output[0] = (int16_t)second_output;
    output[1] = (int16_t)first_output;
    return 0;
}
