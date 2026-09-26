#include "line.h"

#include <stdlib.h>
#include <string.h>

int fa18_build_run060_frame7992_line_jobs(FA18LineBlitJob jobs[4]) {
    static const FA18LineBlitJob captured[4] = {
        {0x0fce, 0x0000, 0xffff, 0xffff, 3, 1, 101, 0, 1, 1, 5, 5, 18, 12},
        {0x0fce, 0x0000, 0xffff, 0xffff, 2, 1, 133, 1, 1, 1, 5, 5, 18, 12},
        {0x0fce, 0x0000, 0xffff, 0xffff, 1, 1, 133, 2, 1, 1, 5, 5, 18, 12},
        {0x0fce, 0x0000, 0xffff, 0xffff, 0, 1, 133, 3, 1, 1, 5, 5, 18, 12}
    };
    if (!jobs) return -1;
    memcpy(jobs, captured, sizeof captured);
    return 0;
}

int fa18_validate_line_packet(const FA18LinePacket *packet) {
    return packet && packet->active_plane_mask != 0 &&
           packet->destination_byte_offset < 40u * 200u;
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

int fa18_draw_line(FA18IndexedFrameBuffer *framebuffer,
                   const FA18LineStyle *style, FA18LineSegment segment,
                   int16_t row_limit) {
    if (!framebuffer || !style || row_limit < 0 || row_limit >= FA18_HEIGHT) return -1;

    /* $C2FA7E advances the low endpoint row before preparing line mode. On
     * a nonhorizontal line this makes the supplied vertical extent one pixel
     * shorter. The routine traverses upward inputs in reverse endpoint order.
     */
    int x, y, end_x, end_y;
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
    if (end_y > row_limit) return -1; /* Original has a source-derived clip path. */

    const int dx = end_x - x;
    const int dy = end_y - y;
    const int abs_dx = abs(dx);
    const int abs_dy = abs(dy);
    const int x_step = dx < 0 ? -1 : 1;
    const int y_step = dy < 0 ? -1 : 1;
    const int x_major = abs_dx >= abs_dy;
    const int major = x_major ? abs_dx : abs_dy;
    const int minor = x_major ? abs_dy : abs_dx;

    /* These are the source's BLTAPTL, BLTBMOD and BLTAMOD relationships:
     * 4*minor-2*major, 4*minor and 4*(minor-major). Applying the sign before
     * each advance matches the OCS line-mode sequence. */
    int error = 4 * minor - 2 * major;
    const int minor_error_step = 4 * minor;
    const int diagonal_error_step = 4 * (minor - major);
    for (int step = 0; step <= major; ++step) {
        if (set_line_pixel(framebuffer, style, x, y) != 0) return -1;
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
