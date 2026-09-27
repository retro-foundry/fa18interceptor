#ifndef FA18_SCENE_STREAM_TRANSFORM_VECTOR_H
#define FA18_SCENE_STREAM_TRANSFORM_VECTOR_H

#include <stdint.h>

typedef struct {
    int32_t prepared_component[3];
    int32_t placement_x;
    int32_t placement_y;
    int32_t placement_z;
    uint8_t descriptor_low_nibble;
} FA18SceneStreamTransformVectorInput;

typedef struct {
    uint16_t shift_count;
    int16_t component[3];
} FA18SceneStreamTransformVector;

/* `$C1F078-$C1F0A7`: combine the first and third prepared components with
 * the placement triple, then materialize the source's three shifted words. */
int fa18_prepare_scene_stream_transform_vector(
    const FA18SceneStreamTransformVectorInput *input,
    FA18SceneStreamTransformVector *vector);

#endif
