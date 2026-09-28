#ifndef FA18_SCENE_TEMPLATE_CURSOR_CONTEXT_H
#define FA18_SCENE_TEMPLATE_CURSOR_CONTEXT_H

#include "context_selector_pack.h"
#include "scene_selector_context.h"
#include "terrain_template_cursor_resolver.h"

/* Caller-owned source inputs at `$C1D10C`: the two preceding selector packs,
 * route bytes, and the three static-bit-gate ranges selected by its branches. */
typedef struct {
    const FA18SceneSelectorContextState *first_pack;
    const FA18ContextSelectorPackState *second_pack;
    uint8_t route_flag;
    uint8_t alternate_pack;
    uint8_t map_selector;
    int32_t guard_long;
    const uint8_t *gate_a;
    size_t gate_a_size;
    const uint8_t *gate_b;
    size_t gate_b_size;
    const uint8_t *gate_c;
    size_t gate_c_size;
} FA18SceneTemplateCursorContextInput;

/* `$C1D10C-$C1D22C`: select the append or ordinary mutable selector pack and
 * publish the exact state consumed by the existing static cursor resolver. */
int fa18_prepare_scene_template_cursor_context(
    const FA18SceneTemplateCursorContextInput *input,
    FA18TerrainTemplateCursorState *state);

#endif
