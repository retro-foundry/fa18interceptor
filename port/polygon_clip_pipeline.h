#ifndef FA18_POLYGON_CLIP_PIPELINE_H
#define FA18_POLYGON_CLIP_PIPELINE_H

#include <stddef.h>
#include <stdint.h>

#include "negated_tuple_emit.h"

/* `$C246A0-$C24CFD` clips the caller's scaled triple loop successively at
 * y=z, -y=z, x=z, and -x=z before `$C24CFE` projects the resulting tuples.
 * The source's four fixed work areas can expand an input polygon, so the
 * native caller supplies the output capacity explicitly. */
int fa18_clip_projection_polygon(const FA18ClipTuple *input, uint16_t input_count,
                                 int16_t coordinate_shift, FA18ClipTuple *output,
                                 size_t output_capacity, uint16_t *output_count);

#endif
