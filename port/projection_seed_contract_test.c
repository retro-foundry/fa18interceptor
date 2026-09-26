#include "projection_seed.h"

#include <stdio.h>
#include <string.h>

int main(void) {
    /* run060 `$C1C54E`, record type $11, identity 2.14 matrix. */
    const FA18Fixed14Matrix identity = {{{0x4000, 0, 0}, {0, 0x4000, 0},
                                          {0, 0, 0x4000}}};
    const FA18ProjectionBase base = {0x11982c00, 0x00007708, 0x1059a000};
    const FA18ProjectionSeedResult expected = {0x11982c00, 0x00007c08,
                                                0x1059b400};
    FA18ProjectionSeedResult result;
    if (fa18_select_projection_seed(0x11u, &identity, base, &result) != 0 ||
        memcmp(&result, &expected, sizeof result) != 0) {
        fputs("run060 projection seed fixture failed\n", stderr);
        return 1;
    }
    const FA18ProjectionSeedResult special = {0, 256, -1280};
    if (fa18_select_projection_seed(0x30u, &identity,
                                   (FA18ProjectionBase){0, 0, 0}, &result) != 0 ||
        memcmp(&result, &special, sizeof result) != 0) {
        fputs("projection seed type selection failed\n", stderr);
        return 1;
    }
    puts("run060 projection seed contract passed");
    return 0;
}
