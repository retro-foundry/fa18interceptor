#include "record_delta_scan.h"

#include <assert.h>

typedef struct {
    unsigned calls;
    uint16_t selector;
    int32_t component[3];
} Fixture;

static int submit(void *context, uint16_t selector, int32_t component_0,
                  int32_t component_1, int32_t component_2) {
    Fixture *fixture = context;
    ++fixture->calls;
    fixture->selector = selector;
    fixture->component[0] = component_0;
    fixture->component[1] = component_1;
    fixture->component[2] = component_2;
    return 0;
}

int main(void) {
    FA18RecordDeltaScanRecord records[FA18_RECORD_DELTA_SCAN_RECORDS] = {{0}};
    records[1] = (FA18RecordDeltaScanRecord){0x40, {0x1000, -0x2000, 0x3000}};
    records[2] = (FA18RecordDeltaScanRecord){0x40,
        {0x04000000, -0x08000000, 0x0c000000}};
    Fixture fixture = {0};
    FA18RecordDeltaScanInput input = {0, 0, 0, 0, records, submit, &fixture};
    FA18RecordDeltaScanState state;

    assert(fa18_scan_record_delta_submission(&input, &state) == 0);
    assert(state.latch == 0);
    assert(state.mode == 1);
    assert(state.result_offset == 0);
    assert(state.submitted_count == 2);
    assert(fixture.calls == 2);
    assert(fixture.selector == 0x0210);
    assert(fixture.component[0] == 0x4000);
    assert(fixture.component[1] == -0x8000);
    assert(fixture.component[2] == 0xc000);

    input.scene_active = 1;
    assert(fa18_scan_record_delta_submission(&input, &state) == 0 &&
           !state.mode && !state.submitted_count);
    input.scene_active = 0;
    input.selector = 1;
    assert(fa18_scan_record_delta_submission(&input, &state) == 0 &&
           state.mode == 1 && !state.submitted_count);
    input.selector = 2;
    input.activity = 2;
    assert(fa18_scan_record_delta_submission(&input, &state) == 0 &&
           state.mode == 1 && state.submitted_count == 2);
    input.selected_offset = 1;
    assert(fa18_scan_record_delta_submission(&input, &state) == -1);
    return 0;
}
