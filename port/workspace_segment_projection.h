#ifndef FA18_WORKSPACE_SEGMENT_PROJECTION_H
#define FA18_WORKSPACE_SEGMENT_PROJECTION_H

#include <stdint.h>

#include "projection.h"

typedef int (*FA18WorkspaceSegmentLineEmitter)(void *context,
                                               int16_t x0, int16_t y0,
                                               int16_t x1, int16_t y1);

/* `$C2ED6C-$C2EE42`: project the two triples in the source's six-word
 * `$C4C592` workspace and transfer the resulting endpoints to the direct
 * `$C2FA7E` line-emitter owner.  It returns 1 after that transfer, 0 for one
 * of the source depth/bounds rejections, and -1 for invalid native inputs or
 * an emitter failure.  This leaf owns neither workspace scheduling nor line
 * style/page state. */
int fa18_project_workspace_segment_to_line(
    const FA18ViewVertex endpoints[2], FA18WorkspaceSegmentLineEmitter emitter,
    void *emitter_context);

#endif
