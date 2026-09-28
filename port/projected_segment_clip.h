#ifndef FA18_PROJECTED_SEGMENT_CLIP_H
#define FA18_PROJECTED_SEGMENT_CLIP_H

#include "projection.h"

typedef enum {
    FA18_PROJECTED_SEGMENT_CLIP_D3_FIRST_C2F0C6,
    FA18_PROJECTED_SEGMENT_CLIP_D3_SECOND_C2F0F4,
    FA18_PROJECTED_SEGMENT_CLIP_D4_FIRST_C2F128,
    FA18_PROJECTED_SEGMENT_CLIP_D4_SECOND_C2F156
} FA18ProjectedSegmentClipEntry;

/* `$C2F0C6-$C2F1B6`: execute one source-selected interpolation helper over
 * the two `$C4C592` triples. `candidate` is the helper's `$C45AC6` write;
 * `outside` is its D0 result (zero for the in-bounds result-gate path). */
int fa18_clip_projected_segment(const FA18ViewVertex endpoints[2],
                                FA18ProjectedSegmentClipEntry entry,
                                FA18ViewVertex *candidate, int *outside);

#endif
