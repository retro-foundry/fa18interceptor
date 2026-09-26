#include "menu.h"

#include <stdio.h>

int main(void) {
    FA18IndexedFrameBuffer framebuffer;
    uint16_t rgb444[FA18_WIDTH * FA18_HEIGHT];
    fa18_render_run075_frame200_menu(&framebuffer, rgb444);

    if (framebuffer.pixels[0] != 0 || rgb444[0] != 0x000 ||
        framebuffer.pixels[93] != 5 || rgb444[93] != 0x55b) {
        fprintf(stderr, "frame-200 menu fixture mismatch\n");
        return 1;
    }
    for (size_t i = 0; i < FA18_WIDTH * FA18_HEIGHT; ++i) {
        if (framebuffer.pixels[i] > 5) {
            fprintf(stderr, "invalid menu index at %zu\n", i);
            return 1;
        }
    }
    return 0;
}
