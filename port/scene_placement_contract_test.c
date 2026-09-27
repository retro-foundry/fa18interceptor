#include "scene_placement.h"

#include <assert.h>
#include <string.h>

typedef struct {
    uint32_t expected_reference;
    FA18ScenePlacementDescriptorProbe probe;
    unsigned calls;
    FA18ScenePlacementRecord record;
    FA18ScenePlacementTraversalState state;
} Fixture;

static int depth_lookup(void *context, int16_t index, int8_t *value) {
    int16_t *observed_index = context;
    *observed_index = index;
    *value = -3;
    return 0;
}

static int lookup(void *context, uint32_t reference,
                  FA18ScenePlacementDescriptorProbe *probe) {
    Fixture *fixture = context;
    assert(reference == fixture->expected_reference);
    *probe = fixture->probe;
    return 0;
}

static int consume(void *context, const FA18ScenePlacementRecord *record,
                   const FA18ScenePlacementDescriptorProbe *probe,
                   const FA18ScenePlacementTraversalState *state) {
    Fixture *fixture = context;
    assert(probe == &fixture->probe || probe->first_long == fixture->probe.first_long);
    fixture->record = *record;
    fixture->state = *state;
    ++fixture->calls;
    return 0;
}

int main(void) {
    int16_t depth_index = 0;
    FA18ScenePlacementSelectorState selector;
    assert(fa18_prepare_scene_placement_selector(0, 0x12, 0x34, -0x0100,
                                                  depth_lookup, &depth_index,
                                                  &selector) == 0);
    assert(!selector.use_alternate_table && selector.initial_offset == 0x12 &&
           !selector.auxiliary_flag && !selector.status_flag && depth_index == 2 &&
           selector.comparison_word == 0xfd00);
    assert(fa18_prepare_scene_placement_selector(1, 0x12, 0x34, -0x8000,
                                                  depth_lookup, &depth_index,
                                                  &selector) == 0);
    assert(selector.use_alternate_table && selector.initial_offset == 0x34 &&
           selector.comparison_word == 0x7ffe);
    assert(fa18_prepare_scene_placement_selector(0, 0, 0, 0, 0, 0, &selector) == -1);

    uint8_t records[FA18_SCENE_PLACEMENT_BYTES * 2] = {0};
    records[0] = 0x15; records[1] = 0x02;
    records[2] = 0x00; records[3] = 0xc2; records[4] = 0x23; records[5] = 0x2c;
    records[6] = 0x20; records[7] = 0x00;
    records[10] = 0xff; records[11] = 0xf8;
    records[12] = 0x00; records[13] = 0x11;
    records[14] = 0x40; records[15] = 0x45;
    records[24] = 0xff; records[25] = 0xff;

    FA18ScenePlacementRecord decoded;
    assert(fa18_decode_scene_placement_record(records, &decoded) == 0);
    assert(decoded.selector == 0x1502 && decoded.descriptor_reference == 0x00c2232cu);
    assert(decoded.coordinate[0] == 8192 && decoded.coordinate[1] == 0 &&
           decoded.coordinate[2] == -8 && decoded.field_0c == 0x0011);
    assert(decoded.per_frame[0] == 0x4045 && decoded.per_frame[4] == 0);

    Fixture fixture = {0x00c2232cu, {0, 0}, 0, {0}, {0}};
    FA18ScenePlacementTraversalState state;
    assert(fa18_traverse_scene_placements(records, sizeof records, 0, 0, 0, 0,
                                          0, lookup, consume, &fixture, &state) == 0);
    assert(fixture.calls == 1 && state.accepted_count == 1 && state.skipped_count == 0);
    assert(state.next_offset == FA18_SCENE_PLACEMENT_BYTES);
    assert(fixture.state.selector_byte == 2 && fixture.state.selector_low_nibble == 2);
    assert(fixture.state.projection_coordinate[0] == 0x200000 &&
           fixture.state.projection_coordinate[1] == 0 &&
           fixture.state.projection_coordinate[2] == -2048);

    fixture.probe.first_word = -1;
    fixture.calls = 0;
    assert(fa18_traverse_scene_placements(records, sizeof records, 0, 0, 0, 0,
                                          0, lookup, consume, &fixture, &state) == 0);
    assert(fixture.calls == 0 && state.accepted_count == 0 && state.skipped_count == 1);

    fixture.probe = (FA18ScenePlacementDescriptorProbe){0, 0x00c1ed48u};
    assert(fa18_traverse_scene_placements(records, sizeof records, 0, 0, 0, 0,
                                          -0x380001, lookup, consume, &fixture, &state) == 0);
    assert(state.accepted_count == 0 && state.skipped_count == 1);
    assert(fa18_traverse_scene_placements(records, sizeof records, 0, 0, 0, 0,
                                          -0x380000, lookup, consume, &fixture, &state) == 0);
    assert(state.accepted_count == 1);

    assert(fa18_traverse_scene_placements(records, FA18_SCENE_PLACEMENT_BYTES - 1,
                                          0, 0, 0, 0, 0, lookup, consume,
                                          &fixture, &state) == -1);

    FA18ScenePlacementStageInput stage_input = {
        0, 0, 0, -0x100, 0,
        records, sizeof records, 0, 0,
        depth_lookup, &depth_index, lookup, consume, &fixture
    };
    FA18ScenePlacementStageResult stage_result;
    fixture.probe = (FA18ScenePlacementDescriptorProbe){0, 0};
    fixture.calls = 0;
    assert(fa18_run_scene_placement_stage(&stage_input, &stage_result) == 0);
    assert(depth_index == 2 && stage_result.selector.comparison_word == 0xfd00 &&
           stage_result.traversal.accepted_count == 1 && fixture.calls == 1);
    FA18ParentFlightPlacementStageContext callback = {&stage_input, &stage_result};
    assert(fa18_run_parent_flight_placement_stage(&callback) == 0 &&
           stage_result.traversal.accepted_count == 1);
    assert(fa18_run_scene_placement_stage(0, &stage_result) == -1);
    return 0;
}
