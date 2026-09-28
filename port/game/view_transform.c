#include "view_transform.h"

#include "globals.h"

void view_transform(gaddr in, int16_t shift, gaddr out) {
    int count = shift & 63;
    int16_t v[3];
    int row;

    v[0] = rd_s16(in);
    v[1] = (int16_t)(rd_s16(in + 2) - 0x28);
    v[2] = (int16_t)(rd_s16(in + 4) - 0xA3);
    for (row = 0; row < 3; row++) v[row] = (int16_t)(count > 15 ? (v[row] < 0 ? -1 : 0) : v[row] >> count);
    for (row = 0; row < 3; row++) {
        gaddr m = CAMERA_MATRIX + (gaddr)(6 * row);
        int32_t sum = (int32_t)v[2] * rd_s16(m + 4) + (int32_t)v[1] * rd_s16(m + 2) + (int32_t)v[0] * rd_s16(m);
        wr_s16(out + (gaddr)(2 * row), (int16_t)(sum >> 8));
    }
}
