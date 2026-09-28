/* Periodic notification cadence. */
#include "notify.h"

#include "globals.h"
#include "memory.h"

#define CADENCE_STEPS 8

void tick_notification_cadence(void) {
    int8_t step = (int8_t)(rd_u8(NOTIFY_COUNTDOWN) - 1);

    wr_u8(NOTIFY_COUNTDOWN, (uint8_t)step);
    if (step <= 0) {
        wr_u8(NOTIFY_COUNTDOWN, CADENCE_STEPS);
        wr_u8(NOTIFY_CODE, 0x86);
    } else if (step == 4) {
        wr_u8(NOTIFY_CODE, 0x06);
    } else if ((step & 3) == 2) {
        wr_u8(NOTIFY_CODE, 0x04);
    } else if (step > CADENCE_STEPS) {
        wr_u8(NOTIFY_COUNTDOWN, CADENCE_STEPS);
        wr_u8(NOTIFY_CODE, 0);
    }
}
