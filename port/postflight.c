#include "postflight.h"

int fa18_postflight_submit(FA18PostflightState *state,
                           FA18PostflightRecord record,
                           FA18PostflightSubmit submit, void *context) {
    if (!state || !submit || state->submitted >= state->table_limit) {
        if (state) state->rejected = 1;
        return state ? 1 : -1;
    }
    const int16_t y = (int16_t)((uint16_t)record.y +
                                (uint16_t)state->vertical_offset);
    const FA18PostflightRenderer renderer =
        (record.flags & 1u) ? FA18_POSTFLIGHT_ADJACENT : FA18_POSTFLIGHT_SHARED;
    if (submit(renderer, record.x, y, context) != 0) return -1;
    ++state->submitted;
    return 0;
}
