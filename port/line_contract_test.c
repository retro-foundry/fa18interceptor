#include "line.h"
#include "run075_frame395_lines.h"
#include "run075_frame395_line_packet.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>

int main(void) {
    FA18LineBlitJob jobs[4];
    assert(fa18_build_run060_frame7992_line_jobs(jobs) == 0);
    assert(jobs[0].bltcon0 == 0x0fce && jobs[0].a_source == 0x00a230);
    assert(jobs[0].b_source == 0x00b038 && jobs[0].c_source == 0x019e4a);
    assert(jobs[1].b_source == 0x00ae88 && jobs[1].c_source == 0x017f0a);
    assert(jobs[2].b_source == 0x00acd8 && jobs[2].c_source == 0x015fca);
    assert(jobs[3].b_source == 0x00ab28 && jobs[3].c_source == 0x01408a);
    assert(jobs[3].width_height == 0x0312);
    assert(jobs[0].a_modulus == 1 && jobs[0].b_modulus == 1 &&
           jobs[0].c_modulus == 5 && jobs[0].d_modulus == 5);
    assert(FA18_RUN075_FRAME395_LINES == 12);
    assert(fa18_run075_frame395_lines[0].x0 == 175);
    assert(fa18_run075_frame395_lines[11].x1 == 199);
    assert(fa18_validate_line_packet(&fa18_run075_frame395_line_packet));
    assert(fa18_run075_frame395_line_packet.bltsize == 0x005d);
    assert(fa18_run075_frame395_line_packet.destination_byte_offset == 0x0ec5);
    assert(FA18_RUN075_FRAME395_LINE_PACKETS == 4);
    assert(fa18_run075_frame395_line_packets[1].bltcon1 == 81);
    assert(fa18_run075_frame395_line_packets[2].bltbmod == 0xffe6);
    assert(fa18_run075_frame395_line_packets[2].bltsize == 0x0382);
    assert(fa18_run075_frame395_line_packets[3].destination_byte_offset == 0x0ec6);
    /* Generic raster contract. A settled run060+ visual fixture is still
     * required before this primitive can be promoted to port parity. */
    FA18IndexedFrameBuffer framebuffer;
    memset(&framebuffer, 4, sizeof framebuffer);
    const FA18LineStyle style = {0x0f, -1, 0, 0x06};
    const FA18LineSegment segment = {180, 0, 202, 1};
    if (fa18_draw_line(&framebuffer, &style, segment, 179) != 0) {
        fputs("line fixture rejected\n", stderr);
        return 1;
    }
    for (int x = 0; x < FA18_WIDTH; ++x) {
        const uint8_t expected = x >= 180 && x <= 202 ? 6 : 4;
        if (framebuffer.pixels[FA18_WIDTH + x] != expected ||
            framebuffer.pixels[x] != 4) {
            fputs("line-mode raster contract failed\n", stderr);
            return 1;
        }
    }

    /* Equal-Y behavior remains covered as a pure native recurrence test. */
    memset(&framebuffer, 4, sizeof framebuffer);
    if (fa18_draw_line(&framebuffer, &style,
                       (FA18LineSegment){219, 0, 225, 0}, 179) != 0) {
        fputs("horizontal line fixture rejected\n", stderr);
        return 1;
    }
    for (int x = 0; x < FA18_WIDTH; ++x) {
        const uint8_t expected = x >= 219 && x <= 225 ? 6 : 4;
        if (framebuffer.pixels[FA18_WIDTH + x] != expected ||
            framebuffer.pixels[x] != 4) {
            fputs("horizontal line-mode raster contract failed\n", stderr);
            return 1;
        }
    }
    puts("line-mode raster contract passed");
    return 0;
}
