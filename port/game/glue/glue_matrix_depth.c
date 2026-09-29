/* $C2DD4E: record matrix-depth adjustment register replay. */
#include "glue.h"
#include "ports_glue.h"

#include "matrix.h"

int glue_C2DD4E(void) {
    uint32_t saved_d0 = D(0), saved_d1 = D(1), saved_d4 = D(4);
    gaddr saved_a0 = A(0);
    MatrixDepthAdjustment adjustment;

    /* The original saves D0-D1/D4/A0, then adds its transformed D3/D5 words
     * into the restored D0/D2 pair at the shared return tail. */
    adjust_matrix_record_depth(A(1), D(3), D(5), D(6), D(7), &adjustment);
    D(0) = saved_d0;
    D(1) = saved_d1;
    D(4) = saved_d4;
    A(0) = saved_a0;
    SET_W(D(0), (uint16_t)((int16_t)D(0) + (int16_t)adjustment.d3));
    SET_W(D(2), (uint16_t)((int16_t)D(2) + (int16_t)adjustment.d5));
    D(3) = adjustment.d3;
    D(5) = adjustment.d5;
    D(6) = adjustment.d6;
    D(7) = adjustment.d7;
    return glue_return();
}
