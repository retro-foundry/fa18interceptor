#ifndef FA18_SCENE_STREAM_PAIR_TRANSFORM_H
#define FA18_SCENE_STREAM_PAIR_TRANSFORM_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    int16_t value[9];
} FA18SceneStreamPairMatrix;

typedef struct {
    int16_t pair_offset[2];
    int16_t output_base[3];
    uint16_t shift;
    FA18SceneStreamPairMatrix matrix;
} FA18SceneStreamPairTransform;

typedef struct {
    int16_t value[2];
} FA18SceneStreamPair;

typedef struct {
    int16_t value[3];
} FA18SceneStreamPairOutput;

/* `$C1F404-$C1F45B`: transform a positive counted run of two-word records
 * through the source's sparse matrix-lane pattern into three-word records.
 * The post-loop flag route is owned by the caller. */
int fa18_transform_scene_stream_pairs(
    const FA18SceneStreamPairTransform *transform,
    const FA18SceneStreamPair *pairs, size_t pair_count, int16_t count,
    FA18SceneStreamPairOutput *output, size_t output_count,
    uint16_t *transformed_count);

#endif
