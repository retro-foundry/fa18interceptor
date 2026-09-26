#include "blit_job.h"
#include <assert.h>
#include <stdio.h>
int main(void) {
    FA18BlitOperation operation;
    fa18_prepare_lane_blit(0x0302, 0x007b6a, &operation);
    assert(operation.bltcon0 == 0x0d0c && operation.bltcon1 == 2);
    assert(operation.bltapt == 0x007b6a && operation.bltbpt == 0x007b6a && operation.bltdpt == 0x007b6a);
    assert(operation.bltsize == 0x0302);
    assert(fa18_choose_lane_control(1, 1) == FA18_LANE_CONTROL_C);
    assert(fa18_choose_lane_control(0, 0) == FA18_LANE_CONTROL_B);
    assert(fa18_choose_lane_control(0, 1) == FA18_LANE_CONTROL_A);
    puts("blit job contract passed"); return 0;
}
