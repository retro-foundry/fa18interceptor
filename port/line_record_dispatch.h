#ifndef FA18_LINE_RECORD_DISPATCH_H
#define FA18_LINE_RECORD_DISPATCH_H

#include "offset_pair_segment_submission.h"
#include "projected_segment_preparation.h"
#include "record_walker_runtime.h"

typedef struct {
    const uint8_t *vertex_bytes; /* `$C48390`. */
    size_t vertex_byte_count;
    const uint8_t *stream_bytes;
    size_t stream_byte_count;
    uint32_t stream_base;
    FA18ProjectedSegmentPreparationState *preparation_state;
    FA18WorkspaceSegmentLineEmitter line_emitter; /* `$C2FA7E` owner. */
    void *line_emitter_context;
    FA18OffsetPairSegmentSubmissionResult last_submission;
} FA18LineRecordDispatch;

/* `$C1F942` table slot `$0034 -> $C212B0 -> $C2EE4A -> $C2FA7E`.
 * Any other selector is an error: callers must provide its proven source
 * target rather than silently treating it as a line record. */
int fa18_dispatch_line_record(void *context, uint16_t selector,
                              uint32_t record_cursor, int16_t *source_status);

#endif
