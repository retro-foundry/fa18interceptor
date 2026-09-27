#ifndef FA18_SCENE_STREAM_VECTOR_H
#define FA18_SCENE_STREAM_VECTOR_H

#include <stdint.h>

typedef struct {
    int32_t component[3];
    uint16_t shift;
} FA18SceneStreamVectorInput;

typedef struct {
    int32_t shifted_component[3];
    int16_t scaled_component[3];
    int16_t negated_scaled_component[3];
} FA18SceneStreamVector;

/* `$C1EF2E-$C1EF5D`: load the shared prepared-component triple, perform the
 * variable arithmetic shifts when requested, then retain the source's
 * shifted-long, shifted-word, and negated-word local packets. */
int fa18_prepare_scene_stream_vector(const FA18SceneStreamVectorInput *input,
                                     FA18SceneStreamVector *vector);

#endif
