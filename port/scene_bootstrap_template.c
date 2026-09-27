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
