#include "record_scan_candidate_prelude.h"

#include <assert.h>

typedef struct {
    unsigned commands;
    uint16_t first;
    uint16_t second;
    unsigned indexed;
    unsigned slots;
    uint16_t index;
} Fixture;

static uint16_t be16(const uint8_t *bytes) {
    return (uint16_t)((uint16_t)bytes[0] << 8 | bytes[1]);
}

static void put16(uint8_t *bytes, uint16_t value) {
    bytes[0] = (uint8_t)(value >> 8);
    bytes[1] = (uint8_t)value;
}

static int command(void *context, uint16_t first, uint16_t second) {
    Fixture *fixture = context;
    ++fixture->commands;
    fixture->first = first;
    fixture->second = second;
    return 0;
}

static int indexed(void *context, uint16_t index) {
    Fixture *fixture = context;
    ++fixture->indexed;
    fixture->index = index;
    return 0;
}

static int slot_stage(void *context, uint16_t index) {
    Fixture *fixture = context;
    ++fixture->slots;
    fixture->index = index;
    return 0;
}

int main(void) {
    FA18FlaggedSlot slot = {{0}};
    Fixture fixture = {0};
    const FA18RecordScanCandidateOps ops = {command, indexed, slot_stage, &fixture};
    FA18RecordScanCandidateState state = {
        0xa3, 2, 7, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    FA18RecordScanCandidateRoute route;

    assert(fa18_run_record_scan_candidate_prelude(&slot, 6, &state, &ops,
                                                   &route) == 0);
    assert(route == FA18_RECORD_SCAN_CANDIDATE_SLOT_STAGE);
    assert(fixture.commands == 1 && fixture.first == 0x1c && fixture.second == 0x30);
    assert(fixture.indexed == 1 && fixture.slots == 1 && fixture.index == 6);
    assert(state.table_end == 0x93 && state.selected_state_word_3a == 12 &&
           state.state_c457c5 == 1 && state.renderer_budget == 4 &&
           state.scan_counter == 0x14 && state.mode_c45843 == 3);
    assert(be16(slot.bytes + 0x26) == 0x0401 && be16(slot.bytes + 0x2e) == 0);

    slot = (FA18FlaggedSlot){{0}};
    fixture = (Fixture){0};
    state = (FA18RecordScanCandidateState){
        0x10, 1, 1, 0, 0, 0, 2, 1, 0, 0, 0, 0, 0, 0, 0};
    assert(fa18_run_record_scan_candidate_prelude(&slot, 8, &state, &ops,
                                                   &route) == 0);
    assert(route == FA18_RECORD_SCAN_CANDIDATE_SLOT_STAGE && fixture.commands == 1);
    assert(fixture.first == 0x1c && fixture.second == 0x30 &&
           (be16(slot.bytes + 0x26) & 0x1000u) && state.slot_c4588c == 9);
    assert(state.auxiliary_flag == 0 && state.renderer_budget == 1);

    slot = (FA18FlaggedSlot){{0}};
    fixture = (Fixture){0};
    state = (FA18RecordScanCandidateState){
        0x10, 1, 1, 0, 0, 0, 0, 1, 0x20, 0, 0, 0, 0, 0, 0};
    assert(fa18_run_record_scan_candidate_prelude(&slot, 1, &state, &ops,
                                                   &route) == 0);
    assert(fixture.commands == 1 && fixture.first == 0x06 &&
           fixture.second == 0x1e);

    slot = (FA18FlaggedSlot){{0}};
    put16(slot.bytes + 0x26, 1);
    fixture = (Fixture){0};
    state = (FA18RecordScanCandidateState){0};
    assert(fa18_run_record_scan_candidate_prelude(&slot, 4, &state, &ops,
                                                   &route) == 0);
    assert(route == FA18_RECORD_SCAN_CANDIDATE_SLOT_STAGE && fixture.slots == 1 &&
           !fixture.commands && !fixture.indexed);

    slot = (FA18FlaggedSlot){{0}};
    fixture = (Fixture){0};
    state = (FA18RecordScanCandidateState){0, 2, 0, 0, 0, 0, 0, 0, 0,
                                            0, 0, 0, 0, 0, 0};
    assert(fa18_run_record_scan_candidate_prelude(&slot, 2, &state, &ops,
                                                   &route) == 0);
    assert(route == FA18_RECORD_SCAN_CANDIDATE_ADVANCED && state.local_processed == 1);
    return 0;
}
