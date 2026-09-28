#include "polygon_clip_pipeline.h"

enum { FA18_POLYGON_CLIP_SCRATCH_CAPACITY = 64 };

typedef enum {
    FA18_CLIP_Y_POSITIVE,
    FA18_CLIP_Y_NEGATIVE,
    FA18_CLIP_X_POSITIVE,
    FA18_CLIP_X_NEGATIVE
} FA18ClipBoundary;

static int16_t word_add(int16_t left, int16_t right) {
    return (int16_t)(uint16_t)((uint16_t)left + (uint16_t)right);
}

static int16_t word_subtract(int16_t left, int16_t right) {
    return (int16_t)(uint16_t)((uint16_t)left - (uint16_t)right);
}

static int16_t word_negate(int16_t value) {
    return (int16_t)(uint16_t)(UINT16_C(0) - (uint16_t)value);
}

static int16_t divide_round_word(int32_t dividend, int16_t divisor) {
    const int32_t quotient = dividend / divisor;
    const int32_t remainder = dividend % divisor;
    const int32_t magnitude = remainder < 0 ? -remainder : remainder;
    const int32_t half_divisor =
        (divisor < 0 ? -(int32_t)divisor : divisor) / 2;

    if (magnitude <= half_divisor) return (int16_t)(uint16_t)quotient;
    return (int16_t)(uint16_t)(quotient + (quotient < 0 ? -1 : 1));
}

static int16_t boundary_component(FA18ClipTuple tuple, FA18ClipBoundary boundary) {
    switch (boundary) {
    case FA18_CLIP_Y_POSITIVE: return tuple.y;
    case FA18_CLIP_Y_NEGATIVE: return word_negate(tuple.y);
    case FA18_CLIP_X_POSITIVE: return tuple.x;
    case FA18_CLIP_X_NEGATIVE: return word_negate(tuple.x);
    }
    return 0;
}

static int is_inside(FA18ClipTuple tuple, FA18ClipBoundary boundary) {
    return boundary_component(tuple, boundary) <= tuple.z;
}

static FA18ClipTuple intersect(FA18ClipTuple previous, FA18ClipTuple current,
                               FA18ClipBoundary boundary) {
    const int16_t previous_component = boundary_component(previous, boundary);
    const int16_t current_component = boundary_component(current, boundary);
    const int16_t previous_distance = word_subtract(previous_component, previous.z);
    const int16_t current_distance = word_subtract(current_component, current.z);
    const int16_t denominator = word_subtract(current_distance, previous_distance);
    const int16_t numerator = word_negate(previous_distance);
    FA18ClipTuple result;

    result.x = word_add(previous.x, divide_round_word(
        (int32_t)word_subtract(current.x, previous.x) * numerator, denominator));
    result.y = word_add(previous.y, divide_round_word(
        (int32_t)word_subtract(current.y, previous.y) * numerator, denominator));
    switch (boundary) {
    case FA18_CLIP_Y_POSITIVE: result.z = result.y; break;
    case FA18_CLIP_Y_NEGATIVE: result.z = word_negate(result.y); break;
    case FA18_CLIP_X_POSITIVE: result.z = result.x; break;
    case FA18_CLIP_X_NEGATIVE: result.z = word_negate(result.x); break;
    }
    return result;
}

static int append_tuple(FA18ClipTuple tuple, FA18ClipTuple *output,
                        size_t capacity, uint16_t *count) {
    if (*count >= capacity || *count == UINT16_MAX) return -1;
    output[*count] = tuple;
    ++*count;
    return 0;
}

static int clip_boundary(const FA18ClipTuple *input, uint16_t input_count,
                         FA18ClipBoundary boundary, FA18ClipTuple *output,
                         size_t output_capacity, uint16_t *output_count) {
    if (!input || !input_count || !output || !output_count) return -1;
    *output_count = 0;
    const FA18ClipTuple first = input[0];
    const int first_inside = is_inside(first, boundary);
    FA18ClipTuple previous = first;
    int previous_inside = is_inside(previous, boundary);

    /* `$C2469E` records the first tuple before walking its remaining inputs,
     * then its four closures process the final-to-first edge.  Retaining that
     * order matters: the source output starts at the first in-loop crossing,
     * not at the closing edge. */
    if (previous_inside && append_tuple(previous, output, output_capacity,
                                        output_count) != 0)
        return -1;
    for (uint16_t index = 1; index < input_count; ++index) {
        const FA18ClipTuple current = input[index];
        const int current_inside = is_inside(current, boundary);
        if (current_inside != previous_inside &&
            append_tuple(intersect(previous, current, boundary), output,
                         output_capacity, output_count) != 0)
            return -1;
        if (current_inside && append_tuple(current, output, output_capacity,
                                           output_count) != 0)
            return -1;
        previous = current;
        previous_inside = current_inside;
    }
    if (first_inside != previous_inside &&
        append_tuple(intersect(previous, first, boundary), output,
                     output_capacity, output_count) != 0)
        return -1;
    return 0;
}

int fa18_clip_projection_polygon(const FA18ClipTuple *input, uint16_t input_count,
                                 int16_t coordinate_shift, FA18ClipTuple *output,
                                 size_t output_capacity, uint16_t *output_count) {
    FA18ClipTuple first[FA18_POLYGON_CLIP_SCRATCH_CAPACITY];
    FA18ClipTuple second[FA18_POLYGON_CLIP_SCRATCH_CAPACITY];
    uint16_t first_count;
    uint16_t second_count;

    if (!input || !input_count || !output || !output_count || coordinate_shift < 0 ||
        coordinate_shift >= 16 || input_count > FA18_POLYGON_CLIP_SCRATCH_CAPACITY)
        return -1;
    for (uint16_t index = 0; index < input_count; ++index) {
        first[index] = (FA18ClipTuple){
            (int16_t)(uint16_t)((uint16_t)input[index].x << coordinate_shift),
            (int16_t)(uint16_t)((uint16_t)input[index].y << coordinate_shift),
            (int16_t)(uint16_t)((uint16_t)input[index].z << coordinate_shift)
        };
    }
    first_count = input_count;
    if (clip_boundary(first, first_count, FA18_CLIP_Y_POSITIVE, second,
                      FA18_POLYGON_CLIP_SCRATCH_CAPACITY, &second_count) != 0 ||
        !second_count ||
        clip_boundary(second, second_count, FA18_CLIP_Y_NEGATIVE, first,
                      FA18_POLYGON_CLIP_SCRATCH_CAPACITY, &first_count) != 0 ||
        !first_count ||
        clip_boundary(first, first_count, FA18_CLIP_X_POSITIVE, second,
                      FA18_POLYGON_CLIP_SCRATCH_CAPACITY, &second_count) != 0 ||
        !second_count)
        return -1;
    return clip_boundary(second, second_count, FA18_CLIP_X_NEGATIVE, output,
                         output_capacity, output_count);
}
