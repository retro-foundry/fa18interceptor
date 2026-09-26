#include "renderer.h"
#include "run075_frame395_data.h"
#include "run075_frame398_data.h"
#include "run075_frame402_data.h"
#include "run075_frame405_data.h"
#include "run075_frame408_data.h"
#include "run075_frame414_data.h"
#include "run075_frame460_data.h"
#include "run075_frame462_data.h"
#include "run075_hud_deltas.h"
#include "run075_tail_deltas.h"

#include <stdio.h>
#include <string.h>

int main(void) {
    /* Deplanarized before/after fixture from the exact four Chip-RAM planes
     * around run075 frame-315 $C2F688 entry. See the focused routine report.
     */
    static FA18IndexedFrameBuffer framebuffer;
    memset(&framebuffer, 4, sizeof framebuffer);
    fa18_clear_renderer_work_buffer(&framebuffer);
    for (int x = 0; x < FA18_WIDTH * FA18_HEIGHT; ++x) {
        if (framebuffer.pixels[x] != 0) {
            fputs("renderer work-buffer clear contract failed\n", stderr);
            return 1;
        }
    }
    memset(&framebuffer, 4, sizeof framebuffer);
    for (int x = 170; x <= 172; ++x) framebuffer.pixels[100 * FA18_WIDTH + x] = 11;
    framebuffer.pixels[100 * FA18_WIDTH + 173] = 4;
    FA18RendererState state = {0x0b, 0x0f, -1, 0};
    if (fa18_apply_pixel_mask(&framebuffer, &state, FA18_PIXEL_TWO_ROWS, 172, 99) != 0 ||
        framebuffer.pixels[99 * FA18_WIDTH + 170] != 4 ||
        framebuffer.pixels[99 * FA18_WIDTH + 171] != 11 ||
        framebuffer.pixels[99 * FA18_WIDTH + 172] != 11 ||
        framebuffer.pixels[99 * FA18_WIDTH + 173] != 4 ||
        framebuffer.pixels[100 * FA18_WIDTH + 170] != 11 ||
        framebuffer.pixels[100 * FA18_WIDTH + 171] != 11 ||
        framebuffer.pixels[100 * FA18_WIDTH + 172] != 11 ||
        framebuffer.pixels[100 * FA18_WIDTH + 173] != 4) {
        fputs("run075 frame-315 two-row pixel contract failed\n", stderr);
        return 1;
    }

    /* run060 $C2F688 primary path: x=100, y=125, mode 13 chooses
     * $C2F8B2. The original plane words change from lane-1-only to lanes 2/3.
     */
    memset(&framebuffer, 0, sizeof framebuffer);
    framebuffer.pixels[125 * FA18_WIDTH + 100] = 2;
    state = (FA18RendererState){0x0d, 0x0f, -1, 0};
    if (fa18_apply_pixel_mask(&framebuffer, &state, FA18_PIXEL_PRIMARY, 100, 125) != 0 ||
        framebuffer.pixels[125 * FA18_WIDTH + 100] != 12) {
        fputs("run060 primary pixel contract failed\n", stderr);
        return 1;
    }

    /* $C2F718 output-XOR returns before handler dispatch. */
    framebuffer.pixels[125 * FA18_WIDTH + 100] = 2;
    state = (FA18RendererState){0x0d, 0x0f, 0, 0x05};
    if (fa18_apply_pixel_mask(&framebuffer, &state, FA18_PIXEL_PRIMARY, 100, 125) != 0 ||
        framebuffer.pixels[125 * FA18_WIDTH + 100] != 7) {
        fputs("renderer output-XOR contract failed\n", stderr);
        return 1;
    }

    /* Primary mode one is $C2F83A all-lane XOR, not color index one. */
    framebuffer.pixels[125 * FA18_WIDTH + 100] = 3;
    state = (FA18RendererState){1, 0x0a, -1, 0};
    if (fa18_apply_pixel_mask(&framebuffer, &state, FA18_PIXEL_PRIMARY, 100, 125) != 0 ||
        framebuffer.pixels[125 * FA18_WIDTH + 100] != 9) {
        fputs("primary all-XOR handler contract failed\n", stderr);
        return 1;
    }
    FA18IndexedFrameBuffer previous;
    memset(&previous, 0, sizeof previous);
    previous.pixels[1 * FA18_WIDTH] = 1;
    FA18IndexedFrameBuffer scene;
    if (FA18_RUN075_FRAME395_SPANS != 591 ||
        fa18_render_run075_frame395_scene(&previous, &scene) != 0 ||
        scene.pixels[1 * FA18_WIDTH] != 1 ||
        scene.pixels[101 * FA18_WIDTH + 100] != 5) {
        fputs("run075 frame-395 scene contract failed\n", stderr);
        return 1;
    }
    if (FA18_RUN075_FRAME398_SPANS != 857 ||
        fa18_render_run075_frame398_scene(&scene, &previous) != 0 ||
        previous.pixels[1 * FA18_WIDTH] != 6) {
        fputs("run075 frame-398 scene contract failed\n", stderr);
        return 1;
    }
    if (FA18_RUN075_FRAME402_SPANS != 1396 ||
        fa18_render_run075_frame402_scene(&previous, &scene) != 0) {
        fputs("run075 frame-402 scene contract failed\n", stderr);
        return 1;
    }
    if (FA18_RUN075_FRAME405_SPANS != 1850 ||
        fa18_render_run075_frame405_scene(&scene, &previous) != 0) {
        fputs("run075 frame-405 scene contract failed\n", stderr);
        return 1;
    }
    if (FA18_RUN075_FRAME408_SPANS != 1732 ||
        fa18_render_run075_frame408_scene(&previous, &scene) != 0) {
        fputs("run075 frame-408 scene contract failed\n", stderr);
        return 1;
    }
    if (FA18_RUN075_FRAME414_SPANS != 1403 ||
        fa18_render_run075_frame414_scene(&previous, &scene) != 0) {
        fputs("run075 frame-414 scene contract failed\n", stderr);
        return 1;
    }
    if (FA18_RUN075_FRAME460_SPANS != 10 ||
        fa18_render_run075_frame460_scene(&previous, &scene) != 0) {
        fputs("run075 frame-460 scene contract failed\n", stderr);
        return 1;
    }
    if (FA18_RUN075_FRAME462_SPANS != 12 ||
        fa18_render_run075_frame462_scene(&previous, &scene) != 0) {
        fputs("run075 frame-462 scene contract failed\n", stderr);
        return 1;
    }
    uint16_t hud_previous[FA18_WIDTH * FA18_HEIGHT] = {0};
    uint16_t hud_output[FA18_WIDTH * FA18_HEIGHT] = {0};
    if (FA18_RUN075_HUD_DELTA_COUNT != 114 ||
        fa18_apply_run075_hud_delta(464, hud_previous, hud_output) != 0 ||
        fa18_apply_run075_hud_delta(500, hud_previous, hud_output) != 0 ||
        fa18_apply_run075_hud_delta(559, hud_previous, hud_output) != 0 ||
        fa18_apply_run075_hud_delta(586, hud_previous, hud_output) != 0 ||
        fa18_apply_run075_hud_delta(614, hud_previous, hud_output) != 0 ||
        fa18_apply_run075_hud_delta(641, hud_previous, hud_output) != 0 ||
        fa18_apply_run075_hud_delta(661, hud_previous, hud_output) != 0 ||
        fa18_apply_run075_hud_delta(793, hud_previous, hud_output) != 0 ||
        fa18_apply_run075_hud_delta(881, hud_previous, hud_output) != 0 ||
        fa18_apply_run075_hud_delta(894, hud_previous, hud_output) != 0 ||
        fa18_apply_run075_hud_delta(907, hud_previous, hud_output) != 0 ||
        fa18_apply_run075_hud_delta(932, hud_previous, hud_output) != 0 ||
        fa18_apply_run075_hud_delta(945, hud_previous, hud_output) != 0 ||
        fa18_apply_run075_hud_delta(970, hud_previous, hud_output) != 0 ||
        fa18_apply_run075_hud_delta(1007, hud_previous, hud_output) != 0 ||
        fa18_apply_run075_hud_delta(1047, hud_previous, hud_output) != 0 ||
        fa18_apply_run075_hud_delta(1237, hud_previous, hud_output) != 0 ||
        fa18_apply_run075_hud_delta(1390, hud_previous, hud_output) != 0 ||
        fa18_apply_run075_hud_delta(1454, hud_previous, hud_output) != 0 ||
        fa18_apply_run075_hud_delta(1521, hud_previous, hud_output) != 0 ||
        fa18_apply_run075_hud_delta(1609, hud_previous, hud_output) != 0 ||
        fa18_apply_run075_hud_delta(1817, hud_previous, hud_output) != 0 ||
        fa18_apply_run075_hud_delta(1938, hud_previous, hud_output) != 0 ||
        fa18_apply_run075_hud_delta(1995, hud_previous, hud_output) != 0 ||
        FA18_RUN075_TAIL_DELTA_COUNT != 1966 ||
        fa18_apply_run075_hud_delta(2001, hud_previous, hud_output) != 0 ||
        fa18_apply_run075_hud_delta(20985, hud_previous, hud_output) != 0) {
        fputs("run075 HUD delta contract failed\n", stderr);
        return 1;
    }
    puts("run075 two-row pixel contract passed");
    return 0;
}
