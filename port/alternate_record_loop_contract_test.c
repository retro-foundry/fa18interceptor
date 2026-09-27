#include "alternate_record_loop.h"

#include <assert.h>

static int32_t call(void *context, const FA18SceneDescriptorStageRecord *record) {
    unsigned *calls = context;
    assert(record->context == 0x22 && record->control_stream == 0x33);
    ++*calls;
    return 5;
}

int main(void) {
    FA18ScenePlacementRecord record = {0};
    unsigned calls = 0;
    FA18AlternateRecordLoopInput input = {
        0x1200, 0x2400, 0x18, 0, 7,
        {0, 0x22, 0x33, 0x44}, call, &calls
    };
    FA18AlternateRecordLoopState state;
    FA18AlternateRecordLoopRoute route;
    record.per_frame[1] = 9;
    record.per_frame[2] = 3;
    record.per_frame[3] = 4;
    assert(fa18_finish_alternate_record_loop(&record, &input, &state, &route) == 0);
    assert(route == FA18_ALTERNATE_RECORD_LOOP_DISPATCHED && calls == 1 &&
           state.record_byte == 2 && state.fixed_value == 0x7fff &&
           state.kind == 0x2400 && state.next_offset == 0x30 &&
           record.per_frame[2] == 2 && record.per_frame[3] == 5);

    record.per_frame[2] = 1;
    record.per_frame[3] = 0xffff;
    calls = 0;
    assert(fa18_finish_alternate_record_loop(&record, &input, &state, &route) == 0);
    assert(route == FA18_ALTERNATE_RECORD_LOOP_SKIPPED && !calls &&
           record.per_frame[2] == 0);
    return 0;
}
