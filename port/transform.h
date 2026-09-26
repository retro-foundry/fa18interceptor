#ifndef FA18_TRANSFORM_H
#define FA18_TRANSFORM_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} FA18LocalVertex;

typedef struct {
    int16_t value[3][3];
} FA18TransformMatrix;

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} FA18TransformTranslation;

typedef struct {
    uint8_t local_shift;
    FA18TransformTranslation translation;
    FA18TransformMatrix matrix;
} FA18VertexTransform;

typedef struct {
    int16_t x;
    int16_t y;
    int16_t depth;
} FA18TransformedVertex;

/* Proved fixed-point core shared by $C1F4AC and its $C1F524 continuation.
 * Each local component is arithmetic-shifted, translated with 16-bit wrap,
 * then multiplied by the signed 8.8 matrix. Each wrapped 32-bit dot product
 * is arithmetic-shifted by eight and retained as a 16-bit output component. */
int fa18_transform_vertices(const FA18VertexTransform *transform,
                            const FA18LocalVertex *local_vertices,
                            size_t vertex_count,
                            FA18TransformedVertex *output_vertices);

#endif
