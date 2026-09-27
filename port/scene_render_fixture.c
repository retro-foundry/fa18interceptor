#include "scene_render_fixture.h"

#include <stdio.h>
#include <string.h>

#include "viewport_palette.h"

enum {
    C279_SLOW_PACKET_X_OFFSET = 0x45a72,
    C279_SLOW_PACKET_Z_OFFSET = 0x45a76,
    C279_SLOW_PACKET_DEPTH_OFFSET = 0x45a78,
    C279_SLOW_MATRIX_OFFSET = 0x45bd8,
    C279_SLOW_PACKET_MODE_OFFSET = 0x4586b,
    C279_SLOW_DISPLAY_BOUND_OFFSET = 0x45984
};

static const long source_plane_offsets[FA18_COPPER_PAGE_PLANES] = {
    /* run075's proved `$5200` page family; external Chip-RAM offsets only. */
    0x04db30L, 0x04fa70L, 0x0519b0L, 0x0538f0L, 0x055830L
};

static int load_capture_page(FA18FivePlanePage *page, const char *chip_capture_path) {
    FILE *capture;

    if (!page || !chip_capture_path) return -1;
    capture = fopen(chip_capture_path, "rb");
    if (!capture) return -1;
    for (unsigned plane = 0; plane < FA18_COPPER_PAGE_PLANES; ++plane) {
        if (fseek(capture, source_plane_offsets[plane], SEEK_SET) != 0 ||
            fread(page->planes[plane], 1, FA18_COPPER_PAGE_BYTES, capture) !=
                FA18_COPPER_PAGE_BYTES) {
            fclose(capture);
            return -1;
        }
    }
    fclose(capture);
    return 0;
}

static int read_capture_bytes(FILE *capture, long offset, uint8_t *bytes, size_t count) {
    return !capture || !bytes || fseek(capture, offset, SEEK_SET) != 0 ||
           fread(bytes, 1, count, capture) != count ? -1 : 0;
}

static int read_capture_word(FILE *capture, long offset, int16_t *value) {
    uint8_t bytes[2];
    if (!value || read_capture_bytes(capture, offset, bytes, sizeof bytes) != 0) return -1;
    *value = (int16_t)fa18_be16(bytes);
    return 0;
}

static int read_capture_long(FILE *capture, long offset, int32_t *value) {
    uint8_t bytes[4];
    if (!value || read_capture_bytes(capture, offset, bytes, sizeof bytes) != 0) return -1;
    *value = (int32_t)fa18_be32(bytes);
    return 0;
}

int fa18_initialize_scene_render_fixture(FA18SceneRenderFixture *fixture,
                                         const FA18Hunks *exe,
                                         const char *chip_capture_path,
                                         FA18Video *video) {
    uint16_t palette[FA18_VIEWPORT_PALETTE_WORDS];

    if (!fixture || !exe || !chip_capture_path || !video) return -1;
    memset(fixture, 0, sizeof *fixture);
    fa18_five_plane_page_init(&fixture->page);
    /* The recorded frame-384 triangle outcomes use only the direct `$C2FA7E`
     * route and `$C3025A` return, neither of which reads the remaining
     * submission inputs. Any unexpected far route fails through its absent
     * callbacks instead of interpreting these zero values as source state. */
    if (fa18_load_viewport_mode_palette(exe, 8, palette) != 0 ||
        fa18_five_plane_page_load_rgb4(&fixture->page, palette,
                                       FA18_VIEWPORT_PALETTE_WORDS) != 0)
        return -1;
    if (load_capture_page(&fixture->page, chip_capture_path) != 0) return -1;
    if (fa18_five_plane_page_present(&fixture->page, video) != 0) return -1;
    fixture->enabled = 1;
    return 0;
}

int fa18_present_scene_render_fixture(const FA18SceneRenderFixture *fixture,
                                      FA18Video *video) {
    if (!fixture || !fixture->enabled) return -1;
    return fa18_five_plane_page_present(&fixture->page, video);
}

int fa18_initialize_c279_render_fixture(FA18C279RenderFixture *fixture,
                                        const FA18Hunks *exe,
                                        const FA18ProjectionGrid *grid,
                                        const char *slow_capture_path,
                                        const char *chip_capture_path,
                                        FA18Video *video) {
    FA18ProjectionPacket packet;
    FA18ProjectionPairMatrix matrix;
    FA18ProjectionGridSetup setup;
    FA18PlanarPixelState initial_pixels = { 0, 1, -1, 0 };
    FA18LineStyle initial_lines = { 1, -1, 0, 0 };
    uint16_t palette[FA18_VIEWPORT_PALETTE_WORDS];
    int16_t display_bound_y;
    uint8_t packet_mode;
    FILE *capture;

    if (!fixture || !exe || !grid || !slow_capture_path || !video) return -1;
    memset(fixture, 0, sizeof *fixture);
    capture = fopen(slow_capture_path, "rb");
    if (!capture) return -1;
    if (read_capture_word(capture, C279_SLOW_PACKET_X_OFFSET, &packet.x) != 0 ||
        read_capture_word(capture, C279_SLOW_PACKET_Z_OFFSET, &packet.z) != 0 ||
        read_capture_long(capture, C279_SLOW_PACKET_DEPTH_OFFSET,
                          &packet.depth_metric) != 0 ||
        read_capture_bytes(capture, C279_SLOW_PACKET_MODE_OFFSET, &packet_mode, 1) != 0 ||
        read_capture_word(capture, C279_SLOW_DISPLAY_BOUND_OFFSET,
                          &display_bound_y) != 0)
        goto fail;
    packet.y = (int16_t)packet.depth_metric;
    for (unsigned index = 0; index < 9; ++index)
        if (read_capture_word(capture, C279_SLOW_MATRIX_OFFSET + (long)index * 2,
                              &matrix.words[index]) != 0)
            goto fail;
    fclose(capture);

    fa18_five_plane_page_init(&fixture->page);
    if (fa18_load_viewport_mode_palette(exe, 8, palette) != 0 ||
        fa18_five_plane_page_load_rgb4(&fixture->page, palette,
                                       FA18_VIEWPORT_PALETTE_WORDS) != 0 ||
        load_capture_page(&fixture->page, chip_capture_path) != 0 ||
        fa18_flight_renderer_page_init(&fixture->renderer, &fixture->page,
                                       &initial_pixels, &initial_lines,
                                       display_bound_y, 0, 0, 0, 0, 0) != 0 ||
        fa18_initialize_projection_grid_packet(
            grid, packet_mode, packet.depth_metric, packet.y, packet.x, packet.z,
            &fixture->packet_state, &setup, &fixture->packet_route) != 0 ||
        fixture->packet_route != FA18_PROJECTION_GRID_PACKET_READY ||
        fa18_flight_renderer_page_apply_projection_grid_packet_state(
            &fixture->renderer, &fixture->packet_state) != 0 ||
        fa18_render_flight_projection_grid(
            grid, &packet, &matrix, packet_mode, display_bound_y,
            fa18_flight_renderer_page_submission(&fixture->renderer),
            &fixture->packet_state, &fixture->packet_route,
            &fixture->submitted_record_count) != 0 ||
        fa18_five_plane_page_present(&fixture->page, video) != 0)
        return -1;

    fixture->enabled = 1;
    return 0;

fail:
    fclose(capture);
    return -1;
}

int fa18_present_c279_render_fixture(const FA18C279RenderFixture *fixture,
                                     FA18Video *video) {
    if (!fixture || !fixture->enabled) return -1;
    return fa18_five_plane_page_present(&fixture->page, video);
}
