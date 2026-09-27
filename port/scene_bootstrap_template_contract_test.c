#include "scene_bootstrap_template.h"

#include <assert.h>

int main(void) {
    uint8_t templates[0x100] = { 0 };
    FA18HunkReloc relocs[] = {
        {FA18_SCENE_BOOTSTRAP_TEMPLATE_SELECTOR_10_OFFSET + 4u, 4},
        {FA18_SCENE_BOOTSTRAP_TEMPLATE_NORMAL_OFFSET + 4u, 3}
    };
    FA18HunkSegment segments[17] = { 0 };
    FA18Hunks hunks = {segments, 17};
    FA18SceneBootstrapTemplate template_state;

    segments[FA18_SCENE_BOOTSTRAP_TEMPLATE_HUNK] = (FA18HunkSegment){
        FA18_HUNK_CODE, templates, sizeof templates, relocs, 2
    };
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
    assert(fa18_load_scene_bootstrap_template(&hunks, 0x10, &template_state) == 0);
    assert(template_state.template_offset == 0x3c &&
           template_state.descriptor_segment == 4 &&
           template_state.descriptor_offset == 0x9abcdeu);
    segments[FA18_SCENE_BOOTSTRAP_TEMPLATE_HUNK].reloc_count = 0;
    assert(fa18_load_scene_bootstrap_template(&hunks, 0, &template_state) == -1);
    return 0;
}
