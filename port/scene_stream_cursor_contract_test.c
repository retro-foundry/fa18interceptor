#include "scene_stream_cursor.h"

#include <assert.h>

int main(void) {
    FA18SceneStreamCursorState state = {0x10, 0x20, 0, 0};
    FA18SceneStreamRefreshRoute refresh_route;
    assert(fa18_refresh_scene_stream_cursor(&state, &refresh_route) == 0);
    assert(state.cursor == 0x20 && refresh_route == FA18_SCENE_STREAM_REFRESH_READ_WORD);
    state.auxiliary_cursor = state.cursor;
    assert(fa18_refresh_scene_stream_cursor(&state, &refresh_route) == 0);
    assert(refresh_route == FA18_SCENE_STREAM_REFRESH_INDIRECT_STAGE);
    state.auxiliary_cursor = 0x30; state.refresh_latch = 1;
    assert(fa18_refresh_scene_stream_cursor(&state, &refresh_route) == 0);
    assert(state.cursor == 0x20 && refresh_route == FA18_SCENE_STREAM_REFRESH_INDIRECT_STAGE);
    state.refresh_latch = 0; state.flight_update_mode = 1;
    assert(fa18_refresh_scene_stream_cursor(&state, &refresh_route) == 0);
    assert(state.cursor == 0x30 && refresh_route == FA18_SCENE_STREAM_REFRESH_C1EDAC_BOUNDARY);

    const uint8_t stream[] = {0x12, 0x34, 0xff, 0xff};
    int16_t word;
    FA18SceneStreamWordRoute word_route;
    assert(fa18_read_scene_stream_word(stream, sizeof stream, 0, &word, &word_route) == 0);
    assert(word == 0x1234 && word_route == FA18_SCENE_STREAM_WORD_NONNEGATIVE);
    assert(fa18_read_scene_stream_word(stream, sizeof stream, 2, &word, &word_route) == 0);
    assert(word == -1 && word_route == FA18_SCENE_STREAM_WORD_NEGATIVE);
    assert(fa18_read_scene_stream_word(stream, sizeof stream, 3, &word, &word_route) == -1);

    FA18SceneStreamAdvanceRoute advance_route;
    assert(fa18_advance_scene_stream_cursor(&state, -1, &advance_route) == 0);
    assert(state.cursor == 0x30 && advance_route == FA18_SCENE_STREAM_ADVANCE_C1EDE4_PREDECESSOR);
    assert(fa18_advance_scene_stream_cursor(&state, -2, &advance_route) == 0);
    assert(state.cursor == 0x34 && advance_route == FA18_SCENE_STREAM_ADVANCE_INDIRECT_STAGE);
    return 0;
}
