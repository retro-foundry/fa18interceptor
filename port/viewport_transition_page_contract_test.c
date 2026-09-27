#include "five_plane_page.h"
#include "outer_loop_child.h"

#include <assert.h>

static void put_be16(uint8_t *bytes, uint16_t value) {
    bytes[0] = (uint8_t)(value >> 8);
    bytes[1] = (uint8_t)value;
}

static int wait_viewport(void *context) {
    return context ? 0 : -1;
}

int main(void) {
    enum { table_offset = 0x80, stride = FA18_VIEWPORT_PALETTE_WORDS * 2 };
    uint8_t table[table_offset + FA18_VIEWPORT_PALETTE_MODE_COUNT * stride] = { 0 };
    FA18HunkSegment segments[22] = { 0 };
    FA18Hunks exe = { segments, 22 };
    uint8_t copper_bytes[] = {
        0x01, 0x80, 0, 0, 0x01, 0x82, 0, 0,
        0x01, 0x9e, 0, 0, 0xff, 0xff, 0xff, 0xfe
    };
    FA18CopperMutableInstructionStream stream = { copper_bytes, sizeof copper_bytes };
    const uint32_t left[] = { 1, 2 }, right[] = { 3, 4 };
    FA18ViewportPaletteBuffer palette_buffer;
    FA18ViewportModeBindings bindings = { 0, left, right, 2, &palette_buffer };
    FA18ViewportModeState mode = { 14, 15, 0, 0 };
    FA18ViewportModeStep mode_step;
    FA18FivePlanePage page;
    FA18OuterLoopChildOps ops = { wait_viewport, fa18_five_plane_page_load_rgb4, &page };
    FA18OuterLoopChildState outer = { 0, 0, 0, &palette_buffer };
    FA18OuterLoopChildStep outer_step;

    segments[21] = (FA18HunkSegment){ .data = table, .size = sizeof table };
    for (unsigned mode_index = 0; mode_index < FA18_VIEWPORT_PALETTE_MODE_COUNT;
         ++mode_index) {
        const uint32_t offset = table_offset +
            (FA18_VIEWPORT_PALETTE_MODE_COUNT - 1u - mode_index) * stride;
        for (unsigned colour = 0; colour < FA18_VIEWPORT_PALETTE_WORDS; ++colour)
            put_be16(table + offset + colour * 2u,
                     (uint16_t)((mode_index << 8) | colour));
    }
    assert(fa18_initialize_viewport_palette_buffer(&exe, &palette_buffer) == 0);
    assert(palette_buffer.words[0] == 0x0f00 && palette_buffer.words[16] == 0x0e00);
    fa18_five_plane_page_init(&page);
    assert(fa18_advance_viewport_mode(&mode, &bindings, &exe, &stream, 1, &mode_step) == 0);
    assert(mode.current == 15 && mode.state == 3 && mode_step.mode_words_copied);
    assert(palette_buffer.words[0] == 0x0f00 && palette_buffer.words[15] == 0x0f0f);
    assert(palette_buffer.words[16] == 0x0e00 && palette_buffer.words[31] == 0x0e0f);

    assert(fa18_advance_outer_loop_idle_child(&outer, &mode, &ops, &outer_step) == 0);
    assert(mode.state == 2 && outer_step.dynamic_palette_loaded);
    assert(page.display_state.palette[0] == 0x0f00 &&
           page.display_state.palette[15] == 0x0f0f &&
           page.display_state.palette[16] == 0x0e00 &&
           page.display_state.palette[31] == 0x0e0f);
    return 0;
}
