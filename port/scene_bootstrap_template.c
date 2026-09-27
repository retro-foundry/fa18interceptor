#include "scene_bootstrap_template.h"

int fa18_load_scene_bootstrap_template(const FA18Hunks *hunks,
                                       uint8_t source_selector,
                                       FA18SceneBootstrapTemplate *template_state) {
    uint32_t offset;
    uint32_t descriptor_segment;
    uint32_t descriptor_offset;

    if (!hunks || !template_state ||
        FA18_SCENE_BOOTSTRAP_TEMPLATE_HUNK >= hunks->count)
        return -1;
    offset = source_selector == 0x10u
                 ? FA18_SCENE_BOOTSTRAP_TEMPLATE_SELECTOR_10_OFFSET
                 : FA18_SCENE_BOOTSTRAP_TEMPLATE_NORMAL_OFFSET;
    if (!fa18_hunk_pointer(hunks, FA18_SCENE_BOOTSTRAP_TEMPLATE_HUNK,
                           offset + 4u, &descriptor_segment, &descriptor_offset))
        return -1;
    *template_state = (FA18SceneBootstrapTemplate){
        offset, descriptor_segment, descriptor_offset
    };
    return 0;
}

int fa18_decode_scene_bootstrap_class(const FA18Hunks *hunks,
                                      const FA18SceneBootstrapTemplate *template_state,
                                      uint8_t *class_nibble) {
    const FA18HunkSegment *segment;
    const uint8_t *descriptor;
    int16_t selector;
    uint16_t table_offset;

    if (!hunks || !template_state || !class_nibble ||
        template_state->descriptor_segment >= hunks->count)
        return -1;
    segment = &hunks->segments[template_state->descriptor_segment];
    if (!segment->data || template_state->descriptor_offset > segment->size ||
        segment->size - template_state->descriptor_offset < 6u)
        return -1;
    descriptor = segment->data + template_state->descriptor_offset;
    selector = (int16_t)fa18_be16(descriptor);
    if (selector < 0) {
        table_offset = (uint16_t)selector & 0x0fffu;
    } else if (selector & 0x4000) {
        table_offset = fa18_be16(descriptor + 2u) & 0x0fffu;
    } else {
        table_offset = fa18_be16(descriptor + 4u) & 0x0fffu;
    }
    if ((uint32_t)table_offset + 6u >= segment->size - template_state->descriptor_offset)
        return -1;
    *class_nibble = descriptor[6u + table_offset] & 0x0fu;
    return 0;
}
