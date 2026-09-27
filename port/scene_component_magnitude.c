#include "scene_component_magnitude.h"

#include <limits.h>

int fa18_load_scene_magnitude_table(const FA18Hunks *hunks,
                                    FA18LoadedSceneMagnitudeTable *table) {
    if (!hunks || !table || FA18_SCENE_MAGNITUDE_HUNK >= hunks->count) return -1;
    const FA18HunkSegment *segment = &hunks->segments[FA18_SCENE_MAGNITUDE_HUNK];
    const size_t bytes = FA18_SCENE_MAGNITUDE_WORDS * 2u;
    if (!segment->data || segment->size < FA18_SCENE_MAGNITUDE_OFFSET ||
        segment->size - FA18_SCENE_MAGNITUDE_OFFSET < bytes)
        return -1;
    const uint8_t *source = segment->data + FA18_SCENE_MAGNITUDE_OFFSET;
    for (unsigned index = 0; index != FA18_SCENE_MAGNITUDE_WORDS; ++index)
        table->words[index] = fa18_be16(source + index * 2u);
    return 0;
}

static int32_t arithmetic_shift_right_long(int32_t value, unsigned count) {
    if (!count) return value;
    if (value >= 0) return value >> count;
    const int64_t magnitude = -(int64_t)value;
    return (int32_t)-((magnitude + ((INT64_C(1) << count) - 1)) >> count);
}

static int16_t arithmetic_shift_right_word(int16_t value, unsigned count) {
    return (int16_t)arithmetic_shift_right_long(value, count);
}

/* Return one for DIVU overflow, matching the unchanged destination register. */
static int divu_word(uint32_t dividend, uint16_t divisor, uint32_t *destination) {
    if (!divisor || !destination) return -1;
    const uint32_t quotient = dividend / divisor;
    if (quotient > UINT16_MAX) return 1;
    *destination = (*destination & UINT32_C(0xffff0000)) | quotient;
    return 0;
}

static int table_word(const FA18SceneMagnitudeTable *table, int16_t byte_offset,
                      uint16_t *word) {
    if (!table || !table->words || !word || byte_offset < 0 ||
        ((uint16_t)byte_offset & 1u))
        return -1;
    const size_t index = (uint16_t)byte_offset / 2u;
    if (index >= table->count) return -1;
    *word = table->words[index];
    return 0;
}

int fa18_scene_component_magnitude(const FA18SceneMagnitudeTable *table,
                                   int16_t component_0,
                                   int16_t component_1,
                                   int16_t component_2,
                                   int16_t *result) {
    if (!table || !result) return -1;
    int16_t d2 = component_0;
    int32_t d3 = component_1;
    int16_t d4 = component_2;
    uint16_t lookup;

    if ((int16_t)d3 > d2) {
        const int16_t swap = d2;
        d2 = (int16_t)d3;
        d3 = swap;
    }
    d3 = (int16_t)d3;
    if (d3) {
        d3 = (int32_t)((uint32_t)d3 << 8);
        if (d2) {
            uint32_t d3_register = (uint32_t)d3;
            const int division = divu_word(d3_register, (uint16_t)d2,
                                           &d3_register);
            if (division < 0) return -1;
            d3 = (int32_t)d3_register;
        } else {
            d3 = 0;
        }
        d3 = (int16_t)((uint16_t)d3 << 1);
    }
    if (table_word(table, (int16_t)d3, &lookup) != 0) return -1;
    int32_t planar = (int32_t)(uint16_t)d2 * lookup;

    int32_t vertical = (int16_t)d4;
    vertical = (int32_t)((uint32_t)vertical << 14);
    if (vertical > planar) {
        const int32_t swap = planar;
        planar = vertical;
        vertical = swap;
    }
    planar = arithmetic_shift_right_long(planar, 14);
    if ((int16_t)planar) {
        uint32_t vertical_register = (uint32_t)vertical;
        const int division = divu_word(vertical_register, (uint16_t)planar,
                                       &vertical_register);
        if (division < 0) return -1;
        vertical = (int32_t)vertical_register;
        vertical = arithmetic_shift_right_word((int16_t)vertical, 6);
        vertical = (int16_t)((uint16_t)vertical << 1);
    } else {
        vertical = 0;
    }
    if (table_word(table, (int16_t)vertical, &lookup) != 0) return -1;
    const int32_t scalar = arithmetic_shift_right_long(
        (int32_t)(uint16_t)planar * lookup, 14);
    *result = scalar > INT16_MAX ? INT16_MAX : (int16_t)scalar;
    return 0;
}
