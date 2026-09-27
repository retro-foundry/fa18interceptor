#include "two_angle_matrix.h"
#include "run075_trig_asset.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    /* The test fixture is isolated to this contract binary. Production loads
     * the same table from Hunk 63 through fa18_load_two_angle_trig_table. */
    uint8_t hunk63_bytes[0xAE8 + sizeof fa18_run075_trig_bytes] = {0};
    memcpy(hunk63_bytes + 0xAE8, fa18_run075_trig_bytes,
           sizeof fa18_run075_trig_bytes);
    FA18HunkSegment segments[64] = {0};
    segments[63].data = hunk63_bytes;
    segments[63].size = sizeof hunk63_bytes;
    const FA18Hunks hunks = {segments, 64};
    FA18FlightTrigTable table = {0};
    assert(fa18_load_two_angle_trig_table(&hunks, &table) == 0);
    assert(table.bytes == hunk63_bytes + 0xAE8);
    assert(table.byte_count == sizeof fa18_run075_trig_bytes);

    int16_t matrix[3][3] = {{0}};
    assert(fa18_build_two_angle_matrix(&table, 0, 0, matrix) == 0);
    assert(matrix[0][0] == 0x100 && matrix[0][1] == 0 && matrix[0][2] == 0);
    assert(matrix[1][0] == 0 && matrix[1][1] == 0x100 && matrix[1][2] == 0);
    assert(matrix[2][0] == 0 && matrix[2][1] == 0 && matrix[2][2] == 0x100);
    assert(fa18_build_single_angle_trig_matrix(&table, 0, matrix) == 0);
    assert(matrix[0][0] == 0x4000 && matrix[0][1] == 0 && matrix[0][2] == 0);
    assert(matrix[1][0] == 0 && matrix[1][1] == 0x4000 && matrix[1][2] == 0);
    assert(matrix[2][0] == 0 && matrix[2][1] == 0 && matrix[2][2] == 0x4000);
    assert(fa18_build_single_angle_matrix(&table, 0, matrix) == 0);
    assert(matrix[0][0] == 0x100 && matrix[0][1] == 0 && matrix[0][2] == 0);
    assert(matrix[1][0] == 0 && matrix[1][1] == 0x100 && matrix[1][2] == 0);
    assert(matrix[2][0] == 0 && matrix[2][1] == 0 && matrix[2][2] == 0x100);
    assert(fa18_build_two_angle_matrix(NULL, 0, 0, matrix) == -1);
    assert(fa18_build_single_angle_matrix(NULL, 0, matrix) == -1);
    assert(fa18_build_single_angle_trig_matrix(NULL, 0, matrix) == -1);
    assert(fa18_load_two_angle_trig_table(NULL, &table) == -1);
    puts("two-angle matrix contract passed");
    return 0;
}
