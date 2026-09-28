#include "map_packet_parent_pass.h"

static int run_pass(const FA18MapPacketOriginalPassInput *source,
                    FA18MapPacketDirectoryLayout layout, int32_t metric,
                    FA18MapPacketProjectionRecord *records, size_t capacity,
                    uint16_t *count, FA18MapPacketPassSelectorResult *pass,
                    FA18MapPacketControlWalkerRoute *route) {
    FA18MapPacketOriginalPassInput input = *source;
    input.selector.layout = layout;
    input.selector.metric = metric;
    input.record.gate.alternate_layout = layout == FA18_MAP_PACKET_DIRECTORY_WIDE;
    input.record.gate.metric = metric;
    return fa18_run_original_map_packet_pass(&input, records, capacity, count,
                                             pass, route);
}

int fa18_run_map_packet_parent_pass(
    const FA18MapPacketParentPassInput *input,
    FA18MapPacketProjectionRecord *records, size_t record_capacity,
    FA18MapPacketParentPassResult *result) {
    if (!input || !records || !result ||
        fa18_prepare_map_packet_depth_stage(&input->depth, &result->depth) != 0)
        return -1;
    result->normal_record_count = 0;
    if (result->depth.run_normal_pass &&
        run_pass(&input->pass, FA18_MAP_PACKET_DIRECTORY_NORMAL,
                 result->depth.metric, records, record_capacity,
                 &result->normal_record_count, &result->normal_pass,
                 &result->normal_route) != 0)
        return -1;
    if (!result->depth.run_wide_pass ||
        run_pass(&input->pass, FA18_MAP_PACKET_DIRECTORY_WIDE,
                 result->depth.metric, records, record_capacity,
                 &result->wide_record_count, &result->wide_pass,
                 &result->wide_route) != 0)
        return -1;
    return 0;
}
