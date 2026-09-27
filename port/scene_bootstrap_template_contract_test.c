#include "scene_bootstrap_template.h"

#include <assert.h>

int main(void) {
    uint8_t templates[0x100] = { 0 };
    FA18HunkReloc relocs[] = {
        {FA18_SCENE_BOOTSTRAP_TEMPLATE_SELECTOR_10_OFFSET + 4u, 4},
        {FA18_SCENE_BOOTSTRAP_TEMPLATE_NORMAL_OFFSET + 4u, 3}
    };
    uint8_t descriptors[0x1100] = { 0 };
    FA18HunkSegment segments[17] = { 0 };
    FA18Hunks hunks = {segments, 17};
    FA18SceneBootstrapTemplate template_state;

    segments[FA18_SCENE_BOOTSTRAP_TEMPLATE_HUNK] = (FA18HunkSegment){
        FA18_HUNK_CODE, templates, sizeof templates, relocs, 2
    };
    segments[3] = (FA18HunkSegment){.data = descriptors, .size = sizeof descriptors};
    segments[4] = (FA18HunkSegment){.data = descriptors, .size = sizeof descriptors};
    templates[FA18_SCENE_BOOTSTRAP_TEMPLATE_NORMAL_OFFSET + 4u] = 0;
    templates[FA18_SCENE_BOOTSTRAP_TEMPLATE_NORMAL_OFFSET + 5u] = 0x34;
    templates[FA18_SCENE_BOOTSTRAP_TEMPLATE_NORMAL_OFFSET + 6u] = 0x56;
    templates[FA18_SCENE_BOOTSTRAP_TEMPLATE_NORMAL_OFFSET + 7u] = 0x78;
    templates[FA18_SCENE_BOOTSTRAP_TEMPLATE_SELECTOR_10_OFFSET + 4u] = 0;
    templates[FA18_SCENE_BOOTSTRAP_TEMPLATE_SELECTOR_10_OFFSET + 5u] = 0x9a;
    templates[FA18_SCENE_BOOTSTRAP_TEMPLATE_SELECTOR_10_OFFSET + 6u] = 0xbc;
    templates[FA18_SCENE_BOOTSTRAP_TEMPLATE_SELECTOR_10_OFFSET + 7u] = 0xde;

    assert(fa18_load_scene_bootstrap_template(&hunks, 0, &template_state) == 0);
    assert(template_state.template_offset == 0x50 &&
           template_state.descriptor_segment == 3 &&
           template_state.descriptor_offset == 0x345678u);
    template_state.descriptor_offset = 0;
    descriptors[0] = 0x80; descriptors[1] = 0x0e;
    descriptors[6 + 0x0e] = 0x3b;
    assert(fa18_decode_scene_bootstrap_class(&hunks, &template_state,
                                             &templates[0]) == 0 &&
           templates[0] == 0x0b);
    descriptors[0] = 0x40; descriptors[1] = 0;
    descriptors[2] = 0x02; descriptors[3] = 0;
    descriptors[6 + 0x200] = 0x09;
    assert(fa18_decode_scene_bootstrap_class(&hunks, &template_state,
                                             &templates[0]) == 0 &&
           templates[0] == 9);
    descriptors[0] = 0; descriptors[1] = 1;
    descriptors[4] = 0x01; descriptors[5] = 0x00;
    descriptors[6 + 0x100] = 0x2e;
    assert(fa18_decode_scene_bootstrap_class(&hunks, &template_state,
                                             &templates[0]) == 0 &&
           templates[0] == 0x0e);
    assert(fa18_load_scene_bootstrap_template(&hunks, 0x10, &template_state) == 0);
    assert(template_state.template_offset == 0x3c &&
           template_state.descriptor_segment == 4 &&
           template_state.descriptor_offset == 0x9abcdeu);
    segments[FA18_SCENE_BOOTSTRAP_TEMPLATE_HUNK].reloc_count = 0;
    assert(fa18_load_scene_bootstrap_template(&hunks, 0, &template_state) == -1);
    return 0;
}
