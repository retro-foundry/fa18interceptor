#ifndef FA18_VIDEO_H
#define FA18_VIDEO_H

#include <stdint.h>

#include "dimensions.h"

/* The game's display: 320x200 palette indices plus the 32-entry palette, in
 * the Amiga's 12-bit 0x0RGB colour format. */
typedef struct {
    uint8_t pixels[FA18_PIXELS];
    uint16_t palette[32];
} FA18Video;

/* Convert to 0x0RGB per pixel, the port's output format. */
void fa18_video_to_rgb444(const FA18Video *video, uint16_t *out);

#endif
