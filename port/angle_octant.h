#ifndef FA18_ANGLE_OCTANT_H
#define FA18_ANGLE_OCTANT_H

#include <stdint.h>

/* Caller-owned source words selected by `$C254E8-$C2554B`. */
typedef struct {
    uint8_t source_select_state;
    int16_t primary_angle;
    int32_t alternate_angle;
    uint8_t octant;
} FA18AngleOctantState;

/* `$C254E8-$C2554B`: select the source angle and publish its native octant.
 * The result's gameplay meaning remains outside this bounded classifier. */
int fa18_classify_angle_octant(FA18AngleOctantState *state);

#endif
