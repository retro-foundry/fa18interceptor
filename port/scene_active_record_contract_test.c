#include "scene_active_record.h"

#include <assert.h>

int main(void) {
    uint8_t records[0x200] = { 0 };
    const uint8_t *record = 0;
    FA18SceneActiveRecordState state = { records, sizeof records, 0x20, 0 };

    assert(fa18_resolve_scene_active_record(&state, 0xa4, &record) == 0);
    assert(record == records + 0x20);
    state.selected_offset = 0x100;
    assert(fa18_resolve_scene_active_record(&state, 0xa4, &record) == 0);
    assert(record == records + 0x120);
    state.selected_offset = -0x20;
    assert(fa18_resolve_scene_active_record(&state, 0xa4, &record) == 0);
    assert(record == records);
    state.selected_offset = 0x160;
    assert(fa18_resolve_scene_active_record(&state, 0xa4, &record) == -1);
    assert(fa18_resolve_scene_active_record(0, 1, &record) == -1);
    return 0;
}
