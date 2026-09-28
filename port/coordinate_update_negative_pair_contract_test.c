#include "coordinate_update_negative_pair.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static void store_word(uint8_t *bytes, size_t index, int16_t value) {
    bytes[index * 2] = (uint8_t)((uint16_t)value >> 8);
    bytes[index * 2 + 1] = (uint8_t)value;
}

int main(void) {
    uint8_t bytes[128] = {0};
    FA18CoordinateAngleTable table = {bytes, sizeof bytes};
    FA18CoordinateNegativePairInput input = {-18370, -3072, 1828, -1};
    FA18CoordinateNegativePairOutput output = {0};
    FA18HunkSegment segments[64] = {0};
    FA18Hunks hunks = {segments, 64};

    /* The bounded `$C2D9BA` trace reaches quotient indexes zero and 43.
     * Production reads these words from original Hunk 63, never a capture. */
    store_word(bytes, 0, 0);
    store_word(bytes, 43, 845);
    assert(fa18_update_coordinate_negative_pair(&table, &input, &output) == 0);
    assert(output.output_x == 6760 && output.output_z == 7200 &&
           output.status_flag == 1);
    input.first_component = 1;
    assert(fa18_update_coordinate_negative_pair(&table, &input, &output) == -1);
    input.first_component = -18370;
    input.terminal_component = 0;
    assert(fa18_update_coordinate_negative_pair(&table, &input, &output) == -1);
    input.terminal_component = -1;
    table.byte_count = 2;
    assert(fa18_update_coordinate_negative_pair(&table, &input, &output) == -1);

    segments[63].data = bytes;
    segments[63].size = sizeof bytes;
    assert(fa18_load_coordinate_angle_table(&hunks, &table) == 0);
    assert(table.bytes == bytes && table.byte_count == sizeof bytes);
    assert(fa18_load_coordinate_angle_table(NULL, &table) == -1);
    puts("coordinate negative-pair contract passed");
    return 0;
}
