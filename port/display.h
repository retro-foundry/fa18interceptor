#ifndef FA18_DISPLAY_H
#define FA18_DISPLAY_H

#include "renderer.h"
#include "line.h"

#include <stdint.h>

enum { FA18_PLANES = 4, FA18_PLANAR_ROW_BYTES = FA18_WIDTH / 8,
       FA18_PLANAR_PAGE_BYTES = FA18_PLANAR_ROW_BYTES * FA18_HEIGHT };

/* `$C2FD8C` receives visible plane destinations in lane order 4, 3, 2, 1. */
int fa18_visible_lane_plane(unsigned lane);
uint8_t fa18_visible_lane_mask_to_plane_mask(uint8_t lane_mask);
int fa18_decode_planar_word_offset(uint16_t byte_offset, int *word_x, int *y);

/* Native four-plane page used only at the display boundary. It has no Chip-RAM
 * addresses, Copper list, or hardware register state. Plane zero is the low
 * bit of the resulting chunky colour index. */
typedef struct {
    uint8_t plane[FA18_PLANES][FA18_PLANAR_PAGE_BYTES];
} FA18PlanarPage;

void fa18_clear_planar_page(FA18PlanarPage *page);
int fa18_apply_planar_word(FA18PlanarPage *page, int plane, int word_x,
                           int y, uint16_t and_mask, uint16_t or_mask);
void fa18_planar_page_to_indexed(const FA18PlanarPage *page,
                                 FA18IndexedFrameBuffer *framebuffer);
int fa18_blit_planar_words(const FA18PlanarPage *source, FA18PlanarPage *destination,
                           int source_word_x, int source_y, int destination_word_x,
                           int destination_y, int width_words, int height_rows,
                           uint16_t first_mask, uint16_t last_mask,
                           uint8_t plane_mask);
int fa18_blit_visible_lanes(const FA18PlanarPage *source, FA18PlanarPage *destination,
                            int source_word_x, int source_y, int destination_word_x,
                            int destination_y, int width_words, int height_rows,
                            uint16_t first_mask, uint16_t last_mask,
                            uint8_t lane_mask);
int fa18_execute_planar_blit(const FA18PlanarPage *source,
                             FA18PlanarPage *destination,
                             int a_plane, int b_plane, int c_plane, int d_plane,
                             int source_word_x, int source_y,
                             int destination_word_x, int destination_y,
                             int width_words, int height_rows,
                             uint8_t logic_function,
                             uint16_t first_mask, uint16_t last_mask);

/* Run060 frame 7991's proven descending fill, expressed as a semantic page
 * operation. The captured destination is plane 0, word 19, row 144. */
int fa18_apply_run060_frame7991_fill(FA18PlanarPage *page);

int fa18_execute_run060_frame7992_area_job(
    FA18PlanarPage *page, const FA18AreaBlitJob *job,
    const uint8_t a_source[12][37], const uint8_t b_source[12][37]);

/* Amiga RGB4 words, held as palette state rather than COLORxx registers. */
typedef struct {
    uint16_t rgb4[16];
} FA18Palette;

int fa18_palette_set(FA18Palette *palette, uint8_t index, uint16_t rgb4);

/* Semantic replacement for the two page pairs selected by `$C2F558`. */
typedef struct {
    const FA18PlanarPage *base;
    const FA18PlanarPage *adjusted;
} FA18DisplayPagePair;

const FA18PlanarPage *fa18_select_display_page(const FA18DisplayPagePair *pair,
                                                int adjusted);

/* Convert one 12-bit Amiga RGB4 word to the port's RGB444 pixel format. */
uint16_t fa18_rgb4_colour(uint16_t rgb4);

/* Deplanarize an observed 320x200, four-plane display page into the port's
 * native indexed render target. */
void fa18_decode_planar_page(const FA18PlanarPage *page,
                             FA18IndexedFrameBuffer *framebuffer);

/* Pack the native indexed target into the semantic four-plane display page. */
int fa18_encode_planar_page(const FA18IndexedFrameBuffer *framebuffer,
                            FA18PlanarPage *page);

/* Translate an indexed native framebuffer through named palette state. */
int fa18_apply_palette(const FA18IndexedFrameBuffer *framebuffer,
                       const FA18Palette *palette,
                       uint16_t rgb444[FA18_WIDTH * FA18_HEIGHT]);

#endif
