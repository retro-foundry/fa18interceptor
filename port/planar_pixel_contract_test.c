#include "planar_pixel.h"

#include <assert.h>
#include <string.h>

int main(void) {
    FA18Video video;
    memset(&video, 0, sizeof video);
    FA18PlanarPixelState state = { 13, 15, -1, 0 };

    video.pixels[125 * FA18_WIDTH + 100] = 0x12;
    assert(fa18_apply_planar_pixel_mask(&video, &state,
                                        FA18_PLANAR_PIXEL_PRIMARY, 100, 125) == 0);
    assert(video.pixels[125 * FA18_WIDTH + 100] == 0x1c);

    memset(&video, 0, sizeof video);
    state = (FA18PlanarPixelState){ 11, 15, -1, 0 };
    assert(fa18_apply_planar_pixel_mask(&video, &state,
                                        FA18_PLANAR_PIXEL_TWO_ROWS, 172, 99) == 0);
    assert(video.pixels[99 * FA18_WIDTH + 172] == 11);
    assert(video.pixels[100 * FA18_WIDTH + 172] == 11);

    video.pixels[125 * FA18_WIDTH + 100] = 0x12;
    state = (FA18PlanarPixelState){ 13, 15, 0, 5 };
    assert(fa18_apply_planar_pixel_mask(&video, &state,
                                        FA18_PLANAR_PIXEL_PRIMARY, 100, 125) == 0);
    assert(video.pixels[125 * FA18_WIDTH + 100] == 0x17);

    assert(fa18_apply_planar_pixel_mask(&video, &state,
                                        FA18_PLANAR_PIXEL_PRIMARY, 0, 0) == 1);
    assert(fa18_apply_planar_pixel_mask(&video, &state,
                                        FA18_PLANAR_PIXEL_PRIMARY, FA18_WIDTH, 1) == -1);
    assert(fa18_apply_planar_pixel_mask(0, &state,
                                        FA18_PLANAR_PIXEL_PRIMARY, 1, 1) == -1);
    return 0;
}
