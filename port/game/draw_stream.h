#ifndef FA18_GAME_DRAW_STREAM_H
#define FA18_GAME_DRAW_STREAM_H

/* Commands of an object's draw stream: segments and faces from the
 * transformed vertex table (WORKSPACES), each command's words read from
 * `*stream`, which advances. Vertex offsets are byte offsets into the
 * table (6 bytes a vertex). The drawing ones return 1 when something was
 * drawn (segments: any of them), 0 otherwise. */

#include "memory.h"

/* Colour, then vertex offset pairs, the second offset of the last pair
 * with bit 15 set: a segment for each pair not wholly behind the eye.
 * LINE_STYLE is cleared to "none given" ($C212B0). */
int draw_segment_pairs(gaddr *stream);
/* The same only while PROJECTION_Y is below -$C0; otherwise the commands
 * are skipped to the first negative word ($C2129C). */
int draw_segment_pairs_near(gaddr *stream);

/* A word (count in bits 8-15, colour in bits 0-5), then a vertex offset:
 * `count` segments between consecutive vertex pairs from there ($C211DC). */
int draw_segment_run(gaddr *stream);

/* Colour, a base vertex offset, then offset pairs (bit 15 of the pair
 * after the last marks it): each pair's segment moved back by the base
 * vertex's edge (the next vertex less it) ($C2131C). */
int draw_offset_segments(gaddr *stream);

/* Colour, then a vertex offset: the parallelogram of that vertex, the next
 * and the one after, completed by a fourth corner, as a face, drawn with
 * line planes 8, colour 8 ($C20E4E) or planes 2, colour 0, complement 2
 * ($C20E40). */
int draw_parallelogram_face(gaddr *stream);
int draw_parallelogram_face_2(gaddr *stream);

/* The same parallelogram without a line style, and only once PROJECTION_Y
 * is -$80 or more; its "all behind" test reads three words past the
 * fourth corner instead of the corners' depths ($C21490). */
int draw_parallelogram_face_near(gaddr *stream);

/* Colour, then four offsets a, b, c, d: the face b, c, d and d less the
 * edge from a to the vertex after it ($C2139E). */
int draw_offset_face(gaddr *stream);

/* Colour, then offsets a, b, c: the face a, a less b's edge, c less b's
 * edge, c ($C21412). */
int draw_mixed_face(gaddr *stream);

/* From a vertex offset: points 4-7 of the block there, from points 0-3:
 * p2 and p3 moved by p0 - p1, and p3 moved by p2 - p1 and by both
 * ($C20F10). */
void extend_parallelograms(gaddr *stream);

/* From a vertex offset and a shift: points 8 and 9 (p3 less the p2-p3 edge
 * scaled by the shift, then less the p1-p2 edge), then as
 * extend_parallelograms ($C20EC4). */
void extend_parallelograms_scaled(gaddr *stream);

/* Colour, a vertex offset, a row count, then each row's column count: a
 * grid of segments. With q the block's third vertex, u and v its first two
 * less q: row r's segments run from q_r + c*u/2 to that plus v, q_r
 * stepping one vertex a row ($C20D68). */
int draw_segment_grid(gaddr *stream);

/* Colour, a vertex offset, a count: that many segments along u (stepping by
 * v/2) from q, then as many back from the far corner (q - v/2 + u)
 * ($C20904). */
int draw_segment_lattice(gaddr *stream);

/* A block offset, then a reference offset: three copies of the block's
 * points 1-5 moved by (reference vertex k - block point 0), to points
 * 30-44; then skip the stream by the next word ($C21A20). */
void offset_block_copies(gaddr *stream);

/* A vertex offset and a shift: points 9-17 of the block from its points 1-8
 * (a scaled edge, and moves along the edges of points 4-8) ($C217EA). */
void extend_block_scaled(gaddr *stream);

/* Colour, two offsets a, b into BOUND_RECORD's points (+$A/+$E: x, z; b's
 * bit 15 flips the choice), then a vertex offset and a block offset: a
 * triangle from that vertex and the block, facing one way or the other by
 * which side of the edge a..b the eye is on ($C2159E). The clipper input's
 * shift word is left as it was. */
int draw_side_face(gaddr *stream);

/* Colour 12, line planes 8, colour 8: quads of four vertex offsets until a
 * negative offset, each unless wholly behind ($C21060). */
int draw_quad_list(gaddr *stream);

/* A block offset and a count: quads q, q + a, q + a + b, q + b for q
 * stepping from the block's point 3, a = 3/2 (p1 - p3) + (p0 - p3) and
 * b = p2 - p3; line planes 2, colour 0, complement 2 ($C210E6). */
int draw_quad_strip(gaddr *stream);

/* A block offset, a row count, then each row's column count: a grid of
 * faces. With q stepping from the block's point 3 a row, u, v, w its points
 * 0, 1, 2 less point 3, the face at column c spans q + c*v/2 by u and w.
 * Colour $D, line planes 2, complement 2 ($C20C38), or the colour from the
 * stream and no line style ($C20C22). */
int draw_face_grid(gaddr *stream);
int draw_face_grid_plain(gaddr *stream);

/* A block offset and a count: faces along v (half steps) between points 2
 * and 3 of the block, across u; then as many back from the far edge (drawn,
 * not counted in the result). Colour $D, line planes 2, complement 2
 * ($C20A52), or the colour from the stream, no line style ($C20A40). */
int draw_face_lattice(gaddr *stream);
int draw_face_lattice_plain(gaddr *stream);

/* A face of a workspace block ($C21500): colour word, block offset. Its
 * fourth vertex and the edges from the first to the second and the second
 * to the third make a parallelogram, clipped and drawn unless the view is
 * more than $80 below (PROJECTION_Y) or every corner is behind. */
int draw_block_face(gaddr *stream);

/* A word (count in bits 8-15, colour in bits 0-5), a base vertex offset,
 * then a vertex offset: `count` segments between consecutive vertex pairs
 * from there (at least one), each moved back by the base vertex's edge
 * ($C2122A). */
int draw_offset_run(gaddr *stream);

/* The ground square in the first workspaces ($C20592), split by the
 * diagonal the eye sees across: a long of two colours, the first when the
 * eye's x offset from the square's corner is the larger. -1 when the second
 * workspace point is outside the view. */
int draw_split_square(gaddr *stream);

/* A triangle beside the bound record's edge ($C2168A): colour, a vertex
 * whose edge moves the first corner back, the edge's two bound points (bit
 * 15 of the second flags the other side), the first corner's vertex, then a
 * block whose fourth vertex and edges give the other two corners, by the
 * side of the edge the eye is on. Nothing when all three are behind. */
int draw_side_triangle(gaddr *stream);

/* 1 when the eye's x and z offsets from the ground square's corner have
 * the same sign; `colour` is the second word of `colours` when the z offset
 * is the larger, else the first. */
int square_diagonal(uint32_t colours, uint16_t *colour);

/* The ground square in the first workspaces as quads ($C203D0): a long of
 * two colours as for draw_split_square, then a word colour for the second
 * quad drawn while ATTITUDE_LATCH is set. -1 when the second workspace
 * point is outside the view. */
int draw_square_faces(gaddr *stream);

/* The shadow of the record at SCRIPT_RECORD ($C201A6), in colour 0: its
 * hull (record +$A4) placed at the shadow point, each face (count, one-sided
 * flag, vertex offsets) clipped and drawn unless it turns away, until a
 * face's height word is below the record's +$10. It works in its caller's
 * frame at `frame` (the placed point and scales). Returns 1, or -1 when the
 * record is far below. */
int draw_record_shadow(gaddr *stream, gaddr frame);

/* Three vertex offsets and a face kind word ($C1FF0A). The vertices become
 * the clipper input's first three, where the face test reads them; the 18
 * bytes after the kind are skipped when the test passes. It works in its
 * caller's frame at `frame` (the point table at -$2C, the eye at -$26). */
int test_stream_face(gaddr *stream, gaddr frame);

#endif
