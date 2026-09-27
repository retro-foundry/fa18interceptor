#include "menu_render.h"

#include <stdio.h>
#include <string.h>

int main(void) {
    uint8_t palette[32] = {0};
    uint8_t glyphs[0x16c + 0xc0 + 16] = {0};
    uint8_t text[0x2366 + 8] = {0};
    FA18HunkSegment segments[FA18_MENU_TEXT_HUNK + 1] = {{0}};
    palette[10] = 0x05;
    palette[11] = 0x5b;
    glyphs[0x16c + 2 * ('A' - 0x20)] = 0;
    glyphs[0x16c + 2 * ('A' - 0x20) + 1] = 0xc0;
    glyphs[0x16c + 0xc0] = 0xf0;
    text[0x2366 + 2] = 0;
    segments[21] = (FA18HunkSegment){.data = palette, .size = sizeof palette};
    segments[61] = (FA18HunkSegment){.data = glyphs, .size = sizeof glyphs};
    segments[64] = (FA18HunkSegment){.data = text, .size = sizeof text};
    FA18Hunks exe = {.segments = segments, .count = FA18_MENU_TEXT_HUNK + 1};
    static const uint8_t title[] = "A";
    FA18MenuRecord record = {.text = title, .text_length = 1, .layout_offset = 42,
                             .layout_index = 5};
    FA18Video video;
    if (fa18_render_top_level_menu(&video, &exe, &record, 1) != 0 ||
        video.palette[5] != 0x55b || video.pixels[FA18_WIDTH + 16] != 5 ||
        video.pixels[FA18_WIDTH + 19] != 5 || video.pixels[FA18_WIDTH + 20] != 0) {
        fputs("top-level menu glyph compositor contract failed\n", stderr);
        return 1;
    }
    if (fa18_render_top_level_menu(NULL, &exe, &record, 1) != -1) {
        fputs("top-level menu glyph compositor argument contract failed\n", stderr);
        return 1;
    }
    puts("top-level menu glyph compositor contract passed");
    return 0;
}
