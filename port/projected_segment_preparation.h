#ifndef FA18_PROJECTED_SEGMENT_PREPARATION_H
#define FA18_PROJECTED_SEGMENT_PREPARATION_H

#include "projected_segment_clip.h"
#include "workspace_segment_projection.h"

typedef struct {
    /* `$C45AC6/$C45ACA`, retained by the caller across helper invocations. */
    FA18ViewVertex components;
    int16_t status_word;
} FA18ProjectedSegmentPreparationState;

/* `$C2EE4A-$C2F0C5`: prepare both source triples through the P-code-selected
 * clip helpers and invoke the caller-owned `$C2FA7E` line emitter. */
int fa18_prepare_projected_segment(
    FA18ProjectedSegmentPreparationState *state,
    const FA18ViewVertex endpoints[2], FA18WorkspaceSegmentLineEmitter emitter,
    void *emitter_context);

#endif
