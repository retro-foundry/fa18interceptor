#include "blit_job.h"
#include <string.h>
FA18BlitExtent fa18_decode_blit_extent(uint16_t bltsize) {
    FA18BlitExtent extent;
    extent.width_words = (uint16_t)(bltsize & 0x003fu);
    extent.height_rows = (uint16_t)(bltsize >> 6);
    return extent;
}

int fa18_decode_display_blit_geometry(const FA18DisplayBlitPacket *packet,
                                      FA18DisplayBlitGeometry *geometry) {
    if (!packet || !geometry) return -1;
    geometry->extent = fa18_decode_blit_extent(packet->width_height);
    if (geometry->extent.width_words == 0 || geometry->extent.height_rows == 0) return -1;
    geometry->first_mask = packet->a_first_word;
    geometry->last_mask = packet->a_last_word;
    geometry->source_plane_mask = (uint8_t)((packet->control_a >> 8) & 0x0fu);
    return 0;
}

int fa18_prepare_c304b2_setup(const FA18AreaFillPacket *packet,
                              FA18BlitOperation *operation) {
    if (!packet || !operation) return -1;
    operation->bltcon0 = packet->control_a;
    operation->bltcon1 = packet->control_b;
    operation->bltafwm = packet->first_mask;
    operation->bltalwm = packet->last_mask;
    operation->bltadat = 0;
    operation->bltbdat = 0;
    operation->bltamod = packet->a_modulus;
    operation->bltbmod = packet->b_modulus;
    operation->bltcmod = packet->c_modulus;
    operation->bltdmod = packet->d_modulus;
    operation->bltapt = packet->a_source;
    operation->bltbpt = packet->b_source;
    operation->bltcpt = packet->c_source;
    operation->bltdpt = packet->d_destination;
    operation->bltsize = (uint16_t)((packet->height_rows << 6) |
                                    (packet->width_words & 0x3fu));
    return 0;
}

int fa18_prepare_c30668_submit(uint16_t a_low_word, uint32_t destination,
                               FA18BlitOperation *operation) {
    if (!operation) return -1;
    operation->bltapt = (operation->bltapt & 0xffff0000u) | a_low_word;
    operation->bltcpt = destination;
    operation->bltdpt = destination;
    return 0;
}

int fa18_prepare_line_blit(uint16_t bltcon0, uint16_t bltcon1,
                           uint16_t first_mask, uint16_t last_mask,
                           uint16_t adat, uint16_t bdat,
                           uint16_t amod, uint16_t bmod,
                           uint16_t cmod, uint16_t dmod,
                           uint32_t destination, uint16_t bltsize,
                           FA18BlitOperation *operation) {
    if (!operation || !(bltcon1 & 1u) || (bltsize & 0x3fu) == 0u ||
        (bltsize >> 6) == 0u) return -1;
    *operation = (FA18BlitOperation){
        bltcon0, bltcon1, first_mask, last_mask, adat, bdat,
        amod, bmod, cmod, dmod, 0, 0, destination, destination, bltsize
    };
    return 0;
}

uint16_t fa18_apply_blitter_minterm(uint8_t logic_function,
                                    uint16_t a, uint16_t b, uint16_t c) {
    uint16_t result = 0;
    for (unsigned bit = 0; bit < 16; ++bit) {
        const unsigned index = (((a >> bit) & 1u) << 2) |
                               (((b >> bit) & 1u) << 1) |
                               ((c >> bit) & 1u);
        if (logic_function & (uint8_t)(1u << index))
            result = (uint16_t)(result | (uint16_t)(1u << bit));
    }
    return result;
}

int fa18_execute_blitter_words(uint8_t logic_function,
                               const uint16_t *a_words,
                               const uint16_t *b_words,
                               const uint16_t *c_words,
                               uint16_t *d_words,
                               size_t word_count,
                               uint16_t first_mask,
                               uint16_t last_mask) {
    if (!a_words || !b_words || !c_words || !d_words || word_count == 0)
        return -1;
    for (size_t index = 0; index < word_count; ++index) {
        const uint16_t mask = index == 0 ? first_mask :
                              index + 1 == word_count ? last_mask : 0xffffu;
        const uint16_t result = fa18_apply_blitter_minterm(
            logic_function, a_words[index], b_words[index], c_words[index]);
        d_words[index] = (uint16_t)((d_words[index] & (uint16_t)~mask) |
                                    (result & mask));
    }
    return 0;
}

static int read_chip_word(const uint8_t *bytes, size_t count, uint32_t address,
                          uint16_t *word) {
    if (!bytes || !word || address > count || count - address < 2u) return -1;
    *word = (uint16_t)(((uint16_t)bytes[address] << 8) | bytes[address + 1u]);
    return 0;
}

static int write_chip_word(uint8_t *bytes, size_t count, uint32_t address,
                           uint16_t word) {
    if (!bytes || address > count || count - address < 2u) return -1;
    bytes[address] = (uint8_t)(word >> 8);
    bytes[address + 1u] = (uint8_t)word;
    return 0;
}

/* OCS word DMA ignores pointer bit zero. A signed modulo can leave the raw
 * row-end value odd, but the next row starts at the resolved even DMA address. */
static uint32_t blitter_word_address(uint32_t pointer) {
    return pointer & ~1u;
}

static uint16_t shifted_source_word(uint16_t previous, uint16_t current,
                                    uint8_t shift, int descending) {
    if (shift == 0u) return current;
    if (descending)
        return (uint16_t)((uint16_t)(current << shift) |
                          (uint16_t)(previous >> (16u - shift)));
    return (uint16_t)((uint16_t)(current >> shift) |
                      (uint16_t)(previous << (16u - shift)));
}

/* Exact low-to-high bit walk used by the pinned Engine9000/UAE
 * `build_blitfilltable()`.  BLTCON1 carries the initial fill carry in bit 2,
 * and bit 3 selects inclusive rather than exclusive fill. */
static uint16_t apply_ocs_fill(uint16_t value, int inclusive, int *carry) {
    uint16_t output = value;
    for (unsigned bit = 0; bit < 16; ++bit) {
        const uint16_t mask = (uint16_t)(1u << bit);
        if (*carry) {
            if (inclusive) output = (uint16_t)(output | mask);
            else output = (uint16_t)(output ^ mask);
        }
        if (value & mask) *carry = !*carry;
    }
    return output;
}

int fa18_execute_ocs_block_blit(const FA18BlitOperation *operation,
                                uint8_t *chip_bytes, size_t chip_byte_count) {
    FA18BlitExtent extent;
    uint32_t a, b, c, d;
    uint16_t previous_a, previous_b;
    const uint8_t shift_a = (uint8_t)(operation ? operation->bltcon0 >> 12 : 0u);
    const uint8_t shift_b = (uint8_t)(operation ? operation->bltcon1 >> 12 : 0u);
    const int use_a = operation && (operation->bltcon0 & 0x0800u) != 0u;
    const int use_b = operation && (operation->bltcon0 & 0x0400u) != 0u;
    const int use_c = operation && (operation->bltcon0 & 0x0200u) != 0u;
    const int use_d = operation && (operation->bltcon0 & 0x0100u) != 0u;
    const int descending = operation && (operation->bltcon1 & 0x0002u) != 0u;

    if (!operation || !chip_bytes || (operation->bltcon1 & 0x0001u) != 0u)
        return -1;
    extent = fa18_decode_blit_extent(operation->bltsize);
    if (!extent.width_words || !extent.height_rows || !use_d) return -1;

    /* BLT?PTL accepts only address bits 1..15. Modulo arithmetic may later
     * set bit zero internally; blitter_word_address() masks each DMA access. */
    a = operation->bltapt & ~1u;
    b = operation->bltbpt & ~1u;
    c = operation->bltcpt & ~1u;
    d = operation->bltdpt & ~1u;
    /* The source starts both shifter history registers clear.  BLTADAT and
     * BLTBDAT are the held values only when their DMA channels are absent. */
    previous_a = 0;
    previous_b = 0;
    uint32_t pending_destination = 0;
    uint16_t pending_result = 0;
    int have_pending_destination = 0;
    for (uint16_t row = 0; row < extent.height_rows; ++row) {
        int fill_carry = (operation->bltcon1 & 0x0004u) != 0u;
        for (uint16_t column = 0; column < extent.width_words; ++column) {
            uint16_t raw_a = operation->bltadat;
            uint16_t raw_b = operation->bltbdat;
            uint16_t raw_c = operation->bltcdat;
            uint16_t a_word, b_word, result;
            const uint16_t mask = column == 0 ? operation->bltafwm :
                                  column + 1u == extent.width_words ? operation->bltalwm :
                                  0xffffu;
            if (use_a && read_chip_word(chip_bytes, chip_byte_count,
                                        blitter_word_address(a), &raw_a) != 0)
                return -1;
            if (use_b && read_chip_word(chip_bytes, chip_byte_count,
                                        blitter_word_address(b), &raw_b) != 0)
                return -1;
            if (use_c && read_chip_word(chip_bytes, chip_byte_count,
                                        blitter_word_address(c), &raw_c) != 0)
                return -1;
            raw_a = (uint16_t)(raw_a & mask);
            a_word = use_a ? shifted_source_word(previous_a, raw_a, shift_a, descending) : 0;
            b_word = use_b ? shifted_source_word(previous_b, raw_b, shift_b, descending) : 0;
            /* D is a pipelined write: every source read for this word occurs
             * before the preceding D result becomes visible. */
            if (have_pending_destination &&
                write_chip_word(chip_bytes, chip_byte_count,
                                blitter_word_address(pending_destination),
                                pending_result) != 0)
                return -1;
            result = fa18_apply_blitter_minterm((uint8_t)operation->bltcon0,
                                                a_word, b_word, raw_c);
            if (operation->bltcon1 & 0x0018u)
                result = apply_ocs_fill(result, (operation->bltcon1 & 0x0008u) != 0u,
                                        &fill_carry);
            pending_destination = d;
            pending_result = result;
            have_pending_destination = 1;
            previous_a = raw_a;
            previous_b = raw_b;
            /* The final word access remains at its address while the modulo
             * is applied. The run036 $0486 DMA trace reads $76CE as row
             * one's last word, then begins row two at $76B0 after $001D. */
            if (column + 1u < extent.width_words) {
                if (use_a) a = (uint32_t)((int64_t)a + (descending ? -2 : 2));
                if (use_b) b = (uint32_t)((int64_t)b + (descending ? -2 : 2));
                if (use_c) c = (uint32_t)((int64_t)c + (descending ? -2 : 2));
                d = (uint32_t)((int64_t)d + (descending ? -2 : 2));
            }
        }
        if (use_a)
            a = blitter_word_address((uint32_t)((int64_t)a +
                (descending ? -(int16_t)operation->bltamod :
                              (int16_t)operation->bltamod)));
        if (use_b)
            b = blitter_word_address((uint32_t)((int64_t)b +
                (descending ? -(int16_t)operation->bltbmod :
                              (int16_t)operation->bltbmod)));
        if (use_c)
            c = blitter_word_address((uint32_t)((int64_t)c +
                (descending ? -(int16_t)operation->bltcmod :
                              (int16_t)operation->bltcmod)));
        d = blitter_word_address((uint32_t)((int64_t)d +
            (descending ? -(int16_t)operation->bltdmod :
                          (int16_t)operation->bltdmod)));
    }
    if (have_pending_destination &&
        write_chip_word(chip_bytes, chip_byte_count,
                        blitter_word_address(pending_destination),
                        pending_result) != 0)
        return -1;
    return 0;
}

int fa18_execute_ocs_line_blit(FA18BlitOperation *operation,
                               uint8_t *chip_bytes, size_t chip_byte_count) {
    FA18BlitExtent extent;
    uint32_t a, b, c, d;
    uint16_t con0, con1, bline;
    uint16_t c_data = 0;
    int one_dot = 0;

    if (!operation || !chip_bytes || (operation->bltcon1 & 0x0001u) == 0u ||
        (operation->bltcon0 & 0x0200u) == 0u) return -1;
    extent = fa18_decode_blit_extent(operation->bltsize);
    if (!extent.width_words || !extent.height_rows) return -1;
    /* `$C30668` may carry an odd pre-write destination (run036 has `$74C7`),
     * but BLTCPTL/BLTDPTL retain it as `$74C6` after their `$fffe` mask. */
    a = operation->bltapt & ~1u;
    b = operation->bltbpt & ~1u;
    c = operation->bltcpt & ~1u;
    d = operation->bltdpt & ~1u;
    con0 = operation->bltcon0;
    con1 = operation->bltcon1;
    { const uint8_t initial_bshift = (uint8_t)(con1 >> 12);
      bline = (uint16_t)((operation->bltbdat >> initial_bshift) |
                         (operation->bltbdat << ((16u - initial_bshift) & 15u))); }

    for (uint16_t row = 0; row < extent.height_rows; ++row) {
        /* The initial direction comes from the inherited BLTSIGN latch;
         * later rows refresh it after BLTAPT has advanced. */
        const int sign = (con1 & 0x0040u) != 0u;
        const int single = (con1 & 0x0002u) != 0u;
        const int sud = (con1 & 0x0010u) != 0u;
        const int sul = (con1 & 0x0008u) != 0u;
        const int aul = (con1 & 0x0004u) != 0u;
        const uint8_t shift = (uint8_t)(con0 >> 12);
        uint16_t a_data = operation->bltadat;
        uint16_t a_hold;
        uint16_t b_hold;
        uint16_t result;
        int moved_y = 0;
        /* Status selects the single-dot write before the C pointer steps.
         * A subsequent vertical step clears the latch for the next row. */
        const int write_pixel = !single || !one_dot;
        one_dot = 1;

        if (con0 & 0x0800u)
            a = (uint32_t)((int64_t)a + (sign ? (int16_t)operation->bltbmod :
                                                (int16_t)operation->bltamod));
        if (extent.width_words > 1u && (con0 & 0x0400u)) {
            if (read_chip_word(chip_bytes, chip_byte_count,
                               blitter_word_address(b), &bline) != 0) return -1;
            b = (uint32_t)((int64_t)b + (int16_t)operation->bltbmod);
        }
        /* Line mode consumes the rotating BLTBDAT bitstream regardless of
         * BLTCHB. `$C306A0` explicitly seeds it with $FFFF while B DMA is
         * disabled for the run036 polygon edges. */
        b_hold = (bline & 1u) ? 0xffffu : 0u;
        if (read_chip_word(chip_bytes, chip_byte_count,
                           blitter_word_address(c), &c_data) != 0) return -1;
        a_hold = (uint16_t)((a_data & operation->bltafwm) >> shift);
        result = fa18_apply_blitter_minterm((uint8_t)con0, a_hold, b_hold, c_data);

        if (!sign) {
            if (!sud) {
                if (sul) {
                    if (shift == 0u) c -= 2u;
                    con0 = (uint16_t)((con0 & 0x0fffu) | ((uint16_t)((shift + 15u) & 15u) << 12));
                } else {
                    if (shift == 15u) c += 2u;
                    con0 = (uint16_t)((con0 & 0x0fffu) | ((uint16_t)((shift + 1u) & 15u) << 12));
                }
            }
        }
        if (sud) {
            if (aul) {
                if (shift == 0u) c -= 2u;
                con0 = (uint16_t)((con0 & 0x0fffu) | ((uint16_t)((shift + 15u) & 15u) << 12));
            } else {
                if (shift == 15u) c += 2u;
                con0 = (uint16_t)((con0 & 0x0fffu) | ((uint16_t)((shift + 1u) & 15u) << 12));
            }
        }
        if (!sign && sud) {
            c = (uint32_t)((int64_t)c + (sul ? -(int16_t)operation->bltcmod :
                                             (int16_t)operation->bltcmod));
            moved_y = 1;
        }
        if (!sud) {
            c = (uint32_t)((int64_t)c + (aul ? -(int16_t)operation->bltcmod :
                                             (int16_t)operation->bltcmod));
            moved_y = 1;
        }
        if (moved_y) one_dot = 0;

        con1 = (uint16_t)((con1 & (uint16_t)~0x0040u) | ((((int16_t)a) < 0) ? 0x0040u : 0u));
        { const uint8_t bshift = (uint8_t)(((con1 >> 12) + 15u) & 15u);
          con1 = (uint16_t)((con1 & 0x0fffu) | ((uint16_t)bshift << 12));
          bline = (uint16_t)((operation->bltbdat >> bshift) |
                             (operation->bltbdat << ((16u - bshift) & 15u))); }
        if (write_pixel) {
            if (write_chip_word(chip_bytes, chip_byte_count,
                                blitter_word_address(d), result) != 0) return -1;
        }
        d = c;
    }
    operation->bltapt = a;
    operation->bltbpt = b;
    operation->bltcpt = c;
    operation->bltdpt = d;
    operation->bltcon0 = con0;
    operation->bltcon1 = con1;
    return 0;
}

static int16_t arithmetic_shift_right(int16_t value, unsigned count) {
    if (value >= 0) return (int16_t)(value >> count);
    return (int16_t)-((-(int32_t)value + ((INT32_C(1) << count) - 1)) >> count);
}

int fa18_execute_renderer_lane_stage(FA18RendererLaneStage *stage,
                                     FA18BlitOperation *operation,
                                     uint8_t *chip_bytes, size_t chip_byte_count) {
    if (!stage || !operation || !chip_bytes ||
        (stage->enable_word >= 0 && stage->stage_flag != 0))
        return -1;

    for (unsigned lane = 0; lane < 4; ++lane) {
        int16_t d3;
        int16_t d4;
        if ((stage->lane_enable_mask & (UINT8_C(1) << lane)) == 0u) {
            stage->line_control = (uint16_t)(stage->line_control >> 1);
            continue;
        }
        if (stage->enable_word < 0) {
            d3 = (int16_t)stage->line_control;
            d4 = stage->inherited_d4;
        } else {
            d3 = arithmetic_shift_right(stage->enable_word, lane);
            d4 = arithmetic_shift_right(stage->scale_word, lane);
        }
        /* `$C30466` consumes one bit before deriving the lane job. */
        stage->line_control = (uint16_t)(stage->line_control >> 1);
        operation->bltcon0 = (d4 & 1) ? 0x0fecu :
                             (d3 & 1) ? 0x0dfcu : 0x0d0cu;
        operation->bltcon1 = 0x0002u;
        operation->bltapt = stage->lane_copy;
        operation->bltbpt = stage->lane_offset + stage->plane_pointers[3u - lane];
        operation->bltdpt = operation->bltbpt;
        operation->bltsize = stage->blit_size;
        if (fa18_execute_ocs_block_blit(operation, chip_bytes, chip_byte_count) != 0)
            return -1;
    }
    /* `$C304B2`: A/B/D receive `$C45960`; all other registers are inherited. */
    operation->bltcon0 = 0x0d0cu;
    operation->bltcon1 = 0x0002u;
    operation->bltapt = stage->lane_pointer;
    operation->bltbpt = stage->lane_pointer;
    operation->bltdpt = stage->lane_pointer;
    operation->bltsize = stage->blit_size;
    return fa18_execute_ocs_block_blit(operation, chip_bytes, chip_byte_count);
}

void fa18_prepare_lane_blit(uint16_t blit_size, uint32_t lane_pointer, FA18BlitOperation *operation) {
    if (!operation) return;
    operation->bltcon0 = 0x0d0c; operation->bltcon1 = 2;
    operation->bltafwm = 0xffff; operation->bltalwm = 0xffff;
    operation->bltadat = 0; operation->bltbdat = 0;
    operation->bltamod = 0; operation->bltbmod = 0;
    operation->bltcmod = 0; operation->bltdmod = 0;
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
    operation->bltcon1 = 2;
    operation->bltafwm = 0xffff; operation->bltalwm = 0xffff;
    operation->bltadat = 0; operation->bltbdat = 0;
    operation->bltamod = a_mod; operation->bltbmod = 0;
    operation->bltcmod = 0; operation->bltdmod = 0;
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
