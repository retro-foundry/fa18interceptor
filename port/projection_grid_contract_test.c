#include "projection_grid.h"

#include <assert.h>
#include <string.h>

int main(void) {
    uint8_t data[FA18_C279_PROJECTION_GRID_OFFSET + 16] = { 0 };
    FA18HunkSegment segments[26];
    memset(segments, 0, sizeof segments);
    segments[FA18_C279_PROJECTION_GRID_HUNK].data = data;
    segments[FA18_C279_PROJECTION_GRID_HUNK].size = sizeof data;
    FA18Hunks hunks = { segments, 26 };
    FA18ProjectionGrid grid;
    FA18ProjectionGridRecord record;

    uint8_t *table = data + FA18_C279_PROJECTION_GRID_OFFSET;
    table[0] = 0; table[1] = 2;
    table[2] = 0; table[3] = 3;
    table[4] = 0x05; table[5] = 0x40;
    table[6] = 0xf9; table[7] = 0x80;
    table[8] = 0xff; table[9] = 0xec;
    table[10] = 0x01; table[11] = 0x60;
    table[12] = 0x05; table[13] = 0x80;
    table[14] = 0xff; table[15] = 0xf0;

    assert(fa18_load_projection_grid(&hunks, &grid) == 0);
    assert(grid.record_count == 2);
    assert(grid.bounds_limit == 3);
    assert(fa18_projection_grid_record(&grid, 0, &record) == 0);
    assert(record.x == 0x0540 && record.y == -0x0680 && record.kind == -20);
    assert(fa18_projection_grid_record(&grid, 1, &record) == 0);
    assert(record.x == 0x0160 && record.y == 0x0580 && record.kind == -16);
    assert(fa18_projection_grid_record(&grid, 2, &record) == -1);
    assert(fa18_projection_grid_record(&grid, 0, 0) == -1);

    table[0] = 0; table[1] = 3;
    assert(fa18_load_projection_grid(&hunks, &grid) == -1);
    assert(fa18_load_projection_grid(0, &grid) == -1);
    return 0;
}
