#include "map_packet_selector.h"

#include "hunk.h"

static int read_word(const uint8_t *stream, size_t size, int16_t *word) {
    if (!stream || !word || size < 2u) return -1;
    *word = (int16_t)fa18_be16(stream);
    return 0;
}

static int32_t asr_long_8(int32_t value) {
    if (value >= 0) return value >> 8;
    return (int32_t)-((-(int64_t)value + 255) >> 8);
}

static uint32_t rol_long_4(uint32_t value) {
    return (value << 4) | (value >> 28);
}

int fa18_select_map_packet_stream(const FA18MapPacketSelectorInput *input,
                                  FA18MapPacketSelectorResult *result,
                                  FA18MapPacketSelectorRoute *route) {
    if (!input || !result || !route || !input->packet || input->packet_size < 4u)
        return -1;
    const uint32_t alternate_reference = fa18_be32(input->packet);
    if ((int32_t)alternate_reference < 0) {
        *route = FA18_MAP_PACKET_REJECTED;
        return 0;
    }

    uint32_t origin = (uint32_t)input->packet_origin;
    if (input->origin_adjusted) origin += UINT32_C(0x1000);
    origin = (origin << 16) | (origin >> 16);
    if (input->origin_adjusted) origin = rol_long_4(origin);
    const int16_t negated_origin =
        (int16_t)(UINT16_C(0) - (uint16_t)origin);
    for (unsigned index = 0; index != 3; ++index) {
        result->origin_component[index] =
            (int16_t)(uint16_t)asr_long_8((int32_t)negated_origin *
                                           input->origin_matrix[index]);
    }

    const uint8_t *stream;
    size_t stream_size;
    if (input->alternate_stream) {
        if (!input->resolve_stream ||
            input->resolve_stream(input->context, alternate_reference, &stream, &stream_size) != 0)
            return -1;
    } else {
        stream = input->packet + 4u;
        stream_size = input->packet_size - 4u;
    }

    int16_t count;
    if (read_word(stream, stream_size, &count) != 0) return -1;
    stream += 2u;
    stream_size -= 2u;
    if (count <= 0) {
        if (count == -1) {
            *route = FA18_MAP_PACKET_REJECTED;
            return 0;
        }
        uint16_t relative = (uint16_t)count & UINT16_C(0x7fff);
        relative = (uint16_t)(relative + relative);
        relative = (uint16_t)(relative + relative);
        const int32_t scaled_relative = (int32_t)(int16_t)relative;
        if (scaled_relative > input->detail_metric) {
            *route = FA18_MAP_PACKET_REJECTED;
            return 0;
        }
        if (read_word(stream, stream_size, &count) != 0) return -1;
        stream += 2u;
        stream_size -= 2u;
    }
    if (count > 0x12) {
        result->error_code = 0x20;
        *route = FA18_MAP_PACKET_COUNT_ERROR;
        return 0;
    }
    result->pair_stream = stream;
    result->pair_stream_size = stream_size;
    result->pair_count = count;
    result->error_code = 0;
    *route = FA18_MAP_PACKET_READY;
    return 0;
}
