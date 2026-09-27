#ifndef FA18_SCENE_VECTOR_TRANSFORM_H
#define FA18_SCENE_VECTOR_TRANSFORM_H

#include <stdint.h>

typedef struct {
    int16_t value[3][3];
} FA18SceneVectorMatrix;

typedef struct {
    int32_t value[3];
} FA18SceneVectorBase;

typedef struct {
    int16_t value[3];
} FA18SceneVectorInput;

typedef struct {
    int32_t value[3];
} FA18SceneVectorOutput;

/* `$C091E0-$C09248`: signed word products, wrapping long sums, arithmetic
 * right shift by four, then wrapping long translation. */
int fa18_transform_scene_vector(const FA18SceneVectorMatrix *matrix,
                                const FA18SceneVectorBase *base,
                                const FA18SceneVectorInput *input,
                                FA18SceneVectorOutput *output);

#endif
