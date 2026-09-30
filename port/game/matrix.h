#ifndef FA18_GAME_MATRIX_H
#define FA18_GAME_MATRIX_H

/* 3x3 rotation matrices: nine signed words, row by row. Angles are the
 * game's native angle words (eight per sin_cos unit, 0-28799). */

#include "memory.h"

/* Register outputs of the record adjustment below.  The original's caller
 * carries these values into the following matrix product. */
typedef struct MatrixDepthAdjustment {
    uint32_t d3;
    uint32_t d5;
    uint32_t d6;
    uint32_t d7;
} MatrixDepthAdjustment;

/* Rotation from three angles ($C2E47A), 2.14 fixed point ($4000 = 1). With
 * s/c the sine and cosine of a, b and c:
 *   [ cb*cc + (sb*sa)*sc    -ca*sc    sb*cc - (cb*sa)*sc ]
 *   [ cb*sc - (sb*sa)*cc     ca*cc    sb*sc + (cb*sa)*cc ]
 *   [ -sb*ca                -sa       cb*ca              ]
 * (each product shifted down by 14). */
void rotation_matrix(uint16_t a, uint16_t b, uint16_t c, gaddr out);

/* First arithmetic block of $C2DEE0: guard its three signed angle words,
 * build the rotation at $C45B90, and write nine full signed sums at $C45BA2.
 * The caller's matrix is nine signed words in row-major order. */
void build_transform_product(gaddr source, uint16_t a, uint16_t b, uint16_t c);

/* The three-angle rotation in 2.8 fixed point ($C2E3DE): the same terms,
 * each sum kept as the high word of its 2.14 product shifted down by 4, and
 * the middle bottom term -(sa >> 6). */
void rotation_matrix8(uint16_t a, uint16_t b, uint16_t c, gaddr out);

/* The other composition order ($C2E514), 2.14:
 *   [ cc*cb - (sc*sa)*sb   -(sc*cb + (cc*sa)*sb)   ca*sb ]
 *   [ sc*ca                 cc*ca                  sa    ]
 *   [ -(cc*sb + (sc*sa)*cb) sc*sb - (cc*sa)*cb     ca*cb ] */
void alternate_rotation_matrix(uint16_t a, uint16_t b, uint16_t c, gaddr out);

/* Rotation from two angles ($C2E38E), 2.8 fixed point: the three-angle
 * matrix with c = 0. */
void two_angle_matrix(uint16_t a, uint16_t b, gaddr out);

/* Multiply each row of `matrix` by the matching word of `scales`, keeping
 * 8 fraction bits ($C2E5AC). */
void scale_matrix_rows(gaddr matrix, gaddr scales);

/* $C2DD4E: adjust a record's matrix inputs and its +$22/+24/+54 working
 * words.  `d3`, `d5`, `d6`, and `d7` are the original's live register values
 * on entry and return. */
void adjust_matrix_record_depth(gaddr record, uint32_t d3, uint32_t d5,
                                uint32_t d6, uint32_t d7,
                                MatrixDepthAdjustment *out);

/* The matrix that undoes a record's orientation ($C2D970): the alternate
 * composition of the three angles negated (a full turn, $7080, less each
 * nonzero angle), written at record + $92. */
void inverse_orientation_matrix(gaddr record, uint16_t x, uint16_t y, uint16_t z);

/* Give a record its orientation ($C2D954): the three angle words at +$66,
 * the rotation at +$80 and its inverse at +$92. */
void set_record_orientation(gaddr record, uint16_t x, uint16_t y, uint16_t z);

/* A record-relative point in world coordinates ($C091E0, $C091CE,
 * $C091A8): `matrix` (2.14) times (x, y, z), shifted down by 4, plus the
 * record's position. */
void local_to_world(gaddr record, gaddr matrix, int16_t x, int16_t y, int16_t z, int32_t out[3]);

#endif
