#include "attitude.h"

#include "globals.h"

void attitude_angles(int16_t *a, int16_t *b) {
    if (rd_u8(CONTEXT_SELECT)) {
        *a = (int16_t)(rd_s16(VIEW_PAN) >> 3);
        *b = 0;
    } else {
        *a = (int16_t)(rd_s32(ATTITUDE_A) >> 3);
        *b = (int16_t)(rd_s32(ATTITUDE_B) >> 3);
    }
}

/* Between 90 and 270 degrees: past vertical. */
static int past_vertical(int16_t angle) {
    return angle >= 900 && angle < 2700;
}

void update_attitude_flags(void) {
    int16_t a, b;
    int inverted;

    attitude_angles(&a, &b);
    if (a > 600 && a < 3150) {
        if (!rd_u8(ATTITUDE_LATCH)) {
            wr_u8(ATTITUDE_LATCH, 1);
            wr_u8(UPDATE_MASK, (uint8_t)(rd_u8(UPDATE_MASK) | 0x0B));
            wr_u8(ATTITUDE_BAND, 3);
        }
    } else {
        if (rd_u8(ATTITUDE_LATCH)) {
            wr_u8(ATTITUDE_LATCH, 0);
            wr_u8(UPDATE_MASK, (uint8_t)(rd_u8(UPDATE_MASK) | 0x0B));
        }
        if (a > 450 && a < 3200) {
            wr_u8(ATTITUDE_NEAR, 1);
        } else {
            wr_u8(ATTITUDE_NEAR, 0);
            if (a >= 3200) wr_u8(ATTITUDE_BAND, 0);
            else if (a > 300) wr_u8(ATTITUDE_BAND, 2);
            else if (a > 100) wr_u8(ATTITUDE_BAND, 1);
            else wr_u8(ATTITUDE_BAND, 0);
        }
    }

    wr_u16(STATUS_CA, (uint16_t)(rd_u16(STATUS_CA) & ~0x0002));
    /* B's test is inclusive of 2700 when A is not past vertical. */
    if (past_vertical(a)) inverted = !past_vertical(b);
    else inverted = b >= 900 && b <= 2700;
    if (inverted) wr_u16(STATUS_CA, (uint16_t)(rd_u16(STATUS_CA) | 0x0002));
}
