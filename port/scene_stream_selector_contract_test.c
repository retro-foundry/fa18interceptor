#include "scene_stream_selector.h"

#include <assert.h>

int main(void) {
    const uint8_t stream[] = {0,0, 0,0, 0x12,0x34, 0xff,0xff, 0,0};
    FA18SceneStreamSelectorResult result;
    FA18SceneStreamSelectorRoute route;
    assert(fa18_select_scene_stream_threshold(stream, sizeof stream, 2, 0,
                                              0x0010, 0, 8, &result, &route) == 0);
    assert(route == FA18_SCENE_STREAM_SELECTOR_SELECTED && result.selected_word == 0x1234 &&
           result.next_cursor == 6);
    assert(fa18_select_scene_stream_threshold(stream, sizeof stream, 2, 0,
                                              0x4010, 0, 0x20, &result, &route) == 0);
    assert(route == FA18_SCENE_STREAM_SELECTOR_RETRY && result.next_cursor == 6);
    assert(fa18_select_scene_stream_threshold(stream, sizeof stream, 2, 0,
                                              0x0000, 0, 0, &result, &route) == 0);
    assert(route == FA18_SCENE_STREAM_SELECTOR_RETRY && result.next_cursor == 0);
    assert(fa18_select_scene_stream_threshold(stream, sizeof stream, 6, 0,
                                              0x0000, 0, 0, &result, &route) == 0);
    assert(route == FA18_SCENE_STREAM_SELECTOR_SENTINEL);
    return 0;
}
