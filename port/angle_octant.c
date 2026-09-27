#include "angle_octant.h"

int fa18_classify_angle_octant(FA18AngleOctantState *state) {
    int16_t angle;
    if (!state) return -1;
    angle = state->source_select_state ? state->primary_angle :
        (int16_t)(uint16_t)state->alternate_angle;
    if (angle >= 0x6270) state->octant = 7;
    else if (angle >= 0x5460) state->octant = 6;
    else if (angle >= 0x4650) state->octant = 5;
    else if (angle >= 0x3840) state->octant = 4;
    else if (angle >= 0x2a30) state->octant = 3;
    else if (angle >= 0x1c20) state->octant = 2;
    else if (angle >= 0x0e10) state->octant = 1;
    else state->octant = 0;
    return 0;
}
