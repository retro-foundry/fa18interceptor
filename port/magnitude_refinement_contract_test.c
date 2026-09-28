#include "magnitude_refinement.h"

#include <assert.h>
#include <stdio.h>

int main(void) {
    uint16_t threshold = 0;
    assert(fa18_refine_component_magnitude(0, &threshold) == 0 && threshold == 0);
    assert(fa18_refine_component_magnitude(1, &threshold) == 0 && threshold == 1);
    assert(fa18_refine_component_magnitude(4, &threshold) == 0 && threshold == 2);
    assert(fa18_refine_component_magnitude(9, &threshold) == 0 && threshold == 3);
    assert(fa18_refine_component_magnitude(10, &threshold) == 0 && threshold == 3);
    /* `$C25692` and `$C256D0` retain the original scale by shifting the
     * refined result back by two and four bits respectively. */
    assert(fa18_refine_component_magnitude(UINT32_C(16000000), &threshold) == 0 &&
           threshold == 4000);
    assert(fa18_refine_component_magnitude(UINT32_C(256000000), &threshold) == 0 &&
           threshold == 16000);
    assert(fa18_refine_component_magnitude(1, NULL) == -1);
    puts("magnitude refinement contract passed");
    return 0;
}
