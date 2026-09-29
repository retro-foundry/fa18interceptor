#ifndef FA18_GAME_FACES_H
#define FA18_GAME_FACES_H

/* Faces: polygons whose vertices are offsets into the transformed vertex
 * table (WORKSPACES, x, y, z words), copied into CLIP_INPUT unshifted and
 * drawn through clip_and_draw_polygon. A face with every z negative is
 * behind the eye and skipped. Each advances `*face` past its record and
 * returns what the clipper did (1 when drawn). */

#include "memory.h"

/* Word count, `count` vertex offsets, word colour ($C09952). At least
 * three offsets are read. */
int draw_indexed_face(gaddr *face);

/* Word count, one offset to `count` consecutive vertices, drawn in colour 7
 * with a colour-3 line on planes 0-1 ($C099F6). It copies one vertex past
 * the count, whose z also counts toward "behind". */
int draw_outlined_face(gaddr *face);

/* Word count, then the first `count` vertices of the table (at least four),
 * then a word colour ($C099AA). */
int draw_coloured_face(gaddr *face);

/* split_edge on the selected record's points (SELECTED_EDGE past its $A4
 * table), then on the workspace points the next stream offset names
 * ($C21C2E). */
void split_record_and_stream_edges(gaddr *stream);

/* From the two points at `points` (words 0-2 and 3-5), the midpoint to
 * +$1E and the quarter point to +$24 ($C21C4C). */
void split_edge(gaddr points);

#endif
