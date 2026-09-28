#ifndef FA18_PROJECTION_PAGE_BLITTER_H
#define FA18_PROJECTION_PAGE_BLITTER_H

#include "blit_job.h"
#include "five_plane_chip_binding.h"
#include "projection_grid.h"

/* Typed page-side owner for the direct `$C30668 -> $C30404` submission tail.
 * `$C2FF58` is a distinct caller path, represented separately by the stored
 * lane state. The parent traversal supplies every inherited register and
 * lane-state value; this adapter only performs the source-proved mutations
 * and synchronizes the caller-owned five-plane page at each hardware boundary. */
typedef struct {
    FA18FivePlanePage *page;
    FA18FivePlaneChipBinding binding;
    FA18BlitOperation operation;
    FA18RendererLaneStage lanes;
    uint16_t line_submissions;
    uint8_t initialized;
} FA18ProjectionPageBlitter;

int fa18_projection_page_blitter_init(
    FA18ProjectionPageBlitter *blitter, FA18FivePlanePage *page,
    const FA18FivePlaneChipBinding *binding,
    const FA18BlitOperation *inherited_operation,
    const FA18RendererLaneStage *lane_stage);

/* Callback for the exact partial `$C30668` register packet. */
int fa18_projection_page_emit_blitter(
    void *context, const FA18ProjectionPairBlitterWrites *writes);

/* Callback for the direct `$C303EC-$C30404` descending-fill tail. */
int fa18_projection_page_emit_final(
    void *context, const FA18ProjectionPairFinalState *state);

#endif
