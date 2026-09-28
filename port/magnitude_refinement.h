#ifndef FA18_MAGNITUDE_REFINEMENT_H
#define FA18_MAGNITUDE_REFINEMENT_H

#include <stdint.h>

/* `$C2564E-$C25703`: refine the caller-owned unsigned square sum into the
 * source threshold word. DIVU.W faults are reported as -1. */
int fa18_refine_component_magnitude(uint32_t square_sum, uint16_t *threshold);

#endif
