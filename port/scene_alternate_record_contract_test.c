#include "scene_alternate_record.h"

#include <assert.h>

typedef struct {
    uint32_t reference;
    FA18SceneAlternateHandler handler;
} Fixture;

static int lookup(void *context, uint32_t reference, FA18SceneAlternateHandler *handler) {
    Fixture *fixture = context;
    assert(reference == fixture->reference);
    *handler = fixture->handler;
    return 0;
}

int main(void) {
    uint8_t bytes[FA18_SCENE_PLACEMENT_BYTES] = {0};
    bytes[0] = 0x15; bytes[1] = 0x02;
    bytes[2] = 0x00; bytes[3] = 0xc2; bytes[4] = 0x23; bytes[5] = 0x2c;
    bytes[6] = 0xff; bytes[7] = 0xf0;
    bytes[10] = 0x00; bytes[11] = 0x20;
    bytes[12] = 0x12; bytes[13] = 0x34;
    bytes[14] = 0x56; bytes[15] = 0x78;
    Fixture fixture = {0x00c2232cu, {0, 0x00c1ee14u}};
    FA18SceneAlternateRecordState state = {0};
    FA18SceneAlternateRecordRoute route;

    assert(fa18_prepare_scene_alternate_record(bytes, 0, lookup, &fixture,
                                                &state, &route) == 0);
    assert(route == FA18_SCENE_ALTERNATE_RECORD_PREPARED);
    assert(state.header == 0x1502 && state.selector == 2 && state.shift == 2 &&
           state.kind == 0x15 && state.tuple[0] == -16 && state.tuple[1] == 0 &&
           state.tuple[2] == 32 && state.work == 0x12345678u &&
           state.handler == 0x00c1ee14u && state.component_ready == 0);

    fixture.handler.first_word = -1;
    assert(fa18_prepare_scene_alternate_record(bytes, 0, lookup, &fixture,
                                                &state, &route) == 0);
    assert(route == FA18_SCENE_ALTERNATE_RECORD_SKIPPED);
    fixture.handler = (FA18SceneAlternateHandler){0, 0x00c1ed48u};
    assert(fa18_prepare_scene_alternate_record(bytes, -0x400001, lookup, &fixture,
                                                &state, &route) == 0);
    assert(route == FA18_SCENE_ALTERNATE_RECORD_SKIPPED);
    assert(fa18_prepare_scene_alternate_record(bytes, -0x400000, lookup, &fixture,
                                                &state, &route) == 0);
    assert(route == FA18_SCENE_ALTERNATE_RECORD_PREPARED);

    bytes[0] = 0xff; bytes[1] = 0xff;
    assert(fa18_prepare_scene_alternate_record(bytes, 0, lookup, &fixture,
                                                &state, &route) == 0);
    assert(route == FA18_SCENE_ALTERNATE_RECORD_TERMINATOR);
    return 0;
}
