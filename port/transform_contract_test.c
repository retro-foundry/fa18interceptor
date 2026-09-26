#include "transform.h"

#include <stdio.h>
#include <string.h>

int main(void) {
    /* run075 demonstration replay, $C1F4AC at frame 117, source $C3B720. */
    const FA18LocalVertex local[5] = {
        {2112, 0, -896}, {-2080, 0, -2048}, {-1600, 0, 1280},
        {1088, 0, 1568}, {-640, 1024, 0}
    };
    const FA18VertexTransform transform = {
        1,
        {-9216, -32768, -9216},
        {{{168, 0, 0}, {0, 0, 252}, {0, -128, 0}}}
    };
    const FA18TransformedVertex expected[5] = {
        {-5355, -9513, 16384}, {-6731, -10080, 16384},
        {-6573, -8442, 16384}, {-5691, -8301, 16384},
        {-6258, -9072, 16128}
    };
    FA18TransformedVertex output[5];
    if (fa18_transform_vertices(&transform, local, 5, output) != 0 ||
        memcmp(output, expected, sizeof expected) != 0) {
        fputs("demo matrix transform fixture failed\n", stderr);
        return 1;
    }
    const FA18VertexTransform wrap = {
        0, {1, 0, 0}, {{{256, 0, 0}, {0, 256, 0}, {0, 0, 256}}}
    };
    const FA18LocalVertex edge = {32767, -3, 4};
    const FA18TransformedVertex edge_expected = {-32768, -3, 4};
    if (fa18_transform_vertices(&wrap, &edge, 1, output) != 0 ||
        memcmp(output, &edge_expected, sizeof edge_expected) != 0 ||
        fa18_transform_vertices(NULL, local, 1, output) != -1) {
        fputs("matrix word arithmetic contract failed\n", stderr);
        return 1;
    }
    puts("matrix transform contract passed");
    return 0;
}
