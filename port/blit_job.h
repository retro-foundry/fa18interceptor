#ifndef FA18_BLIT_JOB_H
#define FA18_BLIT_JOB_H
#include <stdint.h>
#include <stddef.h>
typedef struct {
    uint16_t bltcon0, bltcon1;
    uint16_t bltafwm, bltalwm;
    uint16_t bltadat, bltbdat;
    uint16_t bltamod, bltbmod, bltcmod, bltdmod;
    uint32_t bltapt, bltbpt, bltcpt, bltdpt;
    uint16_t bltsize;
} FA18BlitOperation;

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

typedef struct {
    FA18BlitExtent extent;
    uint16_t first_mask;
    uint16_t last_mask;
    uint8_t source_plane_mask;
} FA18DisplayBlitGeometry;

/* Caller state plus the writes made by `$C304B2` for the isolated run060
 * descending area operation.  This is a semantic packet, not an Amiga
 * address image. */
typedef struct {
    uint16_t control_a, control_b;
    uint16_t first_mask, last_mask;
    uint16_t c_modulus, b_modulus, a_modulus, d_modulus;
    uint32_t a_source, b_source, c_source, d_destination;
    uint16_t width_words, height_rows;
} FA18AreaFillPacket;

FA18BlitExtent fa18_decode_blit_extent(uint16_t bltsize);
int fa18_decode_display_blit_geometry(const FA18DisplayBlitPacket *packet,
                                      FA18DisplayBlitGeometry *geometry);
int fa18_build_run060_frame7991_area_fill(FA18AreaFillPacket *packet);
int fa18_build_run060_frame7991_final_fill(FA18AreaFillPacket *packet);
int fa18_prepare_c304b2_setup(const FA18AreaFillPacket *packet,
                              FA18BlitOperation *operation);
/* `$C30668-$C306B3`: retain the caller's A high word, replace BLTAPTL,
 * point C and D at the prepared semantic destination, and leave the other
 * inherited channels unchanged until the final BLTSIZE write. */
int fa18_prepare_c30668_submit(uint16_t a_low_word, uint32_t destination,
                               FA18BlitOperation *operation);
int fa18_prepare_line_blit(uint16_t bltcon0, uint16_t bltcon1,
                           uint16_t first_mask, uint16_t last_mask,
                           uint16_t adat, uint16_t bdat,
                           uint16_t amod, uint16_t bmod,
                           uint16_t cmod, uint16_t dmod,
                           uint32_t destination, uint16_t bltsize,
                           FA18BlitOperation *operation);
uint16_t fa18_apply_blitter_minterm(uint8_t logic_function,
                                    uint16_t a, uint16_t b, uint16_t c);
int fa18_execute_blitter_words(uint8_t logic_function,
                               const uint16_t *a_words,
                               const uint16_t *b_words,
                               const uint16_t *c_words,
                               uint16_t *d_words,
                               size_t word_count,
                               uint16_t first_mask,
                               uint16_t last_mask);

/* Execute one OCS block-mode BLTSIZE submission against caller-owned Chip
 * bytes.  It preserves the source register model: enabled A/B/C channels are
 * selected from BLTCON0, shifts use their BLTCON fields and initial data
 * registers, and pointer/modulo progression follows BLTCON1 DESC.  Line and
 * fill modes have distinct hardware state and deliberately fail here rather
 * than being approximated as a polygon fill. */
int fa18_execute_ocs_block_blit(const FA18BlitOperation *operation,
                                uint8_t *chip_bytes, size_t chip_byte_count);
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
