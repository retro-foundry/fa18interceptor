#include "scene_entry_runtime.h"

#include <assert.h>

int main(void) {
    uint8_t positive_bytes[0x1710] = {0};
    uint8_t template_bytes[0x200] = {0};
    uint8_t dispatch_bytes[0x400] = {0};
    uint8_t descriptor_bytes[0x100] = {0};
    uint8_t trig_bytes[0x1400] = {0};
    uint8_t record_bytes[0x100] = {0};
    FA18HunkReloc relocs[5];
    FA18HunkSegment segments[68] = {{0}};
    FA18Hunks hunks = {segments, 68};
    FA18SceneDispatchTable dispatch_table;
    FA18SceneRecordTable record_table;
    FA18SceneEntryRuntime runtime;
    FA18SceneInitializationState state = {.scene_latch_current = 9};
    int16_t countdown = -1;
    const uint32_t base = FA18_SCENE_DISPATCH_TABLE_OFFSET;

    for (unsigned index = 0; index != 5; ++index)
        relocs[index] = (FA18HunkReloc){0xa0u + index * 4u, 52};
    segments[8] = (FA18HunkSegment){.data = positive_bytes, .size = sizeof positive_bytes};
    segments[16] = (FA18HunkSegment){.data = template_bytes, .size = sizeof template_bytes,
                                     .relocs = relocs, .reloc_count = 5};
    segments[27] = (FA18HunkSegment){.data = dispatch_bytes, .size = sizeof dispatch_bytes};
    segments[52] = (FA18HunkSegment){.data = descriptor_bytes, .size = sizeof descriptor_bytes};
    segments[63] = (FA18HunkSegment){.data = trig_bytes, .size = sizeof trig_bytes};
    segments[67] = (FA18HunkSegment){.data = record_bytes, .size = sizeof record_bytes};

    /* The original mode-$7F row selects record 14. */
    dispatch_bytes[base + 0x6c + 1] = 0xa0;
    dispatch_bytes[base + 0x6c + 3] = 6;
    dispatch_bytes[base + 0xa0 + 1] = 8;
    dispatch_bytes[base + 0xa8 + 1] = 0xa0;
    dispatch_bytes[base + 0xaa + 1] = 0x20;
    dispatch_bytes[base + 0xac] = 0x8d;
    dispatch_bytes[base + 0xad] = 0x0e;
    dispatch_bytes[base + 0xae + 1] = 0x40;
    dispatch_bytes[base + 0xb0] = 0x80;
    dispatch_bytes[base + 0xb2] = 0xff;
    dispatch_bytes[base + 0xb3] = 0xff;
    dispatch_bytes[0x40] = 0; dispatch_bytes[0x41] = 0x44;
    dispatch_bytes[0x42] = 0; dispatch_bytes[0x43] = 0x44;
    dispatch_bytes[0x44] = 0x18; dispatch_bytes[0x45] = 0;
    dispatch_bytes[0x46] = 0x18; dispatch_bytes[0x47] = 0;
    template_bytes[0xa0 + 19] = 0x80;
    descriptor_bytes[0] = 0x80; descriptor_bytes[1] = 3;
    descriptor_bytes[9] = 5;
    descriptor_bytes[0x82] = 0x80;
    descriptor_bytes[0x85] = 0x70;
    record_bytes[FA18_SCENE_RECORD_TABLE_A_OFFSET + 3 * 16] = 0x80;
    record_bytes[FA18_SCENE_RECORD_TABLE_A_OFFSET + 3 * 16 + 1] = 0x0e;

    assert(fa18_load_scene_dispatch_table(&hunks, &dispatch_table) == 0);
    assert(fa18_load_scene_record_table(&hunks, &record_table) == 0);
    assert(fa18_scene_entry_runtime_init(&runtime, &hunks, &dispatch_table,
                                         &record_table) == 0);
    assert(fa18_run_scene_entry_runtime(&runtime, 0x7f, &state, &countdown) == 0);
    assert(state.scene_latch_previous == 9 && state.scene_stage == 3 &&
           state.callback_mode == 3 && countdown == 1);
    assert(runtime.dispatch_runtime.record[14].bytes[1] & 0x40u);
    assert(runtime.root_route == FA18_SCENE_ROOT_PLACEMENT_NEGATIVE_APPLIED &&
           runtime.root_placement.selected_record_index == 14 &&
           runtime.root_placement.pose.position[1] == 0x7708);
    assert(runtime.message.delay == 0x1b8 && runtime.finalization.stage_word == 0x90);
    return 0;
}
