#include "scene_template_cursor_context.h"

int fa18_prepare_scene_template_cursor_context(
    const FA18SceneTemplateCursorContextInput *input,
    FA18TerrainTemplateCursorState *state) {
    if (!input || !state || !input->first_pack || !input->second_pack ||
        !input->gate_a || !input->gate_b || !input->gate_c ||
        !input->gate_a_size || !input->gate_b_size || !input->gate_c_size)
        return -1;
    *state = (FA18TerrainTemplateCursorState){
        input->first_pack->append_enabled,
        input->route_flag,
        input->alternate_pack,
        input->second_pack->selector_byte_x,
        input->second_pack->selector_byte_y,
        input->map_selector,
        input->guard_long,
        input->first_pack->first_selector,
        input->first_pack->second_selector,
        (int16_t)input->second_pack->selector_word_x,
        (int16_t)input->second_pack->selector_word_y,
        input->gate_a, input->gate_b, input->gate_c,
        input->gate_a_size, input->gate_b_size, input->gate_c_size
    };
    return 0;
}
