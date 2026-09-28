#include "line_record_dispatch.h"

static int prepare_line(void *context, const FA18ViewVertex endpoints[2]) {
    FA18LineRecordDispatch *dispatch = context;
    return fa18_prepare_projected_segment(dispatch->preparation_state, endpoints,
                                          dispatch->line_emitter,
                                          dispatch->line_emitter_context);
}

int fa18_dispatch_line_record(void *context, uint16_t selector,
                              uint32_t record_cursor, int16_t *source_status) {
    FA18LineRecordDispatch *dispatch = context;
    FA18OffsetPairSegmentStreamInput input;

    if (!dispatch || !source_status || selector != 0x0034 ||
        !dispatch->preparation_state || !dispatch->line_emitter)
        return -1;
    input = (FA18OffsetPairSegmentStreamInput){
        dispatch->vertex_bytes, dispatch->vertex_byte_count,
        dispatch->stream_bytes, dispatch->stream_byte_count,
        dispatch->stream_base, record_cursor, prepare_line, dispatch
    };
    if (fa18_submit_offset_pair_segment_stream(&input, &dispatch->last_submission) != 0)
        return -1;
    *source_status = (int16_t)dispatch->last_submission.result_flags;
    return 0;
}
