#ifndef FA18_GAME_RENDER_BUFFERS_H
#define FA18_GAME_RENDER_BUFFERS_H

#include "memory.h"

/* Clear the renderer's work buffers ($C2FD22): the four A buffers (and the
 * fifth when FIFTH_BUFFER_USED), then the five B buffers. */
void clear_render_buffers(void);

/* Blit the polygon mask between the draw page's first two planes
 * ($C3040C): D = A ? B : C with A = POLY_MASK_SOURCE, B = plane 0 and
 * C = D = plane 1 at POLY_PLANE_OFFSET, POLY_BLIT_SIZE, descending. */
void blit_mask_between_planes(void);

/* Clear the first 40 bytes of each of the four planes of both pages
 * ($C2F582). */
void clear_page_plane_tops(void);

#endif
