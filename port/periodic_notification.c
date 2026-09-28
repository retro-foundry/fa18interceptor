#include "periodic_notification.h"

int fa18_update_periodic_notification(FA18PeriodicNotificationState *state) {
    int8_t countdown;
    if (!state) return -1;
    state->countdown = (uint8_t)(state->countdown - 1u);
    countdown = (int8_t)state->countdown;
    if (countdown <= 0) {
        state->countdown = 8;
        state->code = 0x86;
        return 0;
    }
    if ((uint8_t)(state->countdown - 4u) == 0) {
        state->code = 6;
        return 0;
    }
    if ((uint8_t)((state->countdown & 3u) - 2u) == 0) {
        state->code = 4;
        return 0;
    }
    if ((int8_t)state->countdown > 8) {
        state->countdown = 8;
        state->code = 0;
    }
    return 0;
}
