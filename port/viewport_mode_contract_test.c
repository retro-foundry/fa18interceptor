#include "viewport_mode.h"

#include <assert.h>
#include <string.h>

static void put_be16(uint8_t *bytes, uint16_t value) {
    bytes[0] = (uint8_t)(value >> 8);
    bytes[1] = (uint8_t)value;
}

int main(void) {
    enum { table_offset = 0x80, stride = FA18_VIEWPORT_PALETTE_WORDS * 2 };
    uint8_t table[table_offset + FA18_VIEWPORT_PALETTE_MODE_COUNT * stride] = { 0 };
    FA18HunkSegment segments[22] = { 0 };
    FA18Hunks exe = { segments, 22 };
    uint8_t copper_bytes[] = {
        0x01, 0x80, 0x00, 0x00, 0x01, 0x82, 0x00, 0x00,
        0x01, 0x9e, 0x00, 0x00, 0xff, 0xff, 0xff, 0xfe
    };
    FA18CopperMutableInstructionStream stream = { copper_bytes, sizeof copper_bytes };
    FA18ViewportPaletteBuffer copied = { 0 };
    const uint32_t left[] = { 0x11111111u, 0x22222222u };
    const uint32_t right[] = { 0x33333333u, 0x44444444u };
    FA18ViewportModeBindings bindings = {
        0, left, right, 2, &copied
    };
    FA18ViewportModeState state = { 8, 10, 0, 0 };
    FA18ViewportModeStep step;

    segments[21] = (FA18HunkSegment){ .data = table, .size = sizeof table };
    for (unsigned mode = 0; mode < FA18_VIEWPORT_PALETTE_MODE_COUNT; ++mode) {
        const uint32_t offset = table_offset +
            (FA18_VIEWPORT_PALETTE_MODE_COUNT - 1u - mode) * stride;
        for (unsigned colour = 0; colour < FA18_VIEWPORT_PALETTE_WORDS; ++colour)
            put_be16(table + offset + colour * 2u,
                     (uint16_t)((mode << 8) | colour));
    }

    assert(fa18_advance_viewport_mode(&state, &bindings, &exe, &stream, 1, &step) == 0);
    assert(state.current == 9 && state.countdown == 2 && !state.state);
    assert(step.palette_loaded && step.palette_load_count == 2 &&
           step.pointer_pair_published && step.pointer_pair_publish_count == 2 &&
           !step.mode_words_copied);
    assert(step.publication.selected_index == 1 &&
           step.publication.selected_pointer_1 == left[1] &&
           step.publication.selected_pointer_2 == right[1]);
    assert(step.copper_updated_mask == ((UINT32_C(1) << 0) | (UINT32_C(1) << 1) |
                                        (UINT32_C(1) << 15)));
    assert(copper_bytes[2] == 0x09 && copper_bytes[3] == 0x00);
    assert(copied.words[0] == 0);

    assert(fa18_advance_viewport_mode(&state, &bindings, &exe, &stream, 1, &step) == 0 &&
           !step.palette_loaded && state.countdown == 1);
    assert(fa18_advance_viewport_mode(&state, &bindings, &exe, &stream, 1, &step) == 0 &&
           !step.palette_loaded && state.countdown == 0);
    assert(fa18_advance_viewport_mode(&state, &bindings, &exe, &stream, 1, &step) == 0);
    assert(state.current == 10 && state.countdown == 2 && state.state == 3);
    assert(step.palette_loaded && step.palette_load_count == 2 &&
           step.pointer_pair_published && step.pointer_pair_publish_count == 2 &&
           step.mode_words_copied);
    assert(copied.words[0] == 0x0a00 && copied.words[15] == 0x0a0f);

    bindings.outer_selected_index = 1;
    assert(fa18_advance_viewport_mode(&state, &bindings, &exe, &stream, 1, &step) == 0);
    assert(step.palette_loaded && step.palette_load_count == 1 &&
           !step.pointer_pair_published && !step.pointer_pair_publish_count &&
           !step.mode_words_copied);
    assert(copper_bytes[2] == 0x0a && copper_bytes[3] == 0x00);
    copied.words[0]=0x0bcd;
    assert(fa18_advance_viewport_mode(&state,&bindings,&exe,&stream,1,&step)==0);
    assert(copper_bytes[2]==0x0b && copper_bytes[3]==0xcd);
    bindings.mode_palette_buffer = NULL;
    state.current = 8;
    state.target = 9;
    state.countdown = 0;
    assert(fa18_advance_viewport_mode(&state, &bindings, &exe, &stream, 1, &step) == -1);
    return 0;
}
