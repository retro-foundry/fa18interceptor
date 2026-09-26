#include "display.h"

#include <stddef.h>
#include <string.h>
#include "blit_job.h"

int fa18_apply_run060_frame7991_fill(FA18PlanarPage *page) {
    static const FA18FillSpan spans[] = {
        {94,145,208},{95,141,218},{96,134,236},{97,127,255},
        {98,120,273},{99,113,292},{100,107,310},{101,100,319},
        {102,93,319},{103,86,319},{104,79,319},{105,72,319},
        {106,65,319},{107,58,319},{108,51,319},{109,44,319},
        {110,37,319},{111,31,319},{112,24,319},{113,17,319},
        {114,10,319},{115,3,319},{116,0,319},{117,0,319},
        {118,0,319},{119,0,319},{120,0,319},{121,0,319},
        {122,0,319},{123,0,319},{124,0,319},{125,0,319},
        {126,0,319},{127,0,319},{128,0,319},{129,0,319},
        {130,0,319},{131,0,319},{132,0,319},{133,0,319},
        {134,0,319},{135,0,319},{136,0,319},{137,0,319},
        {138,0,319},{139,0,319},{140,0,319},{141,0,319},
        {142,0,319},{143,0,319},{144,0,319}
    };
    if (!page) return -1;
    for (size_t i = 0; i < sizeof spans / sizeof spans[0]; ++i) {
        for (uint16_t x = spans[i].x_first; x <= spans[i].x_last; ++x) {
            const size_t offset = (size_t)spans[i].y * FA18_PLANAR_ROW_BYTES +
                                  (size_t)x / 8u;
            page->plane[0][offset] |= (uint8_t)(0x80u >> (x & 7u));
        }
    }
    return 0;
}

int fa18_visible_lane_plane(unsigned lane) {
    return lane < FA18_PLANES ? (FA18_PLANES - 1 - (int)lane) : -1;
}

uint8_t fa18_visible_lane_mask_to_plane_mask(uint8_t lane_mask) {
    uint8_t plane_mask = 0;
    for (unsigned lane = 0; lane < FA18_PLANES; ++lane) {
        if (lane_mask & (uint8_t)(1u << lane)) {
            plane_mask |= (uint8_t)(1u << fa18_visible_lane_plane(lane));
        }
    }
    return plane_mask;
}

int fa18_decode_planar_word_offset(uint16_t byte_offset, int *word_x, int *y) {
    if (!word_x || !y || (byte_offset & 1u) != 0) return -1;
    const int row = byte_offset / FA18_PLANAR_ROW_BYTES;
    const int byte_x = byte_offset % FA18_PLANAR_ROW_BYTES;
    if (row >= FA18_HEIGHT || byte_x / 2 >= FA18_WIDTH / 16) return -1;
    *word_x = byte_x / 2;
    *y = row;
    return 0;
}

const FA18PlanarPage *fa18_select_display_page(const FA18DisplayPagePair *pair,
                                                int adjusted) {
    if (!pair) return NULL;
    return adjusted ? pair->adjusted : pair->base;
}

int fa18_palette_set(FA18Palette *palette, uint8_t index, uint16_t rgb4) {
    if (!palette || index >= 16u || (rgb4 & 0xf000u) != 0) return -1;
    palette->rgb4[index] = rgb4;
    return 0;
}

uint16_t fa18_rgb4_colour(uint16_t rgb4) {
    return (uint16_t)(rgb4 & 0x0fffu);
}

int fa18_blit_planar_words(const FA18PlanarPage *source, FA18PlanarPage *destination,
                           int source_word_x, int source_y, int destination_word_x,
                           int destination_y, int width_words, int height_rows,
                           uint16_t first_mask, uint16_t last_mask,
                           uint8_t plane_mask) {
    if (!source || !destination || width_words <= 0 || height_rows <= 0 ||
        source_word_x < 0 || destination_word_x < 0 || source_y < 0 ||
        destination_y < 0 || source_word_x + width_words > FA18_WIDTH / 16 ||
        destination_word_x + width_words > FA18_WIDTH / 16 ||
        source_y + height_rows > FA18_HEIGHT ||
        destination_y + height_rows > FA18_HEIGHT) return -1;
    for (int plane = 0; plane < FA18_PLANES; ++plane) {
        if (!(plane_mask & (uint8_t)(1u << plane))) continue;
        for (int row = 0; row < height_rows; ++row) {
            for (int word = 0; word < width_words; ++word) {
                uint16_t mask = 0xffffu;
                if (word == 0) mask = (uint16_t)(mask & first_mask);
                if (word == width_words - 1) mask = (uint16_t)(mask & last_mask);
                const size_t source_offset = (size_t)(source_y + row) * FA18_PLANAR_ROW_BYTES +
                                             (size_t)(source_word_x + word) * 2u;
                const size_t destination_offset = (size_t)(destination_y + row) * FA18_PLANAR_ROW_BYTES +
                                                  (size_t)(destination_word_x + word) * 2u;
                uint16_t source_word = (uint16_t)(((uint16_t)source->plane[plane][source_offset] << 8) |
                                                  source->plane[plane][source_offset + 1u]);
                uint16_t destination_word = (uint16_t)(((uint16_t)destination->plane[plane][destination_offset] << 8) |
                                                       destination->plane[plane][destination_offset + 1u]);
                destination_word = (uint16_t)((destination_word & (uint16_t)~mask) |
                                              (source_word & mask));
                destination->plane[plane][destination_offset] = (uint8_t)(destination_word >> 8);
                destination->plane[plane][destination_offset + 1u] = (uint8_t)destination_word;
            }
        }
    }
    return 0;
}

int fa18_blit_visible_lanes(const FA18PlanarPage *source, FA18PlanarPage *destination,
                            int source_word_x, int source_y, int destination_word_x,
                            int destination_y, int width_words, int height_rows,
                            uint16_t first_mask, uint16_t last_mask,
                            uint8_t lane_mask) {
    return fa18_blit_planar_words(source, destination, source_word_x, source_y,
                                  destination_word_x, destination_y, width_words,
                                  height_rows, first_mask, last_mask,
                                  fa18_visible_lane_mask_to_plane_mask(lane_mask));
}

int fa18_execute_planar_blit(const FA18PlanarPage *source,
                             FA18PlanarPage *destination,
                             int a_plane, int b_plane, int c_plane, int d_plane,
                             int source_word_x, int source_y,
                             int destination_word_x, int destination_y,
                             int width_words, int height_rows,
                             uint8_t logic_function,
                             uint16_t first_mask, uint16_t last_mask) {
    if (!source || !destination || a_plane < 0 || a_plane >= FA18_PLANES ||
        b_plane < 0 || b_plane >= FA18_PLANES || c_plane < 0 || c_plane >= FA18_PLANES ||
        d_plane < 0 || d_plane >= FA18_PLANES || source_word_x < 0 ||
        destination_word_x < 0 || source_y < 0 || destination_y < 0 ||
        width_words <= 0 || height_rows <= 0 || width_words > FA18_WIDTH / 16 ||
        source_word_x + width_words > FA18_WIDTH / 16 ||
        destination_word_x + width_words > FA18_WIDTH / 16 ||
        source_y + height_rows > FA18_HEIGHT || destination_y + height_rows > FA18_HEIGHT)
        return -1;
    uint16_t a[FA18_WIDTH / 16], b[FA18_WIDTH / 16];
    uint16_t c[FA18_WIDTH / 16], d[FA18_WIDTH / 16];
    for (int row = 0; row < height_rows; ++row) {
        for (int word = 0; word < width_words; ++word) {
            const size_t source_offset = (size_t)(source_y + row) * FA18_PLANAR_ROW_BYTES +
                                         (size_t)(source_word_x + word) * 2u;
            const size_t destination_offset = (size_t)(destination_y + row) * FA18_PLANAR_ROW_BYTES +
                                              (size_t)(destination_word_x + word) * 2u;
            const uint8_t *ap = &source->plane[a_plane][source_offset];
            const uint8_t *bp = &source->plane[b_plane][source_offset];
            const uint8_t *cp = &source->plane[c_plane][source_offset];
            const uint8_t *dp = &destination->plane[d_plane][destination_offset];
            a[word] = (uint16_t)((ap[0] << 8) | ap[1]);
            b[word] = (uint16_t)((bp[0] << 8) | bp[1]);
            c[word] = (uint16_t)((cp[0] << 8) | cp[1]);
            d[word] = (uint16_t)((dp[0] << 8) | dp[1]);
        }
        if (fa18_execute_blitter_words(logic_function, a, b, c, d,
                                       (size_t)width_words, first_mask, last_mask) != 0)
            return -1;
        for (int word = 0; word < width_words; ++word) {
            const size_t offset = (size_t)(destination_y + row) * FA18_PLANAR_ROW_BYTES +
                                  (size_t)(destination_word_x + word) * 2u;
            destination->plane[d_plane][offset] = (uint8_t)(d[word] >> 8);
            destination->plane[d_plane][offset + 1u] = (uint8_t)d[word];
        }
    }
    return 0;
}

void fa18_decode_planar_page(const FA18PlanarPage *page,
                             FA18IndexedFrameBuffer *framebuffer) {
    if (!page || !framebuffer) return;
    for (int y = 0; y < FA18_HEIGHT; ++y) {
        for (int byte_x = 0; byte_x < FA18_PLANAR_ROW_BYTES; ++byte_x) {
            const size_t source = (size_t)y * FA18_PLANAR_ROW_BYTES + (size_t)byte_x;
            for (int bit = 0; bit < 8; ++bit) {
                const uint8_t mask = (uint8_t)(0x80u >> bit);
                uint8_t index = 0;
                for (int plane = 0; plane < FA18_PLANES; ++plane) {
                    if (page->plane[plane][source] & mask) index |= (uint8_t)(1u << plane);
                }
                framebuffer->pixels[(size_t)y * FA18_WIDTH + (size_t)byte_x * 8u + (size_t)bit] = index;
            }
        }
    }
}

int fa18_encode_planar_page(const FA18IndexedFrameBuffer *framebuffer,
                            FA18PlanarPage *page) {
    if (!framebuffer || !page) return -1;
    memset(page, 0, sizeof *page);
    for (int y = 0; y < FA18_HEIGHT; ++y) {
        for (int byte_x = 0; byte_x < FA18_PLANAR_ROW_BYTES; ++byte_x) {
            const size_t source = (size_t)y * FA18_WIDTH + (size_t)byte_x * 8u;
            const size_t destination = (size_t)y * FA18_PLANAR_ROW_BYTES +
                                       (size_t)byte_x;
            for (int bit = 0; bit < 8; ++bit) {
                const uint8_t index = framebuffer->pixels[source + (size_t)bit];
                if (index > 15u) return -1;
                const uint8_t mask = (uint8_t)(0x80u >> bit);
                for (int plane = 0; plane < FA18_PLANES; ++plane) {
                    if (index & (uint8_t)(1u << plane)) {
                        page->plane[plane][destination] |= mask;
                    }
                }
            }
        }
    }
    return 0;
}

int fa18_apply_palette(const FA18IndexedFrameBuffer *framebuffer,
                       const FA18Palette *palette,
                       uint16_t rgb444[FA18_WIDTH * FA18_HEIGHT]) {
    if (!framebuffer || !palette || !rgb444) return -1;
    for (size_t pixel = 0; pixel < FA18_WIDTH * FA18_HEIGHT; ++pixel) {
        if (framebuffer->pixels[pixel] > 15u) return -1;
        rgb444[pixel] = fa18_rgb4_colour(palette->rgb4[framebuffer->pixels[pixel]]);
    }
    return 0;
}
