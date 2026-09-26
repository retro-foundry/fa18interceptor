#ifndef FA18_BLIT_JOB_H
#define FA18_BLIT_JOB_H
#include <stdint.h>
typedef struct { uint16_t bltcon0, bltcon1, bltamod; uint32_t bltapt, bltbpt, bltcpt, bltdpt; uint16_t bltsize; } FA18BlitOperation;

/* Complete semantic packet for the frame559 `$C30678` setup. */
typedef struct {
    uint16_t control_a, control_b;
    uint16_t a_first_word, a_last_word;
    uint16_t a_modulus, b_modulus, c_modulus, d_modulus;
    uint16_t a_data, b_data, width_height;
    uint32_t c_source, d_destination;
} FA18DisplayBlitPacket;
/* The port keeps the operation's visible geometry separately from the
 * original register encoding.  BLTSIZE stores rows in bits 15..6 and words
 * in bits 5..0. */
typedef struct {
    uint16_t width_words;
    uint16_t height_rows;
} FA18BlitExtent;

FA18BlitExtent fa18_decode_blit_extent(uint16_t bltsize);
typedef enum { FA18_LANE_CONTROL_A = 0, FA18_LANE_CONTROL_B, FA18_LANE_CONTROL_C } FA18LaneControl;
void fa18_prepare_lane_blit(uint16_t blit_size, uint32_t lane_pointer, FA18BlitOperation *operation);

/* `$C304FA-$C305A9`: prepare the adjusted lane operation. */
void fa18_prepare_adjusted_lane_blit(uint16_t blit_size, uint32_t lane_pointer,
                                     uint32_t lane_offset, int16_t vertical_input,
                                     int16_t vertical_offset, uint16_t mode_word,
                                     uint16_t limit_word, uint16_t d3,
                                     FA18BlitOperation *operation);
FA18LaneControl fa18_choose_lane_control(uint16_t d4, uint16_t d3);
int fa18_build_run075_frame559_blit_packets(FA18DisplayBlitPacket packets[3]);
#endif
