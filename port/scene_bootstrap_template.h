#ifndef FA18_SCENE_BOOTSTRAP_TEMPLATE_H
#define FA18_SCENE_BOOTSTRAP_TEMPLATE_H

#include <stdint.h>

#include "hunk.h"

enum {
    FA18_SCENE_BOOTSTRAP_TEMPLATE_HUNK = 16,
    FA18_SCENE_BOOTSTRAP_TEMPLATE_NORMAL_OFFSET = 0x50,
    FA18_SCENE_BOOTSTRAP_TEMPLATE_SELECTOR_10_OFFSET = 0x3c
};

/* `$C092D4-$C09302` selects one five-longword template at `$C22048`.  Its
 * second longword is copied into the runtime parameter pack and immediately
 * dereferenced by `$C0930A`; retain it as a relocatable executable reference
 * instead of an Amiga address. */
typedef struct {
    uint32_t template_offset;
    uint32_t descriptor_segment;
    uint32_t descriptor_offset;
} FA18SceneBootstrapTemplate;

int fa18_load_scene_bootstrap_template(const FA18Hunks *hunks,
                                       uint8_t source_selector,
                                       FA18SceneBootstrapTemplate *template_state);

#endif
