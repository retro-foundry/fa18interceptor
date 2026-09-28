#include "scene_template_cursor_context.h"

#include <assert.h>

int main(void) {
    const uint8_t gate_a[1] = {0};
    const uint8_t gate_b[2] = {0};
    const uint8_t gate_c[3] = {0};
    const FA18SceneSelectorContextState first = {16, -15, 1, 0, 0};
    const FA18ContextSelectorPackState second = {
        0, 0, 0, {0, 0, 0}, 0x40, 0xffc0, 7, 0, 0, 0
    };
    const FA18SceneTemplateCursorContextInput input = {
        &first, &second, 1, 0, 3, -42,
        gate_a, sizeof gate_a, gate_b, sizeof gate_b, gate_c, sizeof gate_c
    };
    FA18TerrainTemplateCursorState state;

    assert(fa18_prepare_scene_template_cursor_context(&input, &state) == 0);
    assert(state.append_enable == 1 && state.route_flag == 1 &&
           !state.alternate_pack && state.selector_a == 7 &&
           state.selector_b == 0 && state.map_selector == 3 &&
           state.guard_long == -42 && state.row_a == 16 && state.group_a == -15 &&
           state.row_b == 64 && state.group_b == -64 &&
           state.gate_a == gate_a && state.gate_a_size == sizeof gate_a &&
           state.gate_b == gate_b && state.gate_b_size == sizeof gate_b &&
           state.gate_c == gate_c && state.gate_c_size == sizeof gate_c);
    assert(fa18_prepare_scene_template_cursor_context(0, &state) == -1);
    assert(fa18_prepare_scene_template_cursor_context(&input, 0) == -1);
    return 0;
}
