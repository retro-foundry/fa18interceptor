#ifndef FA18_MODEL_STRIPS_H
#define FA18_MODEL_STRIPS_H
#include "memory.h"

/* C1F584-C1F6F8: transform and interpolate the descriptor's paired strips.
 * Origin is the already scaled model displacement; shift applies to vertices.
 * Output advances through alternate six-byte lanes, then reverses for its pair. */
void transform_model_strips(gaddr input,gaddr output,const int16_t origin[3],int shift);
#endif
