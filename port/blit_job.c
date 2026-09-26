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

void fa18_prepare_lane_blit(uint16_t blit_size, uint32_t lane_pointer, FA18BlitOperation *operation) {
    if (!operation) return;
    operation->bltcon0 = 0x0d0c; operation->bltcon1 = 2;
    operation->bltafwm = 0xffff; operation->bltalwm = 0xffff;
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
