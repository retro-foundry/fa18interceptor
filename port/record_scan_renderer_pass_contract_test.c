#include "record_scan_renderer_pass.h"

#include <assert.h>

static uint16_t be16(const uint8_t *bytes) {
    return (uint16_t)((uint16_t)bytes[0] << 8 | bytes[1]);
}

static void put16(uint8_t *bytes, uint16_t value) {
    bytes[0] = (uint8_t)(value >> 8);
    bytes[1] = (uint8_t)value;
}

int main(void) {
    FA18FlaggedSlot slots[FA18_FLAGGED_SLOT_SCAN_SLOTS] = {{0}};
    FA18RecordScanRendererPassInput input = {0};
    FA18RecordScanRendererPassState state = {0, 4, 0xa3};
    FA18RecordScanRendererPassRoute route;
    for (unsigned index = 0; index < FA18_FLAGGED_SLOT_SCAN_SLOTS; ++index)
        put16(slots[index].bytes + 0x28, (uint16_t)(0x100 + index));
    assert(fa18_run_record_scan_renderer_pass(slots, &input, &state, &route) == 0);
    assert(route == FA18_RECORD_SCAN_RENDERER_PASS_COMPLETE);
    assert(state.renderer_bound == 0x000fffff && state.scan_counter == 0 &&
           state.table_end == 0x53);
    for (unsigned index = 0; index < FA18_FLAGGED_SLOT_SCAN_SLOTS; ++index)
        assert(be16(slots[index].bytes + 0x28) ==
               (uint16_t)(0x100 + index - ((index & 1u) ? 0 : 1)));

    state = (FA18RecordScanRendererPassState){0, 0, 0};
    input = (FA18RecordScanRendererPassInput){0, 1, 0, 0};
    assert(fa18_run_record_scan_renderer_pass(slots, &input, &state, &route) == 0);
    assert(route == FA18_RECORD_SCAN_RENDERER_PASS_COMPLETE &&
           be16(slots[0].bytes + 0x28) == 0xff);

    input = (FA18RecordScanRendererPassInput){0, 0, 8, 0};
    state = (FA18RecordScanRendererPassState){0, 2, 0};
    assert(fa18_run_record_scan_renderer_pass(slots, &input, &state, &route) == 0);
    assert(route == FA18_RECORD_SCAN_RENDERER_PASS_CANDIDATE_CONTINUATION &&
           state.scan_counter == 1);
    input.flags = 0;
    slots[6].bytes[0x27] = 1;
    assert(fa18_run_record_scan_renderer_pass(slots, &input, &state, &route) == 0);
    assert(route == FA18_RECORD_SCAN_RENDERER_PASS_CANDIDATE_CONTINUATION);
    return 0;
}
