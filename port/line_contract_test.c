#include "line.h"
#include "run075_frame395_lines.h"
#include "run075_frame395_line_packet.h"
#include "run060_frame7992_line_packets.h"
#include "run060_frame7991_line_oracle.h"
#include "run060_packet12_c_source.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>

int main(void) {
    FA18AreaBlitJob jobs[4];
    assert(fa18_build_run060_frame7992_area_jobs(jobs) == 0);
    assert(jobs[0].bltcon0 == 0x0fce && jobs[0].destination_plane == 3);
    assert(jobs[0].destination_word == 1 && jobs[0].destination_row == 101);
    assert(jobs[1].destination_plane == 2 && jobs[1].source_asset_index == 1);
    assert(jobs[2].destination_plane == 1 && jobs[2].source_asset_index == 2);
    assert(jobs[3].destination_plane == 0 && jobs[3].source_asset_index == 3);
    assert(jobs[3].width_words == 18 && jobs[3].height_rows == 12);
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
    for (size_t i = 0; i < FA18_RUN060_FRAME7992_LINE_PACKET_COUNT; ++i)
        assert(fa18_validate_line_blit_job(&fa18_run060_frame7992_line_packets[i]) == 0);
    /* The first traced line submission is array packet 0 (DMA job 4). */
    uint8_t line_plane[FA18_WIDTH / 8 * FA18_HEIGHT] = {0};
    line_plane[0x0b7a] = 0xff;
    line_plane[0x0b7b] = 0xff;
    line_plane[0x0b7c] = 0xff;
    line_plane[0x0b7d] = 0xff;
    assert(fa18_execute_line_blit_job(
        line_plane, sizeof line_plane, &fa18_run060_frame7992_line_packets[0]) == 0);
    assert(line_plane[0x0b7a] == 0xff && line_plane[0x0b7b] == 0xf8);
    assert(line_plane[0x0b7c] == 0x3f && line_plane[0x0b7d] == 0xff);

    /* The complete snapshot validates the four short line groups. The first
     * long group (packet 12) remains the active recurrence investigation. */
    for (size_t i = 0; i < 12; ++i) {
        uint8_t snapshot[FA18_RUN060_LINE_PLANE_BYTES];
        const uint8_t plane_index = fa18_run060_line_oracle_planes[i];
        memcpy(snapshot, fa18_run060_frame7991_line_planes[plane_index],
               sizeof snapshot);
        assert(fa18_execute_line_blit_job(
            snapshot, sizeof snapshot,
            &fa18_run060_frame7992_line_packets[i]) == 0);
        for (size_t write = 0;
             write < fa18_run060_line_oracle_write_counts[i]; ++write) {
            const FA18LineOracleWrite expected =
                fa18_run060_line_oracle_writes[i][write];
            const uint16_t actual = (uint16_t)(((uint16_t)snapshot[expected.offset] << 8) |
                                               snapshot[expected.offset + 1]);
            assert(actual == expected.value);
        }
    }
    FA18LineBlitJob packet12 = fa18_run060_frame7992_line_packets[12];
    packet12.c_source_words = fa18_run060_packet12_c_source;
    packet12.c_source_count = 99;
    {
        uint8_t snapshot[FA18_RUN060_LINE_PLANE_BYTES];
        memcpy(snapshot, fa18_run060_frame7991_line_planes[0], sizeof snapshot);
        assert(fa18_execute_line_blit_job(snapshot, sizeof snapshot, &packet12) == 0);
        for (size_t write = 0; write < fa18_run060_line_oracle_write_counts[12]; ++write) {
            const FA18LineOracleWrite expected = fa18_run060_line_oracle_writes[12][write];
            const uint16_t actual = (uint16_t)(((uint16_t)snapshot[expected.offset] << 8) |
                                               snapshot[expected.offset + 1]);
            assert(actual == expected.value);
        }
    }
    /* Generic raster contract. */
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
