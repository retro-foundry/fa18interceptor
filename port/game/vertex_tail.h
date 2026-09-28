#ifndef FA18_GAME_VERTEX_TAIL_H
#define FA18_GAME_VERTEX_TAIL_H

/* Derived vertices of a renderer workspace ($C0D384): word triples from
 * offset $84 on, built from the vertices at $06-$54 by midpoints and
 * parallelogram completions. */

#include "memory.h"

void derive_vertex_tail(gaddr workspace);

#endif
