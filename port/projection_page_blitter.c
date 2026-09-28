#include "projection_page_blitter.h"

#include <string.h>

static int sync_page_to_chip(FA18ProjectionPageBlitter *blitter) {
    return fa18_five_plane_chip_binding_store_page(&blitter->binding, blitter->page);
}

static int sync_chip_to_page(FA18ProjectionPageBlitter *blitter) {
    return fa18_five_plane_chip_binding_load_page(&blitter->binding, blitter->page);
}

int fa18_projection_page_blitter_init(
    FA18ProjectionPageBlitter *blitter, FA18FivePlanePage *page,
    const FA18FivePlaneChipBinding *binding,
    const FA18BlitOperation *inherited_operation,
    const FA18RendererLaneStage *lane_stage) {
    uint32_t lane_pointers[4];

    if (!blitter || !page || !binding || !inherited_operation || !lane_stage ||
        fa18_five_plane_chip_binding_renderer_lane_pointers(binding, lane_pointers) != 0)
        return -1;
    for (unsigned lane = 0; lane < 4; ++lane)
        if (lane_stage->plane_pointers[lane] != lane_pointers[lane]) return -1;

    *blitter = (FA18ProjectionPageBlitter){
        page, *binding, *inherited_operation, *lane_stage, 0, 1
    };
    return 0;
}

int fa18_projection_page_emit_blitter(
    void *context, const FA18ProjectionPairBlitterWrites *writes) {
    FA18ProjectionPageBlitter *blitter = context;
    FA18BlitOperation *operation;

    if (!blitter || !blitter->initialized || !writes) return -1;
    operation = &blitter->operation;
    if (sync_page_to_chip(blitter) != 0) return -1;
    operation->bltcon0 = writes->bltcon0;
    operation->bltcon1 = writes->bltcon1;
    operation->bltafwm = writes->bltafwm;
    operation->bltadat = writes->bltadat;
    operation->bltbdat = writes->bltbdat;
    operation->bltamod = writes->bltamod;
    operation->bltbmod = writes->bltbmod;
    operation->bltcmod = writes->bltcmod;
    operation->bltdmod = writes->bltdmod;
    operation->bltapt = (operation->bltapt & UINT32_C(0xffff0000)) |
                        writes->bltapt_low;
    operation->bltcpt = writes->bltcpt;
    operation->bltdpt = writes->bltdpt;
    operation->bltsize = writes->bltsize;
    if (fa18_execute_ocs_line_blit(operation, blitter->binding.chip_bytes,
                                   blitter->binding.chip_byte_count) != 0 ||
        sync_chip_to_page(blitter) != 0)
        return -1;
    ++blitter->line_submissions;
    return 0;
}

int fa18_projection_page_emit_final(
    void *context, const FA18ProjectionPairFinalState *state) {
    FA18ProjectionPageBlitter *blitter = context;
    FA18BlitOperation *operation;

    if (!blitter || !blitter->initialized || !state) return -1;
    operation = &blitter->operation;
    if (sync_page_to_chip(blitter) != 0) return -1;
    /* `$C303EC-$C30404`: retain the final line's C-data/modulos, replace the
     * source-selected A/C/D values, and submit descending fill state. */
    operation->bltcon0 = state->bltcon0;
    operation->bltcon1 = state->bltcon1;
    operation->bltadat = state->bltadat;
    operation->bltbdat = state->bltbdat;
    operation->bltcdat = state->bltcdat;
    operation->bltapt = state->bltapt;
    operation->bltcpt = state->bltcpt;
    operation->bltdpt = state->bltdpt;
    operation->bltsize = state->blit_size;
    /* `$C303EC-$C3040A` returns directly to its `$C301F6` caller.  The
     * `$C2FF58` renderer-lane stage is a separate source path and must not
     * be inferred from this final fill. */
    if (fa18_execute_ocs_block_blit(operation, blitter->binding.chip_bytes,
                                    blitter->binding.chip_byte_count) != 0 ||
        sync_chip_to_page(blitter) != 0)
        return -1;
    return 0;
}
