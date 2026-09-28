#include "scene_stream_runtime.h"

#include "hunk.h"

static int16_t low_word(int32_t value) {
    return (int16_t)(uint16_t)value;
}

static int32_t add_long(int32_t left, int32_t right) {
    return (int32_t)((uint32_t)left + (uint32_t)right);
}

static int32_t asr_long(int32_t value, unsigned count) {
    count &= 63u;
    if (!count) return value;
    if (count >= 32u) return value < 0 ? -1 : 0;
    if (value >= 0) return value >> count;
    return (int32_t)-((-(int64_t)value + ((INT64_C(1) << count) - 1)) >> count);
}

static int write_vertex(uint8_t *bytes, size_t size, size_t index,
                        const FA18TransformedVertex *vertex) {
    const size_t offset = index * 6u;
    if (!bytes || !vertex || offset > size || size - offset < 6u) return -1;
    bytes[offset] = (uint8_t)((uint16_t)vertex->x >> 8);
    bytes[offset + 1u] = (uint8_t)vertex->x;
    bytes[offset + 2u] = (uint8_t)((uint16_t)vertex->y >> 8);
    bytes[offset + 3u] = (uint8_t)vertex->y;
    bytes[offset + 4u] = (uint8_t)((uint16_t)vertex->depth >> 8);
    bytes[offset + 5u] = (uint8_t)vertex->depth;
    return 0;
}

int fa18_run_scene_stream_direct_runtime(
    const FA18SceneStreamRuntimeInput *input,
    FA18SceneStreamRuntimeResult *result,
    FA18SceneStreamRuntimeRoute *route) {
    FA18SceneStreamEntryRoute entry_route;
    uint32_t descriptor;
    uint8_t control, count;
    uint8_t high_nibble, low_nibble;
    FA18LocalVertex vertices[UINT8_MAX];
    FA18TransformedVertex output[UINT8_MAX];
    FA18RecordWalkerPrefixInput walker;
    FA18RecordWalkerPrefixRoute walker_route;

    if (!input || !result || !route || !input->entry.stream ||
        !input->workspace.bytes || !input->workspace.byte_count ||
        !input->walker_step_budget)
        return -1;
    if (fa18_enter_scene_stream(&input->entry, &result->entry, &entry_route) != 0)
        return -1;
    if (entry_route == FA18_SCENE_STREAM_ENTRY_RETURN_ZERO) {
        *route = FA18_SCENE_STREAM_RUNTIME_RETURN_ZERO;
        return 0;
    }
    if (entry_route == FA18_SCENE_STREAM_ENTRY_RETURN_ONE) {
        *route = FA18_SCENE_STREAM_RUNTIME_RETURN_ONE;
        return 0;
    }
    descriptor = result->entry.descriptor.descriptor_cursor;
    if (descriptor > input->entry.stream_size ||
        input->entry.stream_size - descriptor < 10u ||
        fa18_decode_scene_stream_descriptor(input->entry.stream + descriptor,
                                            input->entry.stream_size - descriptor,
                                            &result->descriptor) != 0)
        return -1;
    control = input->entry.stream[descriptor + 7u];
    count = input->entry.stream[descriptor + 8u];
    if (!count || input->workspace.byte_count / 6u < count ||
        input->entry.stream_size - descriptor < 10u + (size_t)count * 6u)
        return -1;
    if (control & 1u) {
        *route = FA18_SCENE_STREAM_RUNTIME_UNPORTED_CONTROL_BRANCH;
        return 0;
    }
    high_nibble = result->descriptor.high_nibble;
    low_nibble = result->descriptor.low_nibble;
    if (control & 2u) {
        *route = FA18_SCENE_STREAM_RUNTIME_UNPORTED_CONTROL_BRANCH;
        return 0;
    }
    /* `$C1F464-$C1F498`: the middle/last prepared longs exchange before
     * translation; `A5` receives the low word of the shifted middle value. */
    result->transform.local_shift = (uint8_t)(input->stream_shift + high_nibble);
    {
        const unsigned shift = (unsigned)(uint16_t)(8u -
            (uint16_t)(int16_t)((int16_t)low_nibble - (int16_t)high_nibble));
        result->transform.translation.x = low_word(asr_long(
            add_long(input->prepared_component[0], input->stream_component_x), shift));
        result->transform.translation.y = low_word(asr_long(
            add_long(input->prepared_component[1], input->stream_component_y), shift));
        result->transform.translation.z = low_word(asr_long(
            add_long(input->prepared_component[2], input->stream_component_z), shift));
    }
    result->transform.matrix = input->matrix;
    for (uint16_t index = 0; index < count; ++index) {
        const uint8_t *source = input->entry.stream + descriptor + 10u + (size_t)index * 6u;
        vertices[index] = (FA18LocalVertex){
            (int16_t)fa18_be16(source), (int16_t)fa18_be16(source + 2u),
            (int16_t)fa18_be16(source + 4u)
        };
    }
    if (fa18_transform_vertices(&result->transform, vertices, count, output) != 0)
        return -1;
    for (uint16_t index = 0; index < count; ++index)
        if (write_vertex(input->workspace.bytes, input->workspace.byte_count,
                         index, &output[index]) != 0)
            return -1;
    result->transformed_vertex_count = count;
    walker = (FA18RecordWalkerPrefixInput){
        input->entry.stream, input->entry.stream_size, input->entry.record_base,
        result->entry.descriptor.published_stage_cursor,
        input->workspace.bytes, (size_t)count * 6u, input->walker_step_budget,
        input->triple_handler, input->hex_handler, input->other_handler,
        input->handler_context
    };
    if (fa18_run_record_walker_prefix(&walker, &result->walker, &walker_route) != 0)
        return -1;
    if (walker_route == FA18_RECORD_WALKER_NEGATIVE_CONTROL_EXTERNAL) {
        *route = FA18_SCENE_STREAM_RUNTIME_UNPORTED_CONTROL_BRANCH;
        return 0;
    }
    *route = FA18_SCENE_STREAM_RUNTIME_WALKER_COMPLETE;
    return 0;
}
