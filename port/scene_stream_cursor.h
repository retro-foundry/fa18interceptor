#ifndef FA18_SCENE_STREAM_CURSOR_H
#define FA18_SCENE_STREAM_CURSOR_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint32_t cursor;
    uint32_t auxiliary_cursor;
    uint8_t refresh_latch;
    uint8_t flight_update_mode;
} FA18SceneStreamCursorState;

typedef enum {
    FA18_SCENE_STREAM_REFRESH_INDIRECT_STAGE = 0,
    FA18_SCENE_STREAM_REFRESH_READ_WORD = 1,
    FA18_SCENE_STREAM_REFRESH_C1EDAC_BOUNDARY = 2
} FA18SceneStreamRefreshRoute;

typedef enum {
    FA18_SCENE_STREAM_WORD_NONNEGATIVE = 0,
    FA18_SCENE_STREAM_WORD_NEGATIVE = 1
} FA18SceneStreamWordRoute;

typedef enum {
    FA18_SCENE_STREAM_ADVANCE_C1EDE4_PREDECESSOR = 0,
    FA18_SCENE_STREAM_ADVANCE_INDIRECT_STAGE = 1
} FA18SceneStreamAdvanceRoute;

/* `$C1ED84-$C1EDAB`: refresh the published cursor from its auxiliary pointer. */
int fa18_refresh_scene_stream_cursor(FA18SceneStreamCursorState *state,
                                     FA18SceneStreamRefreshRoute *route);

/* `$C1EDCC-$C1EDD5`: consume the next signed big-endian stream word. */
int fa18_read_scene_stream_word(const uint8_t *stream, size_t stream_size,
                                uint32_t cursor, int16_t *word,
                                FA18SceneStreamWordRoute *route);

/* `$C1EDE8-$C1EDF5`: retain the `$FFFF` predecessor route; every other word
 * advances the shared cursor by four and returns to `$C1EE14`. */
int fa18_advance_scene_stream_cursor(FA18SceneStreamCursorState *state,
                                     int16_t word,
                                     FA18SceneStreamAdvanceRoute *route);

#endif
