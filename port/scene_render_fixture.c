#include "scene_render_fixture.h"

#include <stdio.h>
#include <string.h>

#include "viewport_palette.h"

int fa18_initialize_scene_render_fixture(FA18SceneRenderFixture *fixture,
                                         const FA18Hunks *exe,
                                         const char *chip_capture_path,
                                         FA18Video *video) {
    /* run075 frame-392's proved `$5200` page family. These are offsets into
     * an external diagnostic Chip-RAM capture, never native page identities. */
    static const long source_plane_offsets[FA18_COPPER_PAGE_PLANES] = {
        0x04db30L, 0x04fa70L, 0x0519b0L, 0x0538f0L, 0x055830L
    };
    uint16_t palette[FA18_VIEWPORT_PALETTE_WORDS];
    FILE *capture;

    if (!fixture || !exe || !chip_capture_path || !video) return -1;
    memset(fixture, 0, sizeof *fixture);
    fa18_five_plane_page_init(&fixture->page);
    if (fa18_load_viewport_mode_palette(exe, 8, palette) != 0 ||
        fa18_five_plane_page_load_rgb4(&fixture->page, palette,
                                       FA18_VIEWPORT_PALETTE_WORDS) != 0)
        return -1;
    capture = fopen(chip_capture_path, "rb");
    if (!capture) return -1;
    for (unsigned plane = 0; plane < FA18_COPPER_PAGE_PLANES; ++plane) {
        if (fseek(capture, source_plane_offsets[plane], SEEK_SET) != 0 ||
            fread(fixture->page.planes[plane], 1, FA18_COPPER_PAGE_BYTES,
                  capture) != FA18_COPPER_PAGE_BYTES) {
            fclose(capture);
            return -1;
        }
    }
    fclose(capture);
    if (fa18_five_plane_page_present(&fixture->page, video) != 0) return -1;
    fixture->enabled = 1;
    return 0;
}

int fa18_present_scene_render_fixture(const FA18SceneRenderFixture *fixture,
                                      FA18Video *video) {
    if (!fixture || !fixture->enabled) return -1;
    return fa18_five_plane_page_present(&fixture->page, video);
}
