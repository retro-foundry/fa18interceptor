#include "blit_job.h"
FA18BlitExtent fa18_decode_blit_extent(uint16_t bltsize) {
    FA18BlitExtent extent;
    extent.width_words = (uint16_t)(bltsize & 0x003fu);
    extent.height_rows = (uint16_t)(bltsize >> 6);
    return extent;
}

void fa18_prepare_lane_blit(uint16_t blit_size, uint32_t lane_pointer, FA18BlitOperation *operation) {
    if (!operation) return;
    operation->bltcon0 = 0x0d0c; operation->bltcon1 = 2; operation->bltamod = 0;
    operation->bltapt = lane_pointer; operation->bltbpt = lane_pointer;
    operation->bltcpt = 0; operation->bltdpt = lane_pointer; operation->bltsize = blit_size;
}

void fa18_prepare_adjusted_lane_blit(uint16_t blit_size, uint32_t lane_pointer,
                                     uint32_t lane_offset, int16_t vertical_input,
                                     int16_t vertical_offset, uint16_t mode_word,
                                     uint16_t limit_word, uint16_t d3,
                                     FA18BlitOperation *operation) {
    if (!operation) return;
    int32_t vertical = (int32_t)vertical_input - vertical_offset - 0xb7;
    uint32_t c_pointer = (uint32_t)(0x12adc + (vertical * 4) - 2);
    uint16_t a_mod = (uint16_t)(blit_size & 0x3f);
    a_mod = (uint16_t)(a_mod - 3);
    a_mod = (uint16_t)(-(int16_t)a_mod);
    if ((int16_t)(mode_word >> 4) == (int16_t)(limit_word + 0xc)) {
        c_pointer -= 2;
    }
    operation->bltcon0 = (d3 & 1u) ? 0x0fec : 0x0f4c;
    operation->bltcon1 = 2; operation->bltamod = a_mod;
    operation->bltapt = lane_pointer; operation->bltcpt = c_pointer;
    operation->bltbpt = lane_offset + lane_pointer;
    operation->bltdpt = operation->bltbpt;
    operation->bltsize = blit_size;
}
FA18LaneControl fa18_choose_lane_control(uint16_t d4, uint16_t d3) {
    if (d4 & 1u) return FA18_LANE_CONTROL_C;
    if (!(d3 & 1u)) return FA18_LANE_CONTROL_B;
    return FA18_LANE_CONTROL_A;
}
