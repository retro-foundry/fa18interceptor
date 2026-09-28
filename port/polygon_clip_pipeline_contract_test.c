#include "polygon_clip_pipeline.h"

#include <assert.h>
#include <string.h>

int main(void) {
    /* run075, global frame 382: `$C246A0` reads 13 triples from `$C4BF94`
     * and leaves these 14 triples at `$C4B990` before `$C24CFE`. */
    const FA18ClipTuple input[] = {
        {9532,-32,7928}, {4120,-32,7476}, {4088,-32,7316},
        {2948,-32,6832}, {2424,-32,7160}, {2532,-32,7420},
        {1740,-32,7392}, {1096,-32,6956}, {2216,-32,7156},
        {944,-32,6728}, {-1348,-32,5244}, {-964,-32,-852},
        {10060,-32,-456}
    };
    const FA18ClipTuple expected[] = {
        {7782,-32,7782}, {4120,-32,7476}, {4088,-32,7316},
        {2948,-32,6832}, {2424,-32,7160}, {2532,-32,7420},
        {1740,-32,7392}, {1096,-32,6956}, {2216,-32,7156},
        {944,-32,6728}, {-1348,-32,5244}, {-1086,-32,1086},
        {-32,-32,32}, {32,-32,32}
    };
    FA18ClipTuple output[32];
    uint16_t output_count = 0;

    assert(fa18_clip_projection_polygon(input, 13, 0, output,
                                        sizeof output / sizeof *output,
                                        &output_count) == 0);
    assert(output_count == sizeof expected / sizeof *expected);
    assert(!memcmp(output, expected, sizeof expected));
    assert(fa18_clip_projection_polygon(input, 13, -1, output,
                                        sizeof output / sizeof *output,
                                        &output_count) == -1);
    assert(fa18_clip_projection_polygon(input, 13, 16, output,
                                        sizeof output / sizeof *output,
                                        &output_count) == -1);
    return 0;
}
