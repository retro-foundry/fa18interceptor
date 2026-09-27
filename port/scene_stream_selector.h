#ifndef FA18_SCENE_STREAM_SELECTOR_H
#define FA18_SCENE_STREAM_SELECTOR_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
    FA18_SCENE_STREAM_SELECTOR_SELECTED = 0,
    FA18_SCENE_STREAM_SELECTOR_RETRY = 1,
    FA18_SCENE_STREAM_SELECTOR_SENTINEL = 2
} FA18SceneStreamSelectorRoute;

typedef struct {
    uint32_t next_cursor;
    int16_t selected_word;
} FA18SceneStreamSelectorResult;

/* `$C1EE58-$C1EE83`: navigate one threshold-controlled record step. */
int fa18_select_scene_stream_threshold(const uint8_t *stream, size_t stream_size,
                                       uint32_t cursor, uint32_t record_base,
                                       int16_t first_word, int16_t shift,
                                       int16_t limit,
                                       FA18SceneStreamSelectorResult *result,
                                       FA18SceneStreamSelectorRoute *route);

#endif
