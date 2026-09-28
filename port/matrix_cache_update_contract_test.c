#include "matrix_cache_update.h"
#include "run075_trig_asset.h"
#include "two_angle_matrix.h"

#include <assert.h>
#include <string.h>

int main(void) {
    uint8_t hunk63[0xae8 + sizeof fa18_run075_trig_bytes] = {0};
    FA18HunkSegment segments[64] = {0};
    FA18Hunks hunks = {segments, 64};
    FA18FlightTrigTable trig = {0};
    FA18MatrixCacheUpdateInput input = {{0, 28600, 0}, {168, 252, 128}};
    int16_t matrix[3][3];
    const int16_t expected[3][3] = {
        {167, 0, -8}, {0, 252, 0}, {6, 0, 127}
    };

    memcpy(hunk63 + 0xae8, fa18_run075_trig_bytes, sizeof fa18_run075_trig_bytes);
    segments[63] = (FA18HunkSegment){.data = hunk63, .size = sizeof hunk63};
    assert(fa18_load_two_angle_trig_table(&hunks, &trig) == 0);
    /* run075 local frame 189: `$C2DC9A` loads 0, $6fb8, 0; `$C2E5AC`
     * then reads $00a8/$00fc/$0080 from `$C45A3E`. */
    assert(fa18_update_projection_matrix_cache(&trig, &input, matrix) == 0);
    assert(memcmp(matrix, expected, sizeof matrix) == 0);
    assert(fa18_update_projection_matrix_cache(0, &input, matrix) == -1);
    assert(fa18_update_projection_matrix_cache(&trig, 0, matrix) == -1);
    assert(fa18_update_projection_matrix_cache(&trig, &input, 0) == -1);
    return 0;
}
