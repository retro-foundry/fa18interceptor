#ifndef FA18_SCENE_INITIALIZATION_H
#define FA18_SCENE_INITIALIZATION_H

#include <stdint.h>

typedef struct {
    uint8_t scene_latch_current;
    uint8_t scene_latch_previous;
    uint8_t scene_stage;
    uint8_t activity_mode;
    uint8_t update_flag;
    uint8_t scene_ready;
    uint8_t callback_mode;
    uint8_t scene_flag;
    uint8_t transition_auxiliary;
    uint16_t horizontal_offset;
    uint16_t vertical_offset;
    uint8_t marker;
} FA18SceneInitializationState;

typedef int (*FA18SceneInitializationCall)(void *context);

typedef struct {
    FA18SceneInitializationCall initialize_records;
    FA18SceneInitializationCall initialize_scene;
    FA18SceneInitializationCall prepare_callback;
    FA18SceneInitializationCall finalize_scene;
    void *context;
} FA18SceneInitializationOps;

/* `$C0FAA4-$C0FB26`: direct scene-initialization stores and ordered helper
 * calls. The helper bodies remain distinct source owners. */
int fa18_initialize_scene_state(FA18SceneInitializationState *state,
                                int16_t *countdown,
                                const FA18SceneInitializationOps *ops);

/* Adapter for `$C0FA04`'s caller-owned scene-initializer boundary. */
typedef struct {
    FA18SceneInitializationState *state;
    int16_t *countdown;
    const FA18SceneInitializationOps *ops;
} FA18SceneInitializationContext;

int fa18_initialize_scene_callback(void *context);

#endif
