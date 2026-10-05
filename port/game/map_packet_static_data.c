#include "map_packet_static_data.h"

static int resolve(const uint8_t *bytes, size_t byte_count, uint32_t base,
                   uint32_t address, const uint8_t **result, size_t *size) {
    if (!bytes || !result || !size || address < base) return -1;
    const size_t offset = (size_t)(address - base);
    if (offset >= byte_count) return -1;
    *result = bytes + offset;
    *size = byte_count - offset;
    return 0;
}

int fa18_load_map_packet_static_data(const FA18Hunks *hunks,
                                     FA18MapPacketStaticData *data) {
    const FA18HunkSegment *control;
    const FA18HunkSegment *packet;
    if (!hunks || !data || hunks->count <= FA18_MAP_PACKET_PACKET_HUNK)
        return -1;
    control = &hunks->segments[FA18_MAP_PACKET_CONTROL_HUNK];
    packet = &hunks->segments[FA18_MAP_PACKET_PACKET_HUNK];
    if (!control->data || !control->size || !packet->data || !packet->size)
        return -1;
    *data = (FA18MapPacketStaticData){control->data, control->size,
                                      packet->data, packet->size};
    return 0;
}

int fa18_resolve_map_packet_control_pair(void *context, uint8_t mode,
                                         int8_t pair[2]) {
    FA18MapPacketStaticData *data = context;
    const size_t offset = (size_t)mode * 2u;
    if (!data || !pair || !data->control_bytes || offset + 2u > data->control_size)
        return -1;
    pair[0] = (int8_t)data->control_bytes[offset];
    pair[1] = (int8_t)data->control_bytes[offset + 1u];
    return 0;
}

int fa18_resolve_map_packet_control_stream(void *context, uint32_t address,
                                           const uint8_t **stream, size_t *size) {
    FA18MapPacketStaticData *data = context;
    return !data ? -1 : resolve(data->control_bytes, data->control_size,
                                FA18_MAP_PACKET_CONTROL_RUNTIME_BASE,
                                address, stream, size);
}

int fa18_resolve_map_packet_low_filter_row(void *context, uint32_t address,
                                           int16_t selector, int8_t row[4]) {
    FA18MapPacketStaticData *data = context;
    const uint8_t *source;
    size_t size;
    const int32_t offset = (int32_t)selector * 4;
    if (!data || !row || offset < 0 ||
        resolve(data->control_bytes, data->control_size,
                FA18_MAP_PACKET_CONTROL_RUNTIME_BASE, address,
                &source, &size) != 0 || (size_t)offset > size ||
        size - (size_t)offset < 4u)
        return -1;
    for (unsigned index = 0; index != 4; ++index)
        row[index] = (int8_t)source[(size_t)offset + index];
    return 0;
}

int fa18_resolve_map_packet_static_packet(void *context, uint32_t address,
                                          const uint8_t **packet, size_t *size) {
    FA18MapPacketStaticData *data = context;
    return !data ? -1 : resolve(data->packet_bytes, data->packet_size,
                                FA18_MAP_PACKET_PACKET_RUNTIME_BASE,
                                address, packet, size);
}
