#include "scene_alternate_record.h"

int fa18_prepare_scene_alternate_record(
    const uint8_t bytes[FA18_SCENE_PLACEMENT_BYTES], int32_t guard_coordinate,
    FA18SceneAlternateHandlerLookup lookup, void *context,
    FA18SceneAlternateRecordState *state, FA18SceneAlternateRecordRoute *route) {
    FA18ScenePlacementRecord record;
    FA18SceneAlternateHandler handler;
    if (!bytes || !lookup || !state || !route ||
        fa18_decode_scene_placement_record(bytes, &record) != 0)
        return -1;
    if (record.selector == -1) {
        *route = FA18_SCENE_ALTERNATE_RECORD_TERMINATOR;
        return 0;
    }
    if (lookup(context, record.descriptor_reference, &handler) != 0) return -1;
    if (handler.first_word < 0 ||
        (handler.handler == UINT32_C(0x00c1ed48) &&
         guard_coordinate < -INT32_C(0x00400000))) {
        *route = FA18_SCENE_ALTERNATE_RECORD_SKIPPED;
        return 0;
    }
    state->header = (uint16_t)record.selector;
    state->selector = (uint8_t)record.selector;
    state->shift = state->selector & 0x0fu;
    state->kind = (uint16_t)record.selector >> 8;
    state->tuple[0] = record.coordinate[0];
    state->tuple[1] = record.coordinate[1];
    state->tuple[2] = record.coordinate[2];
    state->work = ((uint32_t)record.field_0c << 16) | record.per_frame[0];
    state->handler = handler.handler;
    state->component_ready = 0;
    *route = FA18_SCENE_ALTERNATE_RECORD_PREPARED;
    return 0;
}
