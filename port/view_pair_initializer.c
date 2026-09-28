#include "view_pair_initializer.h"

int fa18_initialize_view_pair_slot(FA18ViewPairInitializerState *state,
                                   uint16_t slot, uint32_t raster_source,
                                   const FA18ViewPairInitializerOps *ops) {
    uint32_t display_instruction;
    uint32_t view;

    if (!state || !ops || !ops->build_display_instructions || !ops->build_view ||
        slot > 1)
        return -1;
    state->active_raster_source = raster_source;
    /* `$C160D6-$C160E2` clears the live pair before constructing slot one. */
    if (slot != 0) state->live_pair = (FA18NativeViewPair){0, 0};
    if (ops->build_display_instructions(ops->context, raster_source,
                                        &display_instruction) != 0)
        return -1;
    state->live_pair.display_instruction_pointer = display_instruction;
    if (ops->build_view(ops->context, raster_source, display_instruction,
                        &view) != 0)
        return -1;
    state->live_pair.view_pointer = view;
    state->pair[slot] = state->live_pair;
    return 0;
}
