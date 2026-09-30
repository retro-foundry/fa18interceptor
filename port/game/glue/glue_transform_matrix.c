/* $C2DEE0: matrix product, angle extraction and caller-visible registers. */
#include "glue.h"
#include "matrix.h"
#include "globals.h"
#include "memory.h"

int glue_C2DEE0(void) {
    int16_t angles[3];
    MatrixTransformAngleState state;
    uint32_t last_term = build_transform_product(
        A(4), (uint16_t)D(0), (uint16_t)D(2), (uint16_t)D(4));
    extract_transform_angles(angles, &state);
    D(0) = state.final_d0;
    D(2) = (state.primary_clears_d2_high ? 0 : last_term & 0xFFFF0000u) |
           (uint16_t)state.secondary_raw;
    SET_W(D(3), state.divisor);
    D(7) = (uint16_t)state.primary_index;
    A(2) = MATRIX_TRANSFORM_ROTATION;
    A(3) = MATRIX_TRANSFORM_PRODUCT + 32;
    D(4) = (uint32_t)(int32_t)angles[0];
    D(5) = (uint32_t)(int32_t)angles[1];
    D(6) = (uint32_t)(int32_t)angles[2];
    flags_logic_l(D(6)); /* final ASL.L #3,D6 */
    return glue_return();
}
