#include "scene_stream_entry.h"

#include <assert.h>

int main(void) {
    const uint8_t selected_stream[] = {0x00, 0x10, 0, 0, 0x12, 0x34};
    FA18SceneStreamEntryInput input = {
        selected_stream, sizeof selected_stream, 0, 0, 0x1000,
        8, 0, 1, 0x80, 1, 0, 0, 0, 0, 0
    };
    FA18SceneStreamEntryResult result;
    FA18SceneStreamEntryRoute route;

    assert(fa18_enter_scene_stream(&input, &result, &route) == 0);
    assert(route == FA18_SCENE_STREAM_ENTRY_DESCRIPTOR_READY &&
           result.effective_limit == 8 && result.selected_word == 0x1234 &&
           result.descriptor.descriptor_cursor == 0x1234 &&
           result.descriptor.published_stage_cursor == 6);

    const uint8_t terminal_stream[] = {0xff, 0xff};
    input.control_stream = terminal_stream;
    input.control_stream_size = sizeof terminal_stream;
    assert(fa18_enter_scene_stream(&input, &result, &route) == 0);
    assert(route == FA18_SCENE_STREAM_ENTRY_RETURN_ZERO);

    const uint8_t active_stream[] = {0x20, 0x01, 0, 0, 0x20, 0x01};
    input.control_stream = active_stream;
    input.control_stream_size = sizeof active_stream;
    input.activity_flag = 1;
    assert(fa18_enter_scene_stream(&input, &result, &route) == 0);
    assert(route == FA18_SCENE_STREAM_ENTRY_RETURN_ONE);

    input.control_stream = selected_stream;
    input.control_stream_size = sizeof selected_stream;
    input.descriptor_gate_flag = 1; input.descriptor_gate_word = 0;
    assert(fa18_enter_scene_stream(&input, &result, &route) == 0);
    assert(route == FA18_SCENE_STREAM_ENTRY_RETURN_ZERO);

    const uint8_t rows[24] = {0x12, 0x00};
    input.descriptor_gate_flag = 0; input.depth_scale_word = 0;
    input.activity_flag = 0; input.activity_selector_rows = rows;
    input.activity_selector_rows_size = sizeof rows; input.activity_selector_index = 0x12;
    assert(fa18_enter_scene_stream(&input, &result, &route) == 0);
    assert(route == FA18_SCENE_STREAM_ENTRY_RETURN_ONE);
    assert(fa18_enter_scene_stream(0, &result, &route) == -1);
    return 0;
}
