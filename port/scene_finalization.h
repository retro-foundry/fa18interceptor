#ifndef FA18_SCENE_FINALIZATION_H
#define FA18_SCENE_FINALIZATION_H

#include <stdint.h>

enum { FA18_SCENE_FINALIZATION_CONTROL_COUNT = 14 };

/* Direct state written by `$C082B0-$C08322`. Names stay structural until the
 * consumers of the individual bytes are reconstructed. */
typedef struct {
    uint16_t stage_word;
    uint8_t controls[FA18_SCENE_FINALIZATION_CONTROL_COUNT];
    uint8_t guard;
    uint16_t guarded_word;
    uint32_t guarded_long;
} FA18SceneFinalizationState;

int fa18_finalize_scene_state(FA18SceneFinalizationState *state);
int fa18_finalize_scene_callback(void *context);

#endif
