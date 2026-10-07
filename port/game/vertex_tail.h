#ifndef FA18_GAME_VERTEX_TAIL_H
#define FA18_GAME_VERTEX_TAIL_H

/* Derived vertices of a renderer workspace ($C0D384): word triples from
 * offset $84 on, built from the vertices at $06-$54 by midpoints and
 * parallelogram completions. */

#include "memory.h"

void derive_vertex_tail(gaddr workspace);

/* Stream operation $C0D334: the vertex tails of the shown record's
 * workspace (record + $A4) and of the workspace at WORKSPACES plus the
 * stream's operand word, then that workspace's +$294 = the midpoint of
 * +$60 and the midpoint of +$6C and +$72. Returns the stream after it. */
gaddr derive_shown_vertices(gaddr stream);

/* Stream operation $C219AE: operands a, b, w. With e = V(b) - V(a) in the
 * workspace bank and W = WORKSPACES + w: W+$12 = W+$00 + e; W+$18 and W+$1E
 * = W+$06 and W+$0C plus e/2. Then skips 14 bytes per step in bits 4-6 of
 * the shown record's +$7C. Returns the stream after it. */
gaddr derive_edge_vertices(gaddr stream);
/* C21FA4: five parallelogram completions at +$60..+$78 in the shown
 * record's +$A4 bank and WORKSPACES. No stream operand is consumed. */
void derive_shown_parallelogram_vertices(void);
/* C0D524: reflect four points about +$18, then four about +$12,
 * writing +$5A..+$84 in the shown record and transformed workspace. */
void derive_shown_reflected_vertices(void);
/* C0D61C: six points at +$8A..+$A8 from the first three vertices,
 * using word-wrapped midpoint/displacement arithmetic in both banks. */
void derive_shown_midpoint_vertices(void);
/* C21E08: ten extensions at +$42..+$78 in WORKSPACES, combining
 * the original points with word-wrapped displacements. No stream operand. */
void derive_workspace_extensions(void);
/* C21EF8: seven workspace points at +$1E..+$42, using wrapped
 * midpoint/reflection, quarter-displacement and translated vertices. */
void derive_workspace_midpoint_extensions(void);
/* C21B38: the compact hull's derived points, both in the shown record and
 * the transformed workspace selected by the operand. */
gaddr derive_compact_shown_vertices(gaddr stream);
/* C21C86: extended hull tails in both banks, then workspace point +$258. */
gaddr derive_extended_shown_vertices(gaddr stream);

#endif
