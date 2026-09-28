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

#endif
