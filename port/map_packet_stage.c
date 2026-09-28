#include "map_packet_stage.h"

#include "hunk.h"

int fa18_run_map_packet_stage(const FA18MapPacketStageInput *input,
                              FA18MapPacketProjectionRecord *records,
                              size_t record_capacity,
                              uint16_t *record_count,
                              FA18MapPacketStageRoute *route) {
    FA18MapPacketSelectorResult selection;
    FA18MapPacketSelectorRoute selector_route;
    if (!input || !records || !record_count || !route || !input->display_stage)
        return -1;
    if (fa18_select_map_packet_stream(&input->selector, &selection,
                                      &selector_route) != 0)
        return -1;
    if (selector_route == FA18_MAP_PACKET_REJECTED) {
        *record_count = 0;
        *route = FA18_MAP_PACKET_STAGE_REJECTED;
        return 0;
    }
    if (selector_route == FA18_MAP_PACKET_COUNT_ERROR) {
        *record_count = 0;
        *route = FA18_MAP_PACKET_STAGE_COUNT_ERROR;
        return 0;
    }
    if (selection.pair_count <= 0 || (size_t)selection.pair_count > record_capacity ||
        selection.pair_stream_size < (size_t)selection.pair_count * 4u)
        return -1;

    FA18MapPacketPair pairs[0x12];
    for (uint16_t index = 0; index != (uint16_t)selection.pair_count; ++index) {
        pairs[index].value[0] = (int16_t)fa18_be16(selection.pair_stream + index * 4u);
        pairs[index].value[1] = (int16_t)fa18_be16(selection.pair_stream + index * 4u + 2u);
    }
    FA18MapPacketTransform transform = input->transform;
    for (unsigned index = 0; index != 3; ++index)
        transform.output_base[index] = selection.origin_component[index];
    if (fa18_transform_map_packet_pairs(&transform, pairs, (uint16_t)selection.pair_count,
                                        selection.pair_count, records, record_capacity,
                                        record_count) != 0 ||
        input->display_stage(input->display_context, records, *record_count,
                             input->workspace_shift) != 0)
        return -1;
    *route = FA18_MAP_PACKET_STAGE_DISPLAYED;
    return 0;
}
