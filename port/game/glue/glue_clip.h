#ifndef FA18_GLUE_CLIP_H
#define FA18_GLUE_CLIP_H

/* A private copy of the clip state, for replaying the clipper's register
 * flow while the C updates the real one (glue_batch36.c). */

#include <stdint.h>

typedef struct {
    int16_t state[4][6]; /* previous, then first */
    uint8_t started[4], passed[4];
    int16_t scratch[3];
} ClipCopy;

void load_clip_copy(ClipCopy *c);

/* The registers stage `k` ($C247C0 etc.) leaves, run on the copy. */
void clip_stage_registers(int k, ClipCopy *c);

/* The clipper $C246A0 for glue whose routine calls it: the snapshot is
 * taken before the C runs, the registers replayed after it, from the
 * registers at the call; `colour` is CURRENT_COLOUR at the call and
 * `drawn` what clip_and_draw_polygon returned (-1: derive it). */
typedef struct {
    ClipCopy copy;
    uint16_t last_size; /* BLTSIZE */
} ClipperSnapshot;

void clipper_snapshot(ClipperSnapshot *s);
void clipper_registers(ClipperSnapshot *s, uint16_t colour, int drawn);

#endif
