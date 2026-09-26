#ifndef FA18_BLIT_JOB_H
#define FA18_BLIT_JOB_H
#include <stdint.h>
typedef struct { uint16_t bltcon0, bltcon1; uint32_t bltapt, bltbpt, bltdpt; uint16_t bltsize; } FA18BlitOperation;
typedef enum { FA18_LANE_CONTROL_A = 0, FA18_LANE_CONTROL_B, FA18_LANE_CONTROL_C } FA18LaneControl;
void fa18_prepare_lane_blit(uint16_t blit_size, uint32_t lane_pointer, FA18BlitOperation *operation);
FA18LaneControl fa18_choose_lane_control(uint16_t d4, uint16_t d3);
#endif
