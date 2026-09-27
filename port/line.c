#include "line.h"

#include <stdlib.h>
#include <string.h>

static uint16_t apply_line_minterm(uint8_t logic_function,
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

static uint16_t line_read_word(const uint8_t *plane, size_t offset) {
    return (uint16_t)(((uint16_t)plane[offset] << 8) | plane[offset + 1u]);
}

static void line_write_word(uint8_t *plane, size_t offset, uint16_t value) {
    plane[offset] = (uint8_t)(value >> 8);
    plane[offset + 1u] = (uint8_t)value;
}

int fa18_validate_line_packet(const FA18LinePacket *packet) {
    return packet && packet->active_plane_mask != 0 &&
           packet->destination_byte_offset < 40u * 200u;
}

int fa18_validate_line_blit_job(const FA18LineBlitJob *job) {
    if (!job || job->destination_plane >= 4u ||
        !(job->bltcon1 & 1u) || job->width_words == 0u ||
        job->width_words > FA18_WIDTH / 16 || job->height_rows == 0u ||
        job->destination_byte_offset >= (FA18_WIDTH / 8) * FA18_HEIGHT ||
        job->destination_byte_offset + job->width_words * 2u >
            (FA18_WIDTH / 8) * FA18_HEIGHT || job->bltadat != 0x8000u ||
        job->bltbdat != 0xffffu || job->bltcmod != 0x28u ||
        job->bltdmod != 0x28u) return -1;
    return 0;
}

int fa18_execute_line_blit_job(uint8_t *plane, size_t plane_bytes,
                               const FA18LineBlitJob *job) {
    if (!plane || !job || fa18_validate_line_blit_job(job) != 0 ||
        plane_bytes < (size_t)(FA18_WIDTH / 8) * FA18_HEIGHT) return -1;
    uint16_t con0 = job->bltcon0;
    uint16_t con1 = job->bltcon1;
    int16_t apt = (int16_t)job->bltapt_low;
    unsigned ashift = (con0 >> 12) & 15u;
    unsigned bshift = (con1 >> 12) & 15u;
    uint16_t bline = (uint16_t)((job->bltb_source_word >> bshift) |
                                (job->bltb_source_word << ((16u - bshift) & 15u)));
    /* The captured C/D pointers are odd; Agnus performs the word cycle at
     * the preceding even byte address. */
    int64_t cpt = (int64_t)(job->destination_byte_offset & ~(size_t)1u);
    int64_t dpt = cpt;
    int one_dot = 0;
    int line_loop = 1;
    uint32_t aold = 0;
    for (uint16_t step = 0; step < job->height_rows; ++step) {
        const int emit = !(con1 & 2u) || !one_dot;
        one_dot = 1;
        const int sign = apt < 0;
        if (job->bltcon0 & 0x0200u)
            apt = (int16_t)(apt + (sign ? (int16_t)job->bltbmod :
                                             (int16_t)job->bltamod));
        if (cpt < 0 || cpt + 1 >= (int64_t)plane_bytes) return -1;
        const uint16_t c = (job->c_source_words && step < job->c_source_count)
            ? job->c_source_words[step]
            : line_read_word(plane, (size_t)cpt);
        const uint16_t b = (bline & 1u) ? 0xffffu : 0u;
        const uint16_t a = (uint16_t)(job->bltadat >> ashift);
        const uint16_t d = apply_line_minterm(
            (uint8_t)(con0 & 0xffu), a, b, c);

        /* BLTCON1 X and Y pointer stages. */
        int overflow = 0;
        if (!sign && !(con1 & 0x10u)) {
            if (con1 & 0x08u) {
                if (ashift == 0u) cpt -= 2;
                overflow = -1;
            } else {
                if (ashift == 15u) cpt += 2;
                overflow = 1;
            }
        }
        if (con1 & 0x10u) {
            if (con1 & 0x04u) {
                if (ashift == 0u) cpt -= 2;
                overflow = -1;
            } else {
                if (ashift == 15u) cpt += 2;
                overflow = 1;
            }
        }
        aold = (aold << 16) | (job->bltadat & job->first_mask);
        aold >>= ashift;
        if (overflow) {
            ashift = (unsigned)((int)ashift + overflow) & 15u;
            con0 = (uint16_t)((con0 & 0x0fffu) | (uint16_t)(ashift << 12));
        }
        if (line_loop && (con0 & 0x0200u)) {
            if (!sign && (con1 & 0x10u)) {
                cpt += (con1 & 0x08u) ? -(int64_t)job->bltcmod : job->bltcmod;
            }
            if (!(con1 & 0x10u)) {
                cpt += (con1 & 0x04u) ? -(int64_t)job->bltcmod : job->bltcmod;
            }
        }
        line_loop = 1;
        con1 = (uint16_t)((con1 & 0x0fffu) | ((apt < 0) ? 0x40u : 0u));
        bshift = (bshift + 15u) & 15u;
        bline = (uint16_t)((((uint32_t)0 << 16) | job->bltb_source_word) >> bshift);
        if (emit) {
            if (dpt < 0 || dpt + 1 >= (int64_t)plane_bytes) return -1;
            line_write_word(plane, (size_t)dpt, d);
        }
        dpt = cpt;
    }
    return 0;
}

static int set_line_pixel(FA18IndexedFrameBuffer *framebuffer,
                          const FA18LineStyle *style, int x, int y) {
    if (x < 0 || x >= FA18_WIDTH || y < 0 || y >= FA18_HEIGHT) return -1;
    const uint8_t active = style->active_plane_mask & 15u;
    const uint8_t selected = (style->plane_mode < 0
        ? style->control_plane_bits : style->plane_bits) & 15u;
    uint8_t *pixel = &framebuffer->pixels[y * FA18_WIDTH + x];
    if (*pixel > 15u) return -1;
    *pixel = (uint8_t)((*pixel & (uint8_t)~active) | (selected & active));
    return 0;
}

static int set_line_page_pixel(FA18LinePlanarPage *page,
                               const FA18LineStyle *style, int x, int y) {
    const uint8_t active = style->active_plane_mask & 15u;
    const uint8_t selected = (style->plane_mode < 0
        ? style->control_plane_bits : style->plane_bits) & 15u;
    const size_t offset = (size_t)y * FA18_LINE_PLANAR_ROW_BYTES +
                          (size_t)x / 8u;
    const uint8_t bit = (uint8_t)(0x80u >> (x & 7));
    if (!page || x < 0 || x >= FA18_WIDTH || y < 0 || y >= FA18_HEIGHT ||
        page->bytes_per_lane < FA18_LINE_PLANAR_PAGE_BYTES)
        return -1;
    for (unsigned lane = 0; lane < FA18_LINE_PLANAR_LANES; ++lane) {
        if (!page->lanes[lane]) return -1;
        if (active & (uint8_t)(1u << lane)) {
            if (selected & (uint8_t)(1u << lane)) page->lanes[lane][offset] |= bit;
            else page->lanes[lane][offset] &= (uint8_t)~bit;
        }
    }
    return 0;
}

static int set_line_framebuffer_adapter(void *target, const FA18LineStyle *style,
                                        int x, int y) {
    return set_line_pixel(target, style, x, y);
}

static int set_line_page_adapter(void *target, const FA18LineStyle *style,
                                 int x, int y) {
    return set_line_page_pixel(target, style, x, y);
}

static int draw_line_points(const FA18LineStyle *style, FA18LineSegment segment,
                            int16_t row_limit, void *target,
                            int (*set_pixel)(void *, const FA18LineStyle *, int, int)) {
    int x, y, end_x, end_y;
    if (!style || !target || !set_pixel || row_limit < 0 || row_limit >= FA18_HEIGHT)
        return -1;
    if (segment.y0 < segment.y1) {
        x = segment.x0; y = segment.y0 + 1;
        end_x = segment.x1; end_y = segment.y1;
    } else if (segment.y0 > segment.y1) {
        x = segment.x1; y = segment.y1 + 1;
        end_x = segment.x0; end_y = segment.y0;
    } else {
        x = segment.x0; y = segment.y0 + 1;
        end_x = segment.x1; end_y = segment.y1 + 1;
    }
    if (y > row_limit) return 1;
    if (end_y > row_limit) return -1;

    const int dx = end_x - x, dy = end_y - y;
    const int abs_dx = abs(dx), abs_dy = abs(dy);
    const int x_step = dx < 0 ? -1 : 1, y_step = dy < 0 ? -1 : 1;
    const int x_major = abs_dx >= abs_dy;
    const int major = x_major ? abs_dx : abs_dy;
    const int minor = x_major ? abs_dy : abs_dx;
    int error = 4 * minor - 2 * major;
    const int minor_error_step = 4 * minor;
    const int diagonal_error_step = 4 * (minor - major);
    for (int step = 0; step <= major; ++step) {
        if (set_pixel(target, style, x, y) != 0) return -1;
        const int was_negative = error < 0;
        error += was_negative ? minor_error_step : diagonal_error_step;
        if (!was_negative) {
            if (x_major) y += y_step;
            else x += x_step;
        }
        if (x_major) x += x_step;
        else y += y_step;
    }
    return 0;
}

int fa18_draw_line(FA18IndexedFrameBuffer *framebuffer,
                   const FA18LineStyle *style, FA18LineSegment segment,
                   int16_t row_limit) {
    /* $C2FA7E advances the low endpoint row before preparing line mode. On
     * a nonhorizontal line this makes the supplied vertical extent one pixel
     * shorter. The routine traverses upward inputs in reverse endpoint order.
     */
    /* These are the source's BLTAPTL, BLTBMOD and BLTAMOD relationships:
     * 4*minor-2*major, 4*minor and 4*(minor-major). Applying the sign before
     * each advance matches the OCS line-mode sequence. */
    return draw_line_points(style, segment, row_limit, framebuffer,
                            set_line_framebuffer_adapter);
}

int fa18_draw_line_to_page(FA18LinePlanarPage *page,
                           const FA18LineStyle *style, FA18LineSegment segment,
                           int16_t row_limit) {
    return draw_line_points(style, segment, row_limit, page, set_line_page_adapter);
}

int fa18_emit_line_to_page(void *context, int16_t x0, int16_t y0,
                           int16_t x1, int16_t y1, int16_t row_limit) {
    const FA18PlanarLinePageContext *line = context;
    if (!line) return -1;
    return fa18_draw_line_to_page(line->page, line->style,
                                  (FA18LineSegment){ x0, y0, x1, y1 }, row_limit);
}
