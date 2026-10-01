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
    if (input->publish_origin)
        input->publish_origin(input->display_context, selection.origin_component);
    FA18MapPacketTransform transform = input->transform;
    for (unsigned index = 0; index != 3; ++index)
        transform.output_base[index] = selection.origin_component[index];
    const uint8_t *stream = selection.pair_stream;
    size_t remaining = selection.pair_stream_size;
    int16_t count = selection.pair_count;
    for (;;) {
        FA18MapPacketPair pairs[0x12];
        size_t bytes;
        if (count <= 0 || (size_t)count > record_capacity || count > 0x12)
            return -1;
        bytes = (size_t)count * 4u;
        if (remaining < bytes) return -1;
        for (uint16_t index = 0; index != (uint16_t)count; ++index) {
            pairs[index].value[0] = (int16_t)fa18_be16(stream + index * 4u);
            pairs[index].value[1] = (int16_t)fa18_be16(stream + index * 4u + 2u);
        }
        if (fa18_transform_map_packet_pairs(&transform, pairs, (uint16_t)count,
                                            count, records, record_capacity,
                                            record_count) != 0 ||
            input->display_stage(input->display_context, records, *record_count,
                                 input->workspace_shift) != 0)
            return -1;
        stream += bytes;
        remaining -= bytes;
        if (remaining < 2u) return -1;
        count = (int16_t)fa18_be16(stream);
        stream += 2;
        remaining -= 2;
        if (count == -1) break;
        if (count <= 0) {
            uint16_t relative = ((uint16_t)count & 0x7fffu);
            relative = (uint16_t)(relative << 2);
            if ((int32_t)(int16_t)relative > input->selector.detail_metric)
                break;
            if (remaining < 2u) return -1;
            count = (int16_t)fa18_be16(stream);
            stream += 2;
            remaining -= 2;
        }
        if (count > 0x12) {
            *record_count = 0;
            *route = FA18_MAP_PACKET_STAGE_COUNT_ERROR;
            return 0;
        }
    }
    *route = FA18_MAP_PACKET_STAGE_DISPLAYED;
    return 0;
}
