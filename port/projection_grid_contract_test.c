#include "projection_grid.h"

#include <assert.h>
#include <string.h>

int main(void) {
    uint8_t data[FA18_C279_PROJECTION_PAIR_SOURCE_20_OFFSET +
                 FA18_C279_PROJECTION_PAIR_SOURCE_BYTES] = { 0 };
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
    uint8_t *pairs_20 = data + FA18_C279_PROJECTION_PAIR_SOURCE_20_OFFSET;
    pairs_20[0] = 0; pairs_20[1] = 43;
    pairs_20[2] = 0xff; pairs_20[3] = 0xd5;
    pairs_20[4] = 0xff; pairs_20[5] = 0xd5;
    pairs_20[6] = 0xff; pairs_20[7] = 0xd5;
    pairs_20[8] = 0; pairs_20[9] = 0;
    pairs_20[10] = 0; pairs_20[11] = 32;

    assert(fa18_load_projection_grid(&hunks, &grid) == 0);
    assert(grid.record_count == 2);
    assert(grid.bounds_limit == 3);
    assert(fa18_projection_grid_record(&grid, 0, &record) == 0);
    assert(record.x == 0x0540 && record.y == -0x0680 && record.kind == -20);
    assert(fa18_projection_grid_record(&grid, 1, &record) == 0);
    assert(record.x == 0x0160 && record.y == 0x0580 && record.kind == -16);
    assert(fa18_projection_grid_record(&grid, 2, &record) == -1);
    assert(fa18_projection_grid_record(&grid, 0, 0) == -1);
    FA18ProjectionPairInput source_pairs[3];
    assert(fa18_projection_grid_pair_source(&grid, -20, source_pairs) == 0);
    assert(source_pairs[0].x == 43 && source_pairs[0].y == -43);
    assert(source_pairs[1].x == -43 && source_pairs[1].y == -43);
    assert(source_pairs[2].x == 0 && source_pairs[2].y == 32);
    assert(fa18_projection_grid_pair_source(&grid, -8, source_pairs) == -1);

    FA18ProjectionGridSetup setup = { 0 };
    assert(fa18_prepare_projection_grid(&grid, -125, -6207, -6578, &setup) == 0);
    assert(setup.record_count == 2 && setup.bounds_limit == 3);
    assert(setup.grid_x == -63 && setup.grid_y == -434);
    assert(setup.scaled_input == -1000 && setup.coordinate_shift == 3);
    assert(fa18_prepare_projection_grid(&grid, -129, -6207, -6578, &setup) == 1);
    assert(fa18_prepare_projection_grid(0, -125, -6207, -6578, &setup) == -1);

    table[4] = 0x01; table[5] = 0x00;
    table[6] = 0x02; table[7] = 0x00;
    table[8] = 0xff; table[9] = 0xf4;
    memset(data + FA18_C279_PROJECTION_BOUNDS_OFFSET, 0,
           FA18_C279_PROJECTION_BOUNDS_BYTES);
    FA18ProjectionGridPreparedRecord prepared;
    FA18ProjectionGridSetup record_setup = { 2, 3, 0, 0, 0, 3 };
    data[FA18_C279_PROJECTION_BOUNDS_OFFSET + 65] = 1;
    assert(fa18_prepare_projection_grid_record(&grid, &record_setup, 0, 0, &prepared) == 1);
    assert(prepared.shifted_x == 0x0800 && prepared.shifted_y == 0x1000);
    assert(prepared.kind == -12 && prepared.bound == 1);
    data[FA18_C279_PROJECTION_BOUNDS_OFFSET + 65] = 2;
    assert(fa18_prepare_projection_grid_record(&grid, &record_setup, 0, 0, &prepared) == 1);
    assert(prepared.kind == 1 && prepared.bound == 2);
    data[FA18_C279_PROJECTION_BOUNDS_OFFSET + 65] = 4;
    assert(fa18_prepare_projection_grid_record(&grid, &record_setup, 0, 0, &prepared) == 0);
    assert(fa18_prepare_projection_grid_record(&grid, &record_setup, 2, 0, &prepared) == -1);

    /* run075 global frame 384, first `$C27B9C` pair: matrix `$C45BD8`,
     * translated pair D3/D4, and base D1/D5/D7 from `-$20(A6)`. */
    FA18ProjectionPairMatrix matrix = {
        { 167, 0, -8, 0, 252, 0, 6, 0, 127 }
    };
    FA18ProjectionPairBase base;
    assert(fa18_prepare_projection_pair_base(&matrix, -1000, &base) == 0);
    assert(base.x == 0 && base.y == -985 && base.depth == 0);
    assert(fa18_prepare_projection_pair_base(0, -1000, &base) == -1);
    FA18ProjectionPairInput pair = { 2355, 7749 };
    FA18ProjectionPairOutput projected;
    assert(fa18_transform_projection_pair(&matrix, &base, &pair, &projected) == 0);
    assert(projected.x == 1294 && projected.y == -985 && projected.depth == 3899);
    assert(fa18_transform_projection_pair(0, &base, &pair, &projected) == -1);

    FA18ProjectionPairScreenPoint screen;
    assert(fa18_project_projection_pair(&projected, &screen) == 1);
    assert(screen.x == 106 && screen.y == 111);
    projected.depth = 0;
    assert(fa18_project_projection_pair(&projected, &screen) == 0);
    projected.x = 101; projected.y = 0; projected.depth = 100;
    assert(fa18_project_projection_pair(&projected, &screen) == 0);
    assert(fa18_project_projection_pair(0, &screen) == -1);

    FA18ProjectionGridPreparedRecord translation = { 2312, 7792, -20, 0 };
    FA18ProjectionTriangle triangle;
    assert(fa18_project_projection_triangle(&matrix, &base, &translation,
                                             source_pairs, &triangle) == 1);
    assert(triangle.points[0].x == 106 && triangle.points[0].y == 111);
    assert(triangle.points[1].x == 109 && triangle.points[1].y == 111);
    assert(triangle.points[2].x == 108 && triangle.points[2].y == 111);
    source_pairs[0].x = 32767;
    assert(fa18_project_projection_triangle(&matrix, &base, &translation,
                                             source_pairs, &triangle) == 0);

    table[0] = 0xff; table[1] = 0xff;
    assert(fa18_load_projection_grid(&hunks, &grid) == -1);
    assert(fa18_load_projection_grid(0, &grid) == -1);
    return 0;
}
