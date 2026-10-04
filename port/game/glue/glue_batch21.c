/* Glue for the view-plane clips ($C2EA5A, $C2EAD0, $C2EB4C, $C2EBC2,
 * $C2F0C6, $C2F0F4, $C2F156, $C2F128) and the view transform $C1F2EE. */
#include "glue.h"
#include "ports_glue.h"

#include "clip.h"
#include "globals.h"
#include "memory.h"
#include "view_transform.h"

/* $C1F2EE: A1 - 6 the point, the shift in the caller's frame at -8(A6), A3
 * the output (advanced by 6). Every register is live after it: D2-D4 the
 * last row's products and sum, D5-D7 the middle row's, A2 past the matrix. */
int glue_C1F2EE(void) {
    int16_t shift = rd_s16(A(6) - 8);
    int count = shift & 63, row;
    int16_t v[3];
    uint32_t product[3][3], sum[3];

    for (row = 0; row < 3; row++) v[row] = rd_s16(A(1) - 6 + (gaddr)(2 * row));
    v[1] = (int16_t)(v[1] - 0x28);
    v[2] = (int16_t)(v[2] - 0xA3);
    for (row = 0; row < 3; row++) v[row] = (int16_t)(count > 15 ? (v[row] < 0 ? -1 : 0) : v[row] >> count);
    for (row = 0; row < 3; row++) {
        int col;
        for (col = 0; col < 3; col++)
            product[row][col] = (uint32_t)((int32_t)v[col] * rd_s16(CAMERA_MATRIX + (gaddr)(6 * row + 2 * col)));
        sum[row] = (uint32_t)((int32_t)(product[row][2] + product[row][1] + product[row][0]) >> 8);
    }
    view_transform(A(1) - 6, shift, A(3));
    D(2) = product[2][0];
    D(3) = product[2][1];
    D(4) = sum[2];
    D(5) = product[1][0];
    D(6) = product[1][1];
    D(7) = sum[1];
    A(2) = CAMERA_MATRIX + 18;
    A(3) += 6;
    return glue_return();
}
