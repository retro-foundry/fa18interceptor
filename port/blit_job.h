#ifndef FA18_BLIT_JOB_H
#define FA18_BLIT_JOB_H
#include <stdint.h>
typedef struct { uint16_t bltcon0, bltcon1, bltamod; uint32_t bltapt, bltbpt, bltcpt, bltdpt; uint16_t bltsize; } FA18BlitOperation;
typedef enum { FA18_LANE_CONTROL_A = 0, FA18_LANE_CONTROL_B, FA18_LANE_CONTROL_C } FA18LaneControl;
void fa18_prepare_lane_blit(uint16_t blit_size, uint32_t lane_pointer, FA18BlitOperation *operation);

/* `$C304FA-$C305A9`: prepare the adjusted lane operation. */
void fa18_prepare_adjusted_lane_blit(uint16_t blit_size, uint32_t lane_pointer,
                                     uint32_t lane_offset, int16_t vertical_input,
                                     int16_t vertical_offset, uint16_t mode_word,
                                     uint16_t limit_word, uint16_t d3,
                                     FA18BlitOperation *operation);
FA18LaneControl fa18_choose_lane_control(uint16_t d4, uint16_t d3);
#endif
