#include "video.h"

void fa18_video_to_rgb444(const FA18Video *video, uint16_t *out) {
    for (int i = 0; i < FA18_PIXELS; ++i) out[i] = video->palette[video->pixels[i] & 31] & 0x0FFF;
}
