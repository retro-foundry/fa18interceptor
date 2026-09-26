#include "renderer.h"

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
    puts("run075 two-row pixel contract passed");
    return 0;
}
