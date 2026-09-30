#ifndef FA18_GAME_RECORD_MATRIX_UPDATE_H
#define FA18_GAME_RECORD_MATRIX_UPDATE_H

#include "memory.h"

/* $C2D704-$C2D99A: the continuation after the record matrix transform.
 * Angles are D4-D6 on entry. Returns 1 when it reaches the orientation
 * writer, 0 when the source returns early after changing a velocity word. */
int finish_record_matrix_angles(gaddr record, uint16_t angles[3]);

#endif
