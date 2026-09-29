#ifndef FA18_GAME_PLANE_TESTS_H
#define FA18_GAME_PLANE_TESTS_H

/* Plane-side tests over face streams, for the candidate record scan
 * ($C26EBE). */

#include "memory.h"

/* Test the faces listed at `*stream` (face record pointers, ended by a
 * negative long) against the eye. Each face record's words +2, +4, +6
 * (the last masked to 12 bits) are offsets past $A4 into `record` of its
 * points p, q, r. With n = (q - p) x (r - p) >> (7 + shift) and
 * e = eye - (p >> shift + the record's position: word +$C, long +$10,
 * word +$E), a face is behind when n . e < 0. Returns 1 when every face is
 * behind, 0 at the first that is not; `*stream` is left 2 bytes into the
 * ending long either way ($C27456). */
int faces_all_behind(gaddr *stream, gaddr record, int16_t shift,
                     int16_t eye_x, int32_t eye_y, int16_t eye_z);

/* Whether a face turns toward the eye ($C1FB8C). With `kind` bits 12-13
 * set the face carries its own point and normal (six words at `points`
 * plus the next offset of `*faces`, which advances): the point, shifted by
 * BOUND_SHIFT and offset by BOUND_OFFSET_X/Z, less `eye`, dotted with the
 * normal. Otherwise the normal is the cross product of the first three
 * clipper input vertices (shifted up by `kind` bits 7-9, the products down
 * by 8) dotted with the first vertex. 1 when the dot is not negative. */
int face_toward_eye(uint16_t kind, gaddr points, gaddr *faces, const int16_t eye[3]);

/* The stored-normal test on its own ($C1FB9C): `point` shifted by
 * BOUND_SHIFT and offset by BOUND_OFFSET_X/Z, less `eye`, dotted with
 * `normal`; 1 when not negative. */
int point_toward_eye(const int16_t point[3], const int16_t normal[3], const int16_t eye[3]);

#endif
