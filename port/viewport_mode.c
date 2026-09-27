#include "viewport_mode.h"

#include <string.h>

static int8_t signed_byte(uint8_t value) {
    return value <= INT8_MAX ? (int8_t)value : (int8_t)((int)value - 256);
}

static int load_palette(const FA18Hunks *exe, uint8_t mode,
                        FA18CopperMutableInstructionStream *streams,
                        size_t stream_count, FA18ViewportModeStep *step) {
    uint32_t updated = 0;
    if (fa18_load_viewport_mode_palette_into_copper(exe, mode, streams,
                                                    stream_count, &updated) != 0)
        return -1;
    step->palette_loaded = 1;
    ++step->palette_load_count;
    step->copper_updated_mask = updated;
    return 0;
}

int fa18_advance_viewport_mode(FA18ViewportModeState *state,
                               const FA18ViewportModeBindings *bindings,
                               const FA18Hunks *exe,
                               FA18CopperMutableInstructionStream *streams,
                               size_t stream_count,
                               FA18ViewportModeStep *step) {
    uint16_t selected_table_index;
    uint16_t palette[FA18_VIEWPORT_PALETTE_WORDS];

    if (!state || !bindings || !exe || !streams || !stream_count || !step ||
        !bindings->mode_palette_buffer ||
        !bindings->left_pointer_table || !bindings->right_pointer_table ||
        bindings->pointer_table_entries < 2 || bindings->outer_selected_index > 1)
        return -1;
    memset(step, 0, sizeof *step);
    if (state->current == state->target) {
        if (!state->state) return 0;
        return load_palette(exe, state->current, streams, stream_count, step);
    }

    --state->countdown;
    if (signed_byte(state->countdown) >= 0) return 0;
    if (signed_byte(state->target) <= signed_byte(state->current)) {
        --state->current;
        state->countdown = 1;
    } else {
        ++state->current;
        state->countdown = 2;
    }
    if (load_palette(exe, state->current, streams, stream_count, step) != 0)
        return -1;

    selected_table_index = (uint16_t)(UINT16_C(1) - bindings->outer_selected_index);
    if (fa18_prepare_outer_page_publication(selected_table_index,
                                            bindings->left_pointer_table,
                                            bindings->right_pointer_table,
                                            bindings->pointer_table_entries,
                                            &step->publication) != 0)
        return -1;
    step->pointer_pair_published = 1;
    ++step->pointer_pair_publish_count;
    /* `$C1718E` invokes LoadRGB4 again after publishing the pointer pair.
     * It supplies the same 16 RGB4 words, but preserving this call ordering
     * matters to a native owner that eventually exposes these operations. */
    if (load_palette(exe, state->current, streams, stream_count, step) != 0)
        return -1;
    if (fa18_prepare_outer_page_publication(selected_table_index,
                                            bindings->left_pointer_table,
                                            bindings->right_pointer_table,
                                            bindings->pointer_table_entries,
                                            &step->publication) != 0)
        return -1;
    ++step->pointer_pair_publish_count;
    if (state->current != state->target) return 0;

    if (fa18_load_viewport_mode_palette(exe, state->current, palette) != 0)
        return -1;
    memcpy(bindings->mode_palette_buffer->words, palette, sizeof palette);
    state->state = 3;
    step->mode_words_copied = 1;
    return 0;
}
