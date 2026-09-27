#include "scene_stream_cursor.h"

#include "hunk.h"

int fa18_refresh_scene_stream_cursor(FA18SceneStreamCursorState *state,
                                     FA18SceneStreamRefreshRoute *route) {
    if (!state || !route) return -1;
    if (state->auxiliary_cursor == state->cursor || state->refresh_latch) {
        *route = FA18_SCENE_STREAM_REFRESH_INDIRECT_STAGE;
    } else {
        state->cursor = state->auxiliary_cursor;
        *route = state->flight_update_mode ? FA18_SCENE_STREAM_REFRESH_C1EDAC_BOUNDARY
                                            : FA18_SCENE_STREAM_REFRESH_READ_WORD;
    }
    return 0;
}

int fa18_read_scene_stream_word(const uint8_t *stream, size_t stream_size,
                                uint32_t cursor, int16_t *word,
                                FA18SceneStreamWordRoute *route) {
    if (!stream || !word || !route || cursor > stream_size ||
        stream_size - cursor < 2u)
        return -1;
    *word = (int16_t)fa18_be16(stream + cursor);
    *route = *word < 0 ? FA18_SCENE_STREAM_WORD_NEGATIVE
                        : FA18_SCENE_STREAM_WORD_NONNEGATIVE;
    return 0;
}

int fa18_advance_scene_stream_cursor(FA18SceneStreamCursorState *state,
                                     int16_t word,
                                     FA18SceneStreamAdvanceRoute *route) {
    if (!state || !route) return -1;
    if (word == -1) {
        *route = FA18_SCENE_STREAM_ADVANCE_C1EDE4_PREDECESSOR;
    } else {
        state->cursor += 4u;
        *route = FA18_SCENE_STREAM_ADVANCE_INDIRECT_STAGE;
    }
    return 0;
}
