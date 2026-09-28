#ifndef FA18_GAME_MATRIX_H
#define FA18_GAME_MATRIX_H

/* 3x3 rotation matrices: nine signed words, row by row. Angles are the
 * game's native angle words (eight per sin_cos unit, 0-28799). */

#include "memory.h"

/* Rotation from three angles ($C2E47A), 2.14 fixed point ($4000 = 1). With
 * s/c the sine and cosine of a, b and c:
 *   [ cb*cc + (sb*sa)*sc    -ca*sc    sb*cc - (cb*sa)*sc ]
 *   [ cb*sc - (sb*sa)*cc     ca*cc    sb*sc + (cb*sa)*cc ]
 *   [ -sb*ca                -sa       cb*ca              ]
 * (each product shifted down by 14). */
void rotation_matrix(uint16_t a, uint16_t b, uint16_t c, gaddr out);

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

#endif
