#include "map_packet_pass_runner.h"

typedef struct {
    const FA18MapPacketPassRunnerInput *input;
    const FA18MapPacketPassSelectorResult *pass;
} ResolverContext;

static int resolve_record(void *context, uint8_t mode,
                          FA18MapPacketRecordStageInput *record) {
    ResolverContext *resolver = context;
    return resolver->input->resolve_record(resolver->input->context,
                                           resolver->pass, mode, record);
}

int fa18_run_map_packet_pass(const FA18MapPacketPassRunnerInput *input,
                             FA18MapPacketProjectionRecord *records,
                             size_t record_capacity, uint16_t *record_count,
                             FA18MapPacketPassSelectorResult *pass,
                             FA18MapPacketControlWalkerRoute *route) {
    if (!input || !input->resolve_control_stream || !input->resolve_record ||
        !records || !record_count || !pass || !route)
        return -1;
    if (fa18_select_map_packet_pass(&input->selector, pass) != 0) return -1;
    const uint8_t *stream;
    size_t stream_size;
    if (input->resolve_control_stream(input->context, pass->control_stream_address,
                                      &stream, &stream_size) != 0)
        return -1;
    ResolverContext resolver = {input, pass};
    const FA18MapPacketControlWalkerInput walker = {
        stream, stream_size, resolve_record, &resolver
    };
    return fa18_walk_map_packet_controls(&walker, records, record_capacity,
                                         record_count, route);
}
