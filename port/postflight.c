#include "postflight.h"

void fa18_postflight_scene_init(FA18PostflightScene *scene,
                                uint8_t table_selection,
                                uint16_t record_limit,
                                int16_t vertical_offset) {
    if (!scene) return;
    scene->table_selection = table_selection;
    scene->record_cursor = 0;
    scene->record_limit = record_limit;
    scene->vertical_offset = vertical_offset;
    scene->renderer_mode = 0;
}

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
