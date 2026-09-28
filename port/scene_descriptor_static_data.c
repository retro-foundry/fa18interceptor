#include "scene_descriptor_static_data.h"

int fa18_load_scene_descriptor_static_data(const FA18Hunks *hunks,
                                           FA18SceneDescriptorStaticData *data) {
    const FA18HunkSegment *segment;
    if (!hunks || !data || hunks->count <= FA18_SCENE_DESCRIPTOR_HUNK)
        return -1;
    segment = &hunks->segments[FA18_SCENE_DESCRIPTOR_HUNK];
    if (!segment->data || !segment->size) return -1;
    *data = (FA18SceneDescriptorStaticData){hunks, segment->data, segment->size};
    return 0;
}

int fa18_resolve_scene_descriptor_pointers(
    const FA18SceneDescriptorStaticData *data, uint32_t runtime_address,
    FA18SceneDescriptorPointers *pointers) {
    uint32_t offset;
    if (!data || !data->hunks || !data->bytes || !pointers ||
        runtime_address < FA18_SCENE_DESCRIPTOR_RUNTIME_BASE)
        return -1;
    offset = runtime_address - FA18_SCENE_DESCRIPTOR_RUNTIME_BASE;
    if (offset > data->size || data->size - offset <
                                FA18_SCENE_DESCRIPTOR_POINTER_COUNT * 4u)
        return -1;
    for (unsigned index = 0; index != FA18_SCENE_DESCRIPTOR_POINTER_COUNT; ++index)
        if (!fa18_hunk_pointer(data->hunks, FA18_SCENE_DESCRIPTOR_HUNK,
                               offset + index * 4u,
                               &pointers->target_hunk[index],
                               &pointers->target_offset[index]))
            return -1;
    return 0;
}

int fa18_lookup_scene_descriptor_static(void *context, uint32_t runtime_address,
                                        FA18ScenePlacementDescriptorProbe *probe) {
    FA18SceneDescriptorStaticData *data = context;
    FA18SceneDescriptorPointers pointers;
    uint32_t offset;
    if (!data || !probe || runtime_address < FA18_SCENE_DESCRIPTOR_RUNTIME_BASE ||
        fa18_resolve_scene_descriptor_pointers(data, runtime_address, &pointers) != 0)
        return -1;
    offset = runtime_address - FA18_SCENE_DESCRIPTOR_RUNTIME_BASE;
    probe->first_word = (int16_t)fa18_be16(data->bytes + offset);
    probe->first_target_hunk = pointers.target_hunk[0];
    probe->first_target_offset = pointers.target_offset[0];
    return 0;
}

int fa18_resolve_scene_descriptor_target(const FA18SceneDescriptorStaticData *data,
                                         uint32_t target_hunk,
                                         uint32_t target_offset,
                                         const uint8_t **bytes, size_t *size) {
    const FA18HunkSegment *segment;
    if (!data || !data->hunks || !bytes || !size || target_hunk >= data->hunks->count)
        return -1;
    segment = &data->hunks->segments[target_hunk];
    if (!segment->data || target_offset >= segment->size) return -1;
    *bytes = segment->data + target_offset;
    *size = segment->size - target_offset;
    return 0;
}
