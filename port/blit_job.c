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

int fa18_build_run060_frame7991_area_fill(FA18AreaFillPacket *packet) {
    if (!packet) return -1;
    *packet = (FA18AreaFillPacket){
        0x0d0c, 0x0002, 0x00ff, 0x00ff,
        0x0028, 0x0001, 0x0001, 0x0000,
        0x000076ee, 0x000076ee, 0x00010026, 0x000076ee,
        20, 52
    };
    return 0;
}

int fa18_build_run060_frame7991_final_fill(FA18AreaFillPacket *packet) {
    if (!packet) return -1;
    *packet = (FA18AreaFillPacket){
        0x0dfc, 0x0002, 0x00ff, 0x00ff,
        0x0028, 0x0001, 0x0001, 0x0001,
        0x000076ee, 0x00014266, 0x00000037, 0x00014266,
        20, 52
    };
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

static uint16_t shifted_source_word(uint16_t previous, uint16_t current,
                                    uint8_t shift, int descending) {
    if (shift == 0u) return current;
    if (descending)
        return (uint16_t)((uint16_t)(current << shift) |
                          (uint16_t)(previous >> (16u - shift)));
    return (uint16_t)((uint16_t)(current >> shift) |
                      (uint16_t)(previous << (16u - shift)));
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

    if (!operation || !chip_bytes || (operation->bltcon1 & 0x0019u) != 0u)
        return -1;
    extent = fa18_decode_blit_extent(operation->bltsize);
    if (!extent.width_words || !extent.height_rows || !use_d) return -1;

    a = operation->bltapt;
    b = operation->bltbpt;
    c = operation->bltcpt;
    d = operation->bltdpt;
    previous_a = operation->bltadat;
    previous_b = operation->bltbdat;
    for (uint16_t row = 0; row < extent.height_rows; ++row) {
        for (uint16_t column = 0; column < extent.width_words; ++column) {
            uint16_t raw_a = operation->bltadat;
            uint16_t raw_b = operation->bltbdat;
            uint16_t raw_c = 0;
            uint16_t a_word, b_word, result, old_d;
            const uint16_t mask = column == 0 ? operation->bltafwm :
                                  column + 1u == extent.width_words ? operation->bltalwm :
                                  0xffffu;
            if (use_a && read_chip_word(chip_bytes, chip_byte_count, a, &raw_a) != 0)
                return -1;
            if (use_b && read_chip_word(chip_bytes, chip_byte_count, b, &raw_b) != 0)
                return -1;
            if (use_c && read_chip_word(chip_bytes, chip_byte_count, c, &raw_c) != 0)
                return -1;
            a_word = use_a ? shifted_source_word(previous_a, raw_a, shift_a, descending) : 0;
            b_word = use_b ? shifted_source_word(previous_b, raw_b, shift_b, descending) : 0;
            result = fa18_apply_blitter_minterm((uint8_t)operation->bltcon0,
                                                a_word, b_word, raw_c);
            if (read_chip_word(chip_bytes, chip_byte_count, d, &old_d) != 0)
                return -1;
            if (write_chip_word(chip_bytes, chip_byte_count, d,
                                (uint16_t)((old_d & (uint16_t)~mask) |
                                           (result & mask))) != 0)
                return -1;
            previous_a = raw_a;
            previous_b = raw_b;
            if (use_a) a = (uint32_t)((int64_t)a + (descending ? -2 : 2));
            if (use_b) b = (uint32_t)((int64_t)b + (descending ? -2 : 2));
            if (use_c) c = (uint32_t)((int64_t)c + (descending ? -2 : 2));
            d = (uint32_t)((int64_t)d + (descending ? -2 : 2));
        }
        if (use_a) a = (uint32_t)((int64_t)a +
                                   (descending ? -(int16_t)operation->bltamod :
                                                 (int16_t)operation->bltamod));
        if (use_b) b = (uint32_t)((int64_t)b +
                                   (descending ? -(int16_t)operation->bltbmod :
                                                 (int16_t)operation->bltbmod));
        if (use_c) c = (uint32_t)((int64_t)c +
                                   (descending ? -(int16_t)operation->bltcmod :
                                                 (int16_t)operation->bltcmod));
        d = (uint32_t)((int64_t)d + (descending ? -(int16_t)operation->bltdmod :
                                                (int16_t)operation->bltdmod));
    }
    return 0;
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

int fa18_build_run075_frame559_blit_packets(FA18DisplayBlitPacket packets[3]) {
    static const FA18DisplayBlitPacket recovered[3] = {
        {0x8aea, 0x0053, 0xffff, 0xffff, 0x0028, 0x0004, 0xfdc8, 0x0028,
         0xffff, 0x8000, 0x1e02, 0x6e71, 0x6e71},
        {0xface, 0x0043, 0xffff, 0xffff, 0x0028, 0x0000, 0xfea4, 0x0028,
         0xffff, 0x8000, 0x0d42, 0x6eef, 0x6eef},
        {0x0b4a, 0x0043, 0xffff, 0xffff, 0x0028, 0x0000, 0xfebc, 0x0028,
         0xffff, 0x8000, 0x0dc2, 0x6e58, 0x6e58}
    };
    if (!packets) return -1;
    memcpy(packets, recovered, sizeof recovered);
    return 0;
}
