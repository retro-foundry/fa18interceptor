#include "blit_job.h"
void fa18_prepare_lane_blit(uint16_t blit_size, uint32_t lane_pointer, FA18BlitOperation *operation) {
    if (!operation) return;
    operation->bltcon0 = 0x0d0c; operation->bltcon1 = 2;
    operation->bltapt = lane_pointer; operation->bltbpt = lane_pointer;
    operation->bltdpt = lane_pointer; operation->bltsize = blit_size;
}
FA18LaneControl fa18_choose_lane_control(uint16_t d4, uint16_t d3) {
    if (d4 & 1u) return FA18_LANE_CONTROL_C;
    if (!(d3 & 1u)) return FA18_LANE_CONTROL_B;
    return FA18_LANE_CONTROL_A;
}
