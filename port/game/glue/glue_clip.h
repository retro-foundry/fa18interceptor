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

#endif
